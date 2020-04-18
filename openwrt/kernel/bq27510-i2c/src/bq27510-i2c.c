#include <linux/module.h>
#include <linux/param.h>
#include <linux/jiffies.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/idr.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <asm/unaligned.h>

#include "bqfs_cmd_type.h"
#include "bqfs_image.h"

#define I2C_RETRY_CNT    3
#define BQGAUGE_I2C_ROM_ADDR    (0x16 >> 1)
#define BQGAUGE_I2C_DEV_ADDR    (0xAA >> 1)

/* bq27510 device stuff */
#define BQ27510_REG_CONTRL          0x00

#define BQ27510_REG_DFCLASS         0x3E
#define BQ27510_REG_DFBLOCK         0x3F
#define BQ27510_REG_BLOCKDATA       0x40

#define BQ27510_REG_BLKCHKSUM       0x60
#define BQ27510_REG_BLKDATCTL       0x61

#define BQ27510_SECURITY_FA         0x01
#define BQ27510_SECURITY_UNSEALED   0x02
#define BQ27510_SECURITY_SEALED     0x03

#define BQ27510_UNSEAL_KEY          0x36720414
#define BQ27510_FA_KEY              0xFFFFFFFF

#define BQ27510_SUBCMD_FLAGS		0x0000
#define BQ27510_SUBCMD_FWVER        0x0002
#define BQ27510_SUBCMD_ENTER_ROM    0x0F00

enum bqGauge_chip {BQ27510,BQ27320};
static DEFINE_MUTEX(battery_mutex);

struct bqGauge_device_info {
    struct  device        *dev;

    int     id;
    enum    bqGauge_chip    chip;
};

/* tool, support function */
static u8 checksum(u8 *data, u8 len)
{
    u16 sum = 0;
    int i;

    for (i = 0; i < len; i++)
        sum += data[i];

    sum &= 0xFF;

    return 0xFF - sum;
}

// common routines for bq I2C gauge                
static int bq_read_i2c_byte(struct bqGauge_device_info *di, u8 reg)
{
    struct i2c_client *client = to_i2c_client(di->dev);
    struct i2c_msg msg[2];
    unsigned char data;
    int ret;
    int i = 0;

    if (!client->adapter)
        return -ENODEV;

    msg[0].addr = client->addr;
    msg[0].flags = 0;
    msg[0].buf = &reg;
    msg[0].len = sizeof(reg);
    msg[1].addr = client->addr;
    msg[1].flags = I2C_M_RD;
    msg[1].buf = &data;
    msg[1].len = 1;

    mutex_lock(&battery_mutex);
    for(i = 0; i < I2C_RETRY_CNT; i++){
        ret = i2c_transfer(client->adapter, msg, ARRAY_SIZE(msg));
        if(ret >= 0) break;
        msleep(5);
    }
    mutex_unlock(&battery_mutex);
    if (ret < 0)
        return ret;

    ret = data;

    return ret;
}

static int bq_read_i2c_word(struct bqGauge_device_info *di, u8 reg)
{
    struct i2c_client *client = to_i2c_client(di->dev);
    struct i2c_msg msg[2];
    unsigned char data[2];
    int ret;
    int i = 0;

    if (!client->adapter)
        return -ENODEV;

    msg[0].addr = client->addr;
    msg[0].flags = 0;
    msg[0].buf = &reg;
    msg[0].len = sizeof(reg);
    msg[1].addr = client->addr;
    msg[1].flags = I2C_M_RD;
    msg[1].buf = data;
    msg[1].len = 2;

    mutex_lock(&battery_mutex);
    for(i = 0; i < I2C_RETRY_CNT; i++){
        ret = i2c_transfer(client->adapter, msg, ARRAY_SIZE(msg));
        if(ret >= 0) break;
        msleep(5);
    }
    mutex_unlock(&battery_mutex);
    if (ret < 0)
        return ret;

    ret = get_unaligned_le16(data);;

    return ret;
}

static int bq_write_i2c_byte(struct bqGauge_device_info *di, u8 reg, unsigned char value)
{
    struct i2c_client *client = to_i2c_client(di->dev);
    struct i2c_msg msg;
    unsigned char data[4];
    int ret;
    int i = 0;

    if (!client->adapter)
        return -ENODEV;

    data[0] = reg;
    data[1] = value;
    
    msg.len = 2;
    msg.buf = data;
    msg.addr = client->addr;
    msg.flags = 0;

    mutex_lock(&battery_mutex);
    for(i = 0; i < I2C_RETRY_CNT; i++){
        ret = i2c_transfer(client->adapter, &msg, 1);
        if(ret >= 0) break;
        msleep(5);
    }
    mutex_unlock(&battery_mutex);    
    if (ret < 0)
        return ret;

    return 0;
}

static int bq_write_i2c_word(struct bqGauge_device_info *di, u8 reg, int value)
{
    struct i2c_client *client = to_i2c_client(di->dev);
    struct i2c_msg msg;
    unsigned char data[4];
    int ret;
    int i = 0;

    if (!client->adapter)
        return -ENODEV;

    data[0] = reg;
    put_unaligned_le16(value, &data[1]);
    
    msg.len = 3;
    msg.buf = data;
    msg.addr = client->addr;
    msg.flags = 0;

    mutex_lock(&battery_mutex);
    for(i = 0; i < I2C_RETRY_CNT; i++){
        ret = i2c_transfer(client->adapter, &msg, 1);
        if(ret >= 0) break;
        msleep(5);
    }
    mutex_unlock(&battery_mutex);    
    if (ret < 0)
        return ret;

    return 0;
}

static int bq_read_i2c_blk(struct bqGauge_device_info *di, u8 reg, u8 *data, u8 len)
{

    struct i2c_client *client = to_i2c_client(di->dev);
    struct i2c_msg msg[2];
    int ret;
    int i = 0;

    if (!client->adapter)
        return -ENODEV;

    msg[0].addr = client->addr;
    msg[0].flags = 0;
    msg[0].buf = &reg;
    msg[0].len = 1;

    msg[1].addr = client->addr;
    msg[1].flags = I2C_M_RD;
    msg[1].buf = data;
    msg[1].len = len;

    mutex_lock(&battery_mutex);
    for(i = 0; i < I2C_RETRY_CNT; i++){
        ret = i2c_transfer(client->adapter, msg, ARRAY_SIZE(msg));
        if(ret >= 0) break;
        msleep(5);
    }
    mutex_unlock(&battery_mutex);    

    if (ret < 0)
        return ret;

    return ret;
}

static int bq_write_i2c_blk(struct bqGauge_device_info *di, u8 reg, u8 *data, u8 sz)
{
    struct i2c_client *client = to_i2c_client(di->dev);
    int ret;
    int i = 0;
    u8 write_data[8];

    if (!client->adapter)
        return -ENODEV;

	if (sz == 0) {
		dev_info(di->dev, "Write Len should not be zero!! \n");
		return -1;
	}

	write_data[0] = reg;

	while (sz > 7) {
		mutex_lock(&battery_mutex);
		for (i = 0; i < 7; i++) {
			write_data[i+1] = *(data+i);
		}
		ret = i2c_master_send(client, write_data, 7+1);
		mutex_unlock(&battery_mutex);

		if (ret < 0) {
			dev_info(di->dev, "Failed to write block data \n");
			return ret;
		}
		data+=7;
		sz-=7;
		write_data[0] = write_data[0]+7; /* Register increase */
	}

	if (sz > 0) {
		mutex_lock(&battery_mutex);
		for (i = 0; i < sz; i++){
			write_data[i+1] = *(data+i);
		}
		ret = i2c_master_send(client, write_data, sz+1);
		mutex_unlock(&battery_mutex);

		if (ret < 0) {
			dev_info(di->dev, "Failed to write block data \n");
			return ret;
		}
	}

	return 0;
#if 0
    buf[0] = reg;
    memcpy(&buf[1], data, sz);

    msg.buf = buf;
    msg.addr = client->addr;
    msg.flags = 0;
    msg.len = sz + 1;

    mutex_lock(&battery_mutex);
    for(i = 0; i < I2C_RETRY_CNT; i++){
        ret = i2c_transfer(client->adapter, &msg, 1);
        if(ret >= 0) break;
        msleep(5);
    }
    mutex_unlock(&battery_mutex);
    if (ret < 0)
        return ret;

    return 0;
#endif
}

/*
 * Version stuff
 */
static int bq27510_read_fw_version(struct bqGauge_device_info *di)
{
    int ret;
    
    ret = bq_write_i2c_word(di, BQ27510_REG_CONTRL, BQ27510_SUBCMD_FWVER);
    if(ret < 0){
        dev_err(di->dev,"Failed to send read fw version command\n");
        return ret;
    }
    mdelay(2);
    ret = bq_read_i2c_word(di, BQ27510_REG_CONTRL);
    if(ret < 0){
        dev_err(di->dev,"Failed to read read fw version \n");
        return ret;
    }

    dev_info(di->dev,"BQ27510 fw version: 0x%04x \n", ret);
 
    return ret;
  
}

static int bq27510_read_df(struct bqGauge_device_info * di,u8 classid, u8 offset, u8* buf, u8 len)
{
    int ret;
    u8 tmp_buf[40] = {0x00};
    int i;
    int crc;
    u8 crc_calc = 0;
    
    if (offset % 32 + len > 32) return -1; // less than one block boundary one time

    ret = bq_write_i2c_byte(di, BQ27510_REG_BLKDATCTL, 0);
    if(ret < 0) return ret;
    mdelay(2);
    ret = bq_write_i2c_byte(di, BQ27510_REG_DFCLASS, classid);
    if(ret < 0) return ret;
    mdelay(2);
    ret = bq_write_i2c_byte(di, BQ27510_REG_DFBLOCK, offset / 32);
    if(ret < 0) return ret;
    
    mdelay(2);
    ret = bq_read_i2c_blk(di, BQ27510_REG_BLOCKDATA, tmp_buf, 32);
    if(ret < 0) return ret;
    
    crc = bq_read_i2c_byte(di, BQ27510_REG_BLKCHKSUM);
    crc_calc = checksum(tmp_buf,32);
    if(crc != crc_calc) return -2;
    
    for(i = 0; i < len; i++){
        buf[i] =  tmp_buf[offset % 32 + i];
    }
    return len;
} 

static int bq27510_write_df(struct bqGauge_device_info * di,u8 classid, u8 offset, u8* buf, u8 len)
{
    int ret;
    u8 tmp_buf[40];
    int i;
    int crc;
    u8 crc_calc = 0;
    
    if (offset % 32 + len > 32) return -1; // less than one block one time

    ret = bq_write_i2c_byte(di,BQ27510_REG_BLKDATCTL, 0);
    if(ret < 0) return ret;
    mdelay(2);
    ret = bq_write_i2c_byte(di,BQ27510_REG_DFCLASS, classid);
    if(ret < 0) return ret;
    mdelay(2);
    ret = bq_write_i2c_byte(di,BQ27510_REG_DFBLOCK, offset / 32);
    if(ret < 0) return ret;
    
    mdelay(2);
    ret = bq_read_i2c_blk(di,BQ27510_REG_BLOCKDATA,tmp_buf, 32);
    if(ret < 0) return ret;
    
    mdelay(2);
    crc = bq_read_i2c_byte(di, BQ27510_REG_BLKCHKSUM);
    crc_calc = checksum(tmp_buf,32);
    if(crc != crc_calc) return -2;
    //update with new value
    for(i = 0; i < len; i++)
        tmp_buf[offset % 32 + i] = buf[i];
    // calculate new crc 
    crc_calc = checksum(tmp_buf,32);
    mdelay(2);
    ret = bq_write_i2c_blk(di,BQ27510_REG_BLOCKDATA,tmp_buf,32);
    if(ret < 0) return ret;
    
    mdelay(2);
    ret = bq_write_i2c_byte(di,BQ27510_REG_BLKCHKSUM,crc_calc);
    return ret;
} 

#define BQ27510_DEVICE_NAME_CLASSID     48
#define BQ27510_DEVICE_NAME_OFFSET      18
#define BQ27510_DEVICE_NAME_LENGTH      7

static bool bq27510_check_update_necessary(struct bqGauge_device_info * di)
{
    // this is application specific, return true if need update firmware or data flash
    u8 buf[40] = {0x00};
    int ret;
	int i = 0;

    ret = bq27510_read_df(di,BQ27510_DEVICE_NAME_CLASSID,BQ27510_DEVICE_NAME_OFFSET, buf, BQ27510_DEVICE_NAME_LENGTH);
    if( ret != BQ27510_DEVICE_NAME_LENGTH)
        return false;
    if(strncmp(buf,"VER4000",BQ27510_DEVICE_NAME_LENGTH) == 0)//same
        return false;
    else    
        return true;
}

static bool bq27510_mark_as_updated(struct bqGauge_device_info * di)
{
    // this is application specific
    int ret = 0;
    ret = bq27510_write_df(di,BQ27510_DEVICE_NAME_CLASSID,BQ27510_DEVICE_NAME_OFFSET, "VER4000", BQ27510_DEVICE_NAME_LENGTH);
    if(ret < 0) 
        return false;
    else    
        return true;
}

/*
 * ROM mode
 */
static bool bqGauge_check_rom_mode(struct bqGauge_device_info * di)
{
    struct i2c_client *client = to_i2c_client(di->dev);
    int ret;

    client->addr = BQGAUGE_I2C_ROM_ADDR;
    ret = bq_read_i2c_byte(di, 0x66);
    mdelay(2);
    client->addr = BQGAUGE_I2C_DEV_ADDR;//restore address
    if(ret < 0 ){ // not in rom mode 
        return false;
    }
    return true;
}

static bool bq27510_enter_rom_mode(struct bqGauge_device_info * di)
{
    int ret;
    
    ret = bq_write_i2c_word(di, BQ27510_REG_CONTRL, BQ27510_SUBCMD_ENTER_ROM);
    mdelay(2);
    if(ret < 0) return false;
    
    return bqGauge_check_rom_mode(di);
}

static bool bq27510_exit_rom_mode(struct bqGauge_device_info * di)
{
    int ret;
	struct i2c_client *client = to_i2c_client(di->dev);
    
	client->addr = BQGAUGE_I2C_ROM_ADDR; 
	ret = bq_write_i2c_byte(di, 0x00, 0x0F);
    mdelay(2);
    ret = bq_write_i2c_word(di, 0x64, 0x000F);
    mdelay(2);
    if(ret < 0) return false;
	mdelay(4000);
    
    return bqGauge_check_rom_mode(di);
}

/*
 * Seal stuff
 */
static int bq27510_check_seal_state(struct bqGauge_device_info * di)
{
    int status;
    bq_write_i2c_word(di, BQ27510_REG_CONTRL, BQ27510_SUBCMD_FLAGS);
    mdelay(2);
    status = bq_read_i2c_word(di, BQ27510_REG_CONTRL);
    if(status < 0) return status;
    
    if(((u16)status & 0x6000) == 0) //FA and SS neither set
        status = BQ27510_SECURITY_FA;
    else if(((u16)status & 0x2000) == 0) // SS not set
        status = BQ27510_SECURITY_UNSEALED;
    else    
        status = BQ27510_SECURITY_SEALED;

    return status;
}

static int bq27510_unseal(struct bqGauge_device_info * di)
{
    int ret;
    
    bq_write_i2c_word(di, BQ27510_REG_CONTRL, BQ27510_UNSEAL_KEY & 0xFFFF);
    mdelay(2);
    bq_write_i2c_word(di, BQ27510_REG_CONTRL, (BQ27510_UNSEAL_KEY >> 16)& 0xFFFF);
    mdelay(5);
    
    ret = bq27510_check_seal_state(di);
    if(ret == BQ27510_SECURITY_UNSEALED || ret == BQ27510_SECURITY_FA)
        return 1;
    else
        return 0;
}

static int bq27510_unseal_full_access(struct bqGauge_device_info * di)
{
    int ret;
    
    bq_write_i2c_word(di, BQ27510_REG_CONTRL, BQ27510_FA_KEY & 0xFFFF);
    mdelay(2);
    bq_write_i2c_word(di, BQ27510_REG_CONTRL, (BQ27510_FA_KEY >> 16)& 0xFFFF);
    mdelay(5);
    
    ret = bq27510_check_seal_state(di);
    if(ret == BQ27510_SECURITY_FA)
        return 1;
    else
        return 0;

}

/*
 * Update
 */
static bool bq27510_update_execute_cmd(struct bqGauge_device_info *di, const bqfs_cmd_t *cmd)
{
    int ret;
    uint8_t tmp_buf[CMD_MAX_DATA_SIZE];

    switch (cmd->cmd_type) {
    case CMD_R:
        ret = bq_read_i2c_blk(di, cmd->reg, (u8 *)&cmd->data.bytes, cmd->data_len);
        if( ret < 0) return false;
        return true;

    case CMD_W:
        ret = bq_write_i2c_blk(di,cmd->reg,(u8 *)&cmd->data.bytes, cmd->data_len);
        if(ret < 0) return false;
        return true;
    case CMD_C:
        if (bq_read_i2c_blk(di,cmd->reg,tmp_buf,cmd->data_len) < 0)
            return false;//read fail
        if (memcmp(tmp_buf, cmd->data.bytes, cmd->data_len)) {
            dev_dbg(di->dev, "\nCommand C failed at line %d:\n",
                cmd->line_num);
            return false;
        }
        return true;

    case CMD_X:
        mdelay(cmd->data.delay);
        return true;

    default:
        dev_err(di->dev, "Unsupported command at line %d\n",
            cmd->line_num);
        return false;
    }
}

static void bq27510_update_bqfs(struct bqGauge_device_info *di)
{
    struct i2c_client *client = to_i2c_client(di->dev);
    u16 i;

	/*
	dev_info(di->dev,"Check device is ROM mode or not \n");
    if(bqGauge_check_rom_mode(di)) goto update;
	*/

    if(!bq27510_check_update_necessary(di)) {
		dev_info(di->dev,"Device need to be updated \n");
		return;
	}

    if (bq27510_check_seal_state(di) != BQ27510_SECURITY_FA){
        if(!bq27510_unseal(di)) 
            return;
        mdelay(10);
        if(!bq27510_unseal_full_access(di)) 
            return;
    }

    if(!bq27510_enter_rom_mode(di)) return;
	dev_info(di->dev,"Device success enter ROM mode \n");
    
update:    
    client->addr = BQGAUGE_I2C_ROM_ADDR;
    dev_info(di->dev,"Updating");
    for(i = 0; i < ARRAY_SIZE(bqfs_image); i++){
        /* dev_info(di->dev,"."); */
		/* printk(KERN_INFO "."); */
        if(!bq27510_update_execute_cmd(di,&bqfs_image[i])){
            dev_err(di->dev,"%s:Failed at command:%d\n",__func__,i);
            return;
        }
    }
    dev_info(di->dev,"Done!\n");

    client->addr = BQGAUGE_I2C_DEV_ADDR;    
    // mark as updated
    /* bq27510_mark_as_updated(di); */
    
    return ;
}

static int  bqGauge_battery_probe(struct i2c_client *client,
                 const struct i2c_device_id *id)
{
    int retval = 0;
    struct bqGauge_device_info *di;

    di = kzalloc(sizeof(*di), GFP_KERNEL);
    if (!di) {
        dev_err(&client->dev, "failed to allocate device info data\n");
        retval = -ENOMEM;
		return retval;
    }

    di->dev = &client->dev;
    di->chip = id->driver_data;

	if (di->chip == BQ27510) {
		dev_info(&client->dev, "Detect device is BQ27510 \n");
	} else {
		dev_err(&client->dev, "Unexpected gas gague: %d\n", di->chip);
	}

    i2c_set_clientdata(client, di);

	bq27510_read_fw_version(di);

	bq27510_update_bqfs(di);
	return retval;
}

static int bqGauge_battery_shutdown(struct i2c_client *client)
{
	int ret = 0;
	return ret;
}

static int bqGauge_battery_remove(struct i2c_client *client)
{
	struct bqGauge_device_info *di = i2c_get_clientdata(client);
	kfree(di);

	return 0;
}

static const struct i2c_device_id bqGauge_id[] = {
    { "bq27510", BQ27510 },
    {},
};
MODULE_DEVICE_TABLE(i2c, bqGauge_id);

#if 0
static const struct of_device_id bqGauge_match[] = {
	{ .compatible = "bq27510" },
};
MODULE_DEVICE_TABLE(of, bqGauge_match);
#endif

static struct i2c_driver bqGauge_battery_driver = {
    .driver = {
        .name = "bq27510-i2c",
		.owner = THIS_MODULE,
#if 0
		.of_match_table = bqGauge_match,
#endif
    },
    .probe      = bqGauge_battery_probe,
    .shutdown   = bqGauge_battery_shutdown,
    .remove     = bqGauge_battery_remove,
	.id_table = bqGauge_id,
};

static inline int  bqGauge_battery_i2c_init(void)
{
	printk(KERN_INFO "bq27510 module init\n");
    int ret = i2c_add_driver(&bqGauge_battery_driver);
    if (ret)
        printk(KERN_ERR "Unable to register bqGauge i2c driver\n");

    return ret;
}

static inline void __exit bqGauge_battery_i2c_exit(void)
{
	printk(KERN_INFO "bq27510 module exit\n");
    i2c_del_driver(&bqGauge_battery_driver);
}


/*
 * Module stuff
 */
static int  bqGauge_battery_init(void)
{
    int ret;

    ret = bqGauge_battery_i2c_init();

    return ret;
}
module_init(bqGauge_battery_init);

static void bqGauge_battery_exit(void)
{
    bqGauge_battery_i2c_exit();
}
module_exit(bqGauge_battery_exit);

MODULE_AUTHOR("Kyson Lok <kysonlok@gmail.com>");
MODULE_DESCRIPTION("bqGauge battery update driver");
MODULE_LICENSE("GPL");

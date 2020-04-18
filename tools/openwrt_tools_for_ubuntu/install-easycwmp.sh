# Fork from https://gist.github.com/claudyus/b57bfc1dc16aa44ae411

USER=ubuntu
GROUP=ubuntu

sudo mkdir -p /opt/{dev,git} || exit 0
sudo chown -R $USER:$GROUP /opt/{dev,git}

git clone git://github.com/json-c/json-c.git /opt/git/json-c
cd /opt/git/json-c/

autoreconf -i
./configure --prefix=/usr

make

sudo make install
sudo ln -sf /usr/include/json-c /usr/include/json

#libubox
git clone git://nbd.name/luci2/libubox.git /opt/git/libubox
cd /opt/git/libubox/

git checkout 0608d1299546d4af1facc271a090cb2abb8c6105
cmake CMakeLists.txt -DBUILD_LUA=OFF

make

sudo make install
sudo ln -sf /usr/local/lib/libubox.so /usr/lib/libubox.so
sudo mkdir -p /usr/share/libubox
sudo ln -sf /usr/local/share/libubox/jshn.sh /usr/share/libubox/jshn.sh

#uci
git clone git://nbd.name/uci.git /opt/git/uci
cd /opt/git/uci/

git checkout b42ee8f21842fab41a4cdf27960000bb1b3f42a7
cmake CMakeLists.txt -DBUILD_LUA=OFF

make

class="western"
sudo make install
sudo ln -sf /usr/local/bin/uci /sbin/uci
sudo ln -sf /usr/local/lib/libuci.so /usr/lib/libuci.so

#ubus
git clone git://nbd.name/luci2/ubus.git /opt/git/ubus
cd /opt/git/ubus/

git checkout 4e82a1fabb87b5e3c948a792e16b0fac3702721b

cmake CMakeLists.txt -DBUILD_LUA=OFF

make

sudo make install
sudo ln -sf /usr/local/sbin/ubusd /usr/sbin/ubusd
sudo ln -sf /usr/local/lib/libubus.so /usr/lib/libubus.so

#microxml
git clone git://dev.freecwmp.org/microxml /opt/git/microxml
cd /opt/git/microxml/

git checkout a62e2f8c15a65efb1a82404d8f31e3bfc1cbe7d9
autoconf -i

./configure --prefix=/usr --enable-threads --enable-shared --enable-static

make

sudo make install
sudo ln -sf /usr/lib/libmicroxml.so.1.0 /lib/libmicroxml.so
sudo ln -sf /usr/lib/libmicroxml.so.1.0 /lib/libmicroxml.so.1

#shflags
svn co http://shflags.googlecode.com/svn/trunk/source/1.0/lib /opt/dev/shflags
cd /opt/dev/shflags/

sudo mkdir -p /usr/share/shflags/
sudo cp shflags /usr/share/shflags/shflags.sh

#easycwmp
cd /opt/dev/
wget http://easycwmp.org/download/easycwmp-1.0.3.tar.gz
tar -xzvf easycwmp-1.0.3.tar.gz
mv easycwmp-1.0.3 easycwmp
cd /opt/dev/easycwmp/

autoreconf -i

./configure --enable-debug --enable-devel --enable-acs=multi --enable-jsonc=1

make

cd /opt/dev/easycwmp/

sudo mkdir -p /usr/share/easycwmp/functions
sudo mkdir -p /etc/easycwmp

sudo ln -sf /opt/dev/easycwmp/ext/openwrt/scripts/easycwmp.sh /usr/sbin/easycwmp
sudo ln -sf /opt/dev/easycwmp/ext/openwrt/scripts/defaults /usr/share/easycwmp/defaults
sudo ln -sf /opt/dev/easycwmp/ext/openwrt/scripts/functions/common /usr/share/easycwmp/functions/common
sudo ln -sf /opt/dev/easycwmp/ext/openwrt/scripts/functions/device_info /usr/share/easycwmp/functions/device_info
sudo ln -sf /opt/dev/easycwmp/ext/openwrt/scripts/functions/lan_device /usr/share/easycwmp/functions/lan_device
sudo ln -sf /opt/dev/easycwmp/ext/openwrt/scripts/functions/management_server /usr/share/easycwmp/functions/management_server
sudo ln -sf /opt/dev/easycwmp/ext/openwrt/scripts/functions/wan_device /usr/share/easycwmp/functions/wan_device

sudo mkdir /etc/config
sudo ln -sf /opt/dev/easycwmp/ext/openwrt/config/easycwmp /etc/config/easycwmp

sudo ln -sf /opt/dev/easycwmp/bin/easycwmpd /usr/sbin/easycwmpd

export UCI_CONFIG_DIR="/opt/dev/easycwmp/ext/openwrt/config/"
export UBUS_SOCKET="/tmp/ubus.sock"

sudo mkdir -p /lib/{config,functions}
sudo wget http://pastebin.lukaperkov.net/openwrt/20121219_lib_functions.sh -O /lib/functions.sh
sudo wget http://pastebin.lukaperkov.net/openwrt/20121219_lib_config_uci.sh -O /lib/config/uci.sh
sudo wget http://pastebin.lukaperkov.net/openwrt/20121219_lib_functions_network.sh -O /lib/functions/network.sh

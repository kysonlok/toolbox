#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#included "sys_gpio.h"

/*
 * Constants
 */
#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */
#define MAX_BUF 64
#define MAX_VALUE 10

typedef enum {
	GPIO_MODE_GET_DIR,
	GPIO_MODE_SET_DIR,
	GPIO_MODE_GET_EDGE,
	GPIO_MODE_SET_EDGE,
	GPIO_MODE_GET_VALUE,
	GPIO_MODE_SET_VALUE,
	GPIO_MODE_POLL
} gpio_mode_t;

/* export and unexport */
int _gpio_export(unsigned int gpio);
int _gpio_unexport(unsigned int gpio);

/* set and get value */
int _gpio_set_value(unsigned int gpio, char *value);
int _gpio_get_value(unsigned int gpio, char *value, unsigned int len);

/* set and get direction */
int _gpio_set_direction(unsigned int gpio, char *direction);
int _gpio_get_direction(unsigned int gpio, char *direction, unsigned int len);

/* set and get edge */
int _gpio_set_edge(unsigned int gpio, char *edge);
int _gpio_get_edge(unsigned int gpio, char *edge, unsigned int len);

/* open and close */
int _gpio_open(unsigned int gpio);
int _gpio_close(int fd);

/* gpio_poll */
int _gpio_poll(unsigned int gpio, int timeout);

/* gpio_usage */
/* void _gpio_usage(char *name); */

/* gpio_parse_args */
/* int _gpio_parse_args(int argc, char **argv, unsigned int *mode, unsigned int *gpio, char **value); */

/* gpio ioctl */
/* int sysfs_gpio_ioctl(unsigned int pin, unsigned int mode, char *buf); */

int _gpio_export(unsigned int gpio)
{
	int status = -1;
	int fd, len;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		status=fd;
	} else {
		len = snprintf(buf, sizeof(buf), "%d", gpio);
		write(fd, buf, len);
		close(fd);
		status=0;
	}
	return status;
}

int _gpio_unexport(unsigned int gpio)
{
	int status=-1;
	int fd, len;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		status=fd;
	} else {
		len = snprintf(buf, sizeof(buf), "%d", gpio);
		write(fd, buf, len);
		close(fd);
		status=0;
	}		

	return status;
}

int _gpio_set_value(unsigned int gpio, char *value)
{
	int status = -1;
	int fd, len;
	char buf[MAX_BUF];

	if ((strcmp(value, "0")==0) || (strcmp(value, "1")==0)) {
		len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

		fd = open(buf, O_WRONLY);
		if (fd < 0) {
			perror("gpio/set-value");
			status = fd;
		} else {
			write(fd, value, 2);
			close(fd);
			status = 0;
		}
	}

	return status;
}

int _gpio_get_value(unsigned int gpio, char *value, unsigned int len)
{
	int status = -1;
	int fd;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

	fd = open(buf, O_RDONLY);
	if (fd < 0) {
		perror("gpio/get-value");
		status=fd;
	} else {
		memset(value, 0x00, len);
		read(fd, value, len - 1);
		status = 0;
	}

	close(fd);
	return status;
}

int _gpio_set_direction(unsigned int gpio, char *direction)
{
	int status = -1;
	int fd, len;
	char buf[MAX_BUF];

	if ((strcmp(direction, "out")==0) || (strcmp(direction, "in")==0)) {
		len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);

		fd = open(buf, O_WRONLY);
		if (fd < 0) {
			perror("gpio/direction");
			status = fd;
		} else {
			write(fd, direction, strlen(direction) + 1);
			close(fd);
			status = 0;
		}
	}

	return status;
}

int _gpio_get_direction(unsigned int gpio, char *direction, unsigned int len)
{
	int status = -1;
	int fd;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);

	fd = open(buf, O_RDONLY);
	if (fd < 0) {
		perror("gpio/direction");
		status = fd;
	} else {
		memset(direction, 0x00, len);
		read(fd, direction, len - 1);
		close(fd);
		status = 0;
	}
	return status;
}

int _gpio_set_edge(unsigned int gpio, char *edge)
{
	int status = -1;
	int fd, len;
	char buf[MAX_BUF];

	if ((strcmp(edge, "rising")==0) || (strcmp(edge, "falling")==0) ||
	    (strcmp(edge, "high")==0)   || (strcmp(edge, "low")==0)) {

		len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", gpio);

		fd = open(buf, O_WRONLY);
		if (fd < 0) {
			perror("gpio/set-edge");
			status = fd;
		} else {
			write(fd, edge, strlen(edge) + 1);
			close(fd);
			status = 0;
		}
	}

	return status;
}

int _gpio_get_edge(unsigned int gpio, char *edge, unsigned int len)
{
	int status = -1;
	int fd;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", gpio);

	fd = open(buf, O_RDONLY);
	if (fd < 0) {
		perror("gpio/set-edge");
		status = fd;
	} else {
		memset(edge, 0x00, len);
		read(fd, edge, len - 1);
		close(fd);
		status = 0;
	}

	return status;
}

int _gpio_open(unsigned int gpio)
{
	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

	fd = open(buf, O_RDONLY | O_NONBLOCK );
	if (fd < 0) {
		perror("gpio/fd_open");
	}

	return fd;
}

int _gpio_close(int fd)
{
	return close(fd);
}

int _gpio_poll(unsigned int gpio, int timeout)
{
	struct pollfd fdset[2];
	int nfds = 2;
	int gpio_fd, rc;
	char *buf[MAX_BUF];
	int len;

	gpio_fd = _gpio_open(gpio);

	while (1) {
		memset((void*)fdset, 0, sizeof(fdset));

		fdset[0].fd = STDIN_FILENO;
		fdset[0].events = POLLIN;

		fdset[1].fd = gpio_fd;
		fdset[1].events = POLLPRI;

		rc = poll(fdset, nfds, timeout);

		if (rc < 0) {
			printf("\npoll() failed!\n");
			return -1;
		}

		if (rc == 0) {
			printf(".");
		}

		if (fdset[1].revents & POLLPRI) {
			len = read(fdset[1].fd, buf, MAX_BUF);
			printf("\npoll() GPIO %d interrupt occurred\n", gpio);
		}

		if (fdset[0].revents & POLLIN) {
			(void)read(fdset[0].fd, buf, 1);
			printf("\npoll() stdin read 0x%2.2X\n", (unsigned int) buf[0]);
		}
	}

	_gpio_close(gpio_fd);

	return 0;
}


int sysfs_gpio_ioctl(unsigned int pin, unsigned int mode, char *buf)
{
	int ret = -1;
	int len = 0;
	unsigned int gpio = pin;

	/* Check args */
	if (mode<0 || mode>6) {
		mode = 0;
	}
	if (mode==GPIO_MODE_GET_DIR || 
			mode==GPIO_MODE_GET_EDGE ||
			mode==GPIO_MODE_GET_VALUE) {
		memset(buf, 0, MAX_BUF);
	}
	if (buf == NULL) {
		printf("sysfs_gpio_ioctl: invalid param, buf is null\n");
		return ret;
	}

	switch (mode) {
		case GPIO_MODE_GET_DIR:
			ret = _gpio_get_direction(gpio, buf, MAX_VALUE);
			break;
		case GPIO_MODE_SET_DIR:
			ret = _gpio_set_direction(gpio, buf);
		    break;
		case GPIO_MODE_GET_EDGE:
			ret = _gpio_get_edge(gpio, buf, MAX_VALUE);
		    break;
		case GPIO_MODE_SET_EDGE:
			ret = _gpio_set_edge(gpio, buf);
		    break;
		case GPIO_MODE_GET_VALUE:
			ret = _gpio_get_value(gpio, buf, MAX_VALUE);
		    break;
		case GPIO_MODE_SET_VALUE:
			ret = _gpio_set_value(gpio, buf);
		    break;
		case GPIO_MODE_POLL:
			ret = _gpio_poll(gpio, POLL_TIMEOUT);
		    break;
	}

	return ret;
}


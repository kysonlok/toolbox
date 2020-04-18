#ifndef SYSFS_GPIO_H_
#define SYSFS_GPIO_H_

/*
 * sysfs_gpio_ioctl - Operating sysfs's gpio
 * @pin: The GPIO pin number on which the action should be done
 * @mode: The action to perform on the GPIO, the value as follow:
 *  0 - get_direction: get the GPIO direction 
 *  1 - set_direction: change the GPIO direction
 *  2 - get_edge: get the GPIO interrupt edge
 *  3 - set_edge: change the GPIO interrupt edge
 *  4 - get_value: get the GPIO state
 *  5 - set_value: change the GPIO state
 *  6 - poll: Waits for a change in the GPIO pin voltage level or input on stdin
 * @buf: There three condition for the value of buf
 *  * input: the buf is the value to set for all set action
 *  * output: the buf is the value to get for all get action
 *  * NULL: if action is poll, it should be NULL
 */
int sysfs_gpio_ioctl(unsigned int pin, unsigned int mode, char *buf);

#endif

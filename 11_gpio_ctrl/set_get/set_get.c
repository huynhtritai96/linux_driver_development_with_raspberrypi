#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio/consumer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MPCoding - LDD");
MODULE_DESCRIPTION("GPIO set get Example");

static const char *device_name = "gpio_ctrl";

#define LED_GPIO 21
#define BUTTON_GPIO 20
#define GPIO_OFFSET 512

static struct gpio_desc *led, *button;
static int led_gpio = (LED_GPIO + GPIO_OFFSET);
static int button_gpio = (BUTTON_GPIO + GPIO_OFFSET);

static int __init my_init(void)
{
    int status;
	led = gpio_to_desc(led_gpio);
	if (!led)
	{
		pr_err("%s: unable to get led_gpio 21\n", device_name);
		return -1;
	}

	button = gpio_to_desc(button_gpio);
	if (!button)
	{
		pr_err("%s: unable to get button_gpio 20\n", device_name);
		return -1;
	}

	// Set pin 21(LED) as GPIO output
	status = gpiod_direction_output(led, 0);
	if (status)
	{
		pr_err("%s: unable to set GPIO 21 as output\n", device_name);
		return -1;
	}
	
	// Set pin 20(BUTTON) as GPIO input
	status = gpiod_direction_input(button);
	if (status)
	{
		pr_err("%s: unable to set GPIO 20 as input\n", device_name);
		return -1;
	}

	gpiod_set_value(led, 1); // Turn ON

	pr_info("%s: Button state is: %d\n", device_name, gpiod_get_value(button));

	return 0;
}

static void __exit my_exit(void)
{
    gpiod_set_value(led, 0); // Turn OFF
	pr_info("%s: Goodbye Kernel\n", device_name);
}

module_init(my_init);
module_exit(my_exit);
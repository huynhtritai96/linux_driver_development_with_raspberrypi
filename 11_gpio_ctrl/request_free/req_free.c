#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MPCoding - LDD");
MODULE_DESCRIPTION("GPIO Requset Example");

static const char *device_name = "gpio_ctrl";

#define LED_GPIO        21
#define BUTTON_GPIO     20
#define GPIO_OFFSET     512

static int led_gpio     = (LED_GPIO    + GPIO_OFFSET);      // GPIO number for LED (physical pin 40)
static int button_gpio  = (BUTTON_GPIO + GPIO_OFFSET);   // GPIO number for BUTTON (physical pin 38)

static int __init my_init(void)
{
    int status;

    // Request GPIO for LED
    status = gpio_request(led_gpio, "led_gpio");
    if (status)
    {
        pr_err("%s: Failed to request led gpio 21\n", device_name);
        return -status;
    }

    // Set GPIO direction for LED
    status = gpio_direction_output(led_gpio, 0); // Set as output and initialize to LOW
    if (status)
    {
        pr_err("%s: Failed to set gpio 21 led direction\n", device_name);
        gpio_free(led_gpio);
        return -status;
    }

    // Request GPIO for BUTTON
    status = gpio_request(button_gpio, "button_gpio");
    if (status)
    {
        pr_err("%s: Failed to request button gpio 20\n", device_name);
        gpio_free(led_gpio);
        return -status;
    }

    // Set GPIO direction for BUTTON
    status = gpio_direction_input(button_gpio); // Set as input
    if (status)
    {
        pr_err("%s: Failed to set gpio 20 button direction\n", device_name);
        gpio_free(led_gpio);
        gpio_free(button_gpio);
        return -status;
    }

    gpio_set_value(led_gpio, 1);    // Turn ON LED

    pr_info("%s: Button state is: %d\n"
            "%s: GPIO request example loaded\n",
            device_name, gpio_get_value(button_gpio),
            device_name);

    return 0;
}

static void __exit my_exit(void)
{
    gpio_set_value(led_gpio, 0);    // Turn OFF LED
    gpio_free(led_gpio);
    gpio_free(button_gpio);
    pr_info("%s: Goodbye Kernel\n", device_name);
}

module_init(my_init);
module_exit(my_exit);
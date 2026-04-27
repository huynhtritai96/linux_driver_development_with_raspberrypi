#define USE_DESCRIPTOR_API 

#include <linux/module.h>
#include <linux/init.h>

#ifdef USE_DESCRIPTOR_API 
#include <linux/gpio/consumer.h>
#else
#include <linux/gpio.h> // legacy
#endif

#include <linux/interrupt.h>
#include <linux/timer_types.h> 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MPCoding - LDD");
MODULE_DESCRIPTION("GPIO IRQ Example");

static const char *device_name = "gpio_ctrl";

#define LED_GPIO        21
#define BUTTON_GPIO     20
#define GPIO_OFFSET     512

static int led_gpio     = (LED_GPIO     + GPIO_OFFSET); // GPIO number for LED (physical pin 21)
static int button_gpio  = (BUTTON_GPIO  + GPIO_OFFSET); // GPIO number for button (physical pin 20)

#ifdef USE_DESCRIPTOR_API
static struct gpio_desc *led, *button;
#endif

/* Variable contains iqr number */
unsigned int irq_number;

/* debounsing */
#define DEBOUNCE_DELAY 20 // 20ms

static int last_button_state;               // Variable to store the last button state for debouncing
static struct timer_list debounce_timer;    // Timer for debouncing

/* GPIO ISR */
static irqreturn_t button_isr(int irq, void *dev_id)
{
    pr_info("%s: Interrupt occoured on GPIO 20\n", device_name);

#ifdef USE_DESCRIPTOR_API
    last_button_state = gpiod_get_value(button);
#else
    last_button_state = gpio_get_value(button_gpio);
#endif
    
    /* Start the debounce timer 
        mod_timer:  This function is used to modify the expiration time of an existing timer. 
                    If the timer is not currently active, it will be added to the kernel's timer list. 
                    If the timer is already active, its expiration time will be updated to the new value provided.
        &debounce_timer:    This is a pointer to the timer_list structure that represents the debounce timer. 
                            This structure contains information about the timer, including its callback function and expiration time.
        jiffies + msecs_to_jiffies(DEBOUNCE_DELAY): This sets the expiration time for the timer. 
                                                    jiffies is a global variable that represents the number of timer ticks that have
        msecs_to_jiffies(DEBOUNCE_DELAY) converts the debounce delay from milliseconds to jiffies, which is the unit of time used by the kernel for timers.
     */
    mod_timer(&debounce_timer, jiffies + msecs_to_jiffies(DEBOUNCE_DELAY));
    
    return IRQ_HANDLED;
}

/* Debounce timer callback function */
static void debounce_timer_callback(struct timer_list *t)
{

#ifdef USE_DESCRIPTOR_API
    int state = gpiod_get_value(button);        // Get the current state of the button GPIO using the descriptor API
#else
    int state = gpio_get_value(button_gpio);
#endif

    if (state == last_button_state) // Check if the button state is stable (same as the last state read in the ISR)
    {
        pr_info("%s: Valid button press detected\n", device_name);

#ifdef USE_DESCRIPTOR_API
        gpiod_set_value(led, !gpiod_get_value(led));        // Toggle the LED state using the descriptor API
#else
        gpio_set_value(led_gpio, !gpio_get_value(led_gpio));
#endif

    }
}

static int __init my_init(void)
{
    int status;

// USE_DESCRIPTOR_API: Request GPIOs using the descriptor API and set their directions.
// LEGACY: Request GPIOs using the legacy API and set their directions.
#ifdef USE_DESCRIPTOR_API
    // Get GPIO descriptors for LED
    led = gpio_to_desc(led_gpio);
    if (!led)
    {
		pr_err("%s: unable gpio_to_desc led_gpio 21\n", device_name);
		return -1;
	}

    // Get GPIO descriptor for BUTTON
    button = gpio_to_desc(button_gpio);
	if (!button)
    {
		pr_err("%s: unable gpio_to_desc button_gpio 20\n", device_name);
		return -1;
	}

    // Set pin 21(LED) as GPIO output
    status = gpiod_direction_output(led, 0); // Set as output and initialize to LOW
    if (status)
    {
        pr_err("%s: Failed to set gpio 20 led direction\n", device_name);
        return -status;
    }

    // Set pin 20(BUTTON) as GPIO input
    status = gpiod_direction_input(button);
    if (status)
    {
        pr_err("%s: Failed to set gpio 21 button direction\n", device_name);
        return -status;
    }

#else
    status = gpio_request(led_gpio, "led_gpio");
    if (status)
    {
        pr_err("%s: Failed to request led gpio 21\n", device_name);
        return -status;
    }

    status = gpio_request(button_gpio, "button_gpio");
    if (status)
    {
        pr_err("%s: Failed to request button gpio 20\n", device_name);
        gpio_free(led_gpio);
        return -status;
    }

    status = gpio_direction_output(led_gpio, 0);
    if (status)
    {
        pr_err("%s: Failed to set gpio 20 led direction\n", device_name);
        gpio_free(led_gpio);
        return -status;
    }

    status = gpio_direction_input(button_gpio);
    if (status)
    {
        pr_err("%s: Failed to set gpio 21 button direction\n", device_name);
        gpio_free(led_gpio);
        gpio_free(button_gpio);
        return -status;
    }
#endif

// USE_DESCRIPTOR_API: Get the IRQ number associated with the button GPIO descriptor using gpiod_to_irq() function.
// LEGACY: Get the IRQ number associated with the button GPIO number using gpio_to_irq() function.
#ifdef USE_DESCRIPTOR_API
    irq_number = gpiod_to_irq(button);      // Get the IRQ number associated with the button GPIO descriptor
#else
    irq_number = gpio_to_irq(button_gpio);  // Get the IRQ number associated with the button GPIO number
#endif

    status = request_irq(irq_number, button_isr, IRQF_TRIGGER_FALLING, "btn_irq_handler", NULL);
    if (status)
    {
        pr_err("%s: IRQ request failed\n", device_name);

#ifndef USE_DESCRIPTOR_API
        gpio_free(led_gpio);
        gpio_free(button_gpio);
#endif

        return -status;
    }
    pr_info("%s: irq_number=%d \n", device_name, irq_number);

    timer_setup(&debounce_timer, debounce_timer_callback, 0);

    pr_info("%s: GPIO request example loaded\n", device_name);

    return 0;
}

static void __exit my_exit(void)
{
#ifdef USE_DESCRIPTOR_API
    gpiod_set_value(led, 0);
#else  
    gpio_set_value(led_gpio, 0);
    gpio_free(led_gpio);
    gpio_free(button_gpio);
#endif
    free_irq(irq_number, NULL);
    del_timer_sync(&debounce_timer);
    pr_info("%s: Goodbye Kernel\n", device_name);
}

module_init(my_init);
module_exit(my_exit);
#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MPCoding - LDD");
MODULE_DESCRIPTION("GPIO top bottom half of IRQ Example");

static const char *device_name = "gpio_ctrl";

#define LED_GPIO        21
#define BUTTON_GPIO     20
#define GPIO_OFFSET     512

static int led_gpio     = (LED_GPIO     + GPIO_OFFSET); // GPIO number for LED (physical pin 21)
static int button_gpio  = (BUTTON_GPIO  + GPIO_OFFSET); // GPIO number for button (physical pin 20)

/* Variable contains iqr number */
unsigned int irq_number;                // Variable to store the IRQ number associated with the button GPIO
static struct work_struct button_work;  // Work structure for the bottom-half workqueue. 
                                        // This structure will be used to schedule work that needs to be done in process context after the ISR is executed.

/* Top-half GPIO ISR */
static irqreturn_t button_isr(int irq, void *dev_id)
{
    pr_info("%s: Interrupt occoured on GPIO 20\n", device_name);
    
    schedule_work(&button_work); // Schedule the bottom-half work to be executed later in process context
    
    return IRQ_HANDLED;
}


/* Bottom-half (Workqueue) */
static void button_work_handler(struct work_struct *work)
{
    pr_info("%s: Bottom-half: processing button press\n", device_name);

    msleep(1000); /* simulate slow work */

    gpio_set_value(led_gpio, !gpio_get_value(led_gpio)); // Toggle the LED state by reading the current state with gpio_get_value() and setting it to the opposite value with gpio_set_value()
}


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

    // Set LED GPIO as output
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

    // Set BUTTON GPIO as input
    status = gpio_direction_input(button_gpio);
    if (status)
    {
        pr_err("%s: Failed to set gpio 21 button direction\n", device_name);
        gpio_free(led_gpio);
        gpio_free(button_gpio);
        return -status;
    }

    // Get the IRQ number associated with the button GPIO using gpio_to_irq() function and request an interrupt line for it.
    /* gpio_to_irq(): This function is used to get the IRQ number associated with a GPIO pin. 
       request_irq(): This function is used to request an interrupt line and associate it with an ISR. 
                        It takes the IRQ number, the ISR function, flags for the interrupt trigger type, a name for the interrupt handler, and a device ID (which can be NULL in this case). 
                        The flags used here (IRQF_TRIGGER_FALLING) indicate that the interrupt should be triggered on a falling edge (when the button is pressed).
    */
    irq_number = gpio_to_irq(button_gpio);
    status = request_irq(irq_number, button_isr, IRQF_TRIGGER_FALLING, "btn_irq_handler", NULL);
    if (status)
    {
        pr_err("%s: IRQ request failed\n", device_name);
        gpio_free(led_gpio);
        gpio_free(button_gpio);
        return -status;
    }
    pr_info("%s: irq_number=%d \n", device_name, irq_number);

    INIT_WORK(&button_work, button_work_handler); 
    // Initialize the work structure with the button_work_handler function, which will be called when the work is executed.
    /* Read the initial state of the button GPIO and store it in last_button_state variable. 
        This is used for debouncing to ensure that we only process valid button presses. 
        The state is read using gpio_get_value() function, which returns the current value of the GPIO pin (0 or 1).
    */

    pr_info("%s: GPIO request example loaded\n", device_name);

    return 0;
}

static void __exit my_exit(void)
{
    gpio_set_value(led_gpio, 0);
    gpio_free(led_gpio);
    gpio_free(button_gpio);
    free_irq(irq_number, NULL);

    cancel_work_sync(&button_work);

    pr_info("%s: Goodbye Kernel\n", device_name);
}

module_init(my_init);
module_exit(my_exit);
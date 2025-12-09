/* Pull in zephyr kernel facilities, e.g. k_sleep() */
#include <zephyr/kernel.h>
/* Include Zephyr's device model types and helpers */
#include <zephyr/device.h>
/* Access the C macros for reading information from the devicetree */
#include <zephyr/devicetree.h>
/* Bring in the GPIO driver API and the typed GPIO helpers required */
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define MYLED     DT_ALIAS(led2)

static const struct gpio_dt_spec led_1 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led_2 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec myled = GPIO_DT_SPEC_GET(MYLED, gpios);

int main(void)
{
    volatile int ret;

    if (!gpio_is_ready_dt(&led_1)) {
        printk("Error: LED device not ready\n");
        return 0;
    }

    if (!gpio_is_ready_dt(&led_2)) {
        printk("Error: LED device not ready\n");
        return 0;
    }    

    if (!gpio_is_ready_dt(&myled)) {
        printk("Error: LED device not ready\n");
        return 0;
    }

    ret = gpio_pin_configure_dt(&led_1, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        printk("Error: Could not configure LED\n");
        return 0;
    }
    ret = gpio_pin_configure_dt(&led_2, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        printk("Error: Could not configure LED\n");
        return 0;
    }    
    ret = gpio_pin_configure_dt(&myled, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        printk("Error: Could not configure LED\n");
        return 0;
    }        

    printk("Starting blinky on %s\n", CONFIG_BOARD);
    printk("Green LED initial status: %d\n", gpio_pin_get_dt(&led_1));
    printk("Red LED initial status: %d\n", gpio_pin_get_dt(&led_2));
    printk("Blue LED initial status: %d\n", gpio_pin_get_dt(&myled));

    while (1) {
        if (gpio_pin_toggle_dt(&led_1) < 0) {
            printk("Error: Could not toggle LED\n");
            return 0;
        }
        printk("Toggled Green LED\n");
        printk("Green %d\n", gpio_pin_get_dt(&led_1));        
        k_msleep(1000);
        if (gpio_pin_toggle_dt(&led_2) < 0) {
            printk("Error: Could not toggle LED\n");
            return 0;
        }
        printk("Toggled Red LED\n");
        printk("Red %d\n", gpio_pin_get_dt(&led_2));
        k_msleep(1000);        

        if (gpio_pin_toggle_dt(&myled) < 0) {
            printk("Error: Could not toggle LED\n");
            return 0;
        }
        printk("Toggled Blue LED\n");
        printk("Blue %d\n", gpio_pin_get_dt(&myled));
        k_msleep(1000);                
    }
    return 0;
}
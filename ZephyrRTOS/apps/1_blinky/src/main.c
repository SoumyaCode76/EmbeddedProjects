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

static const struct gpio_dt_spec led_1 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led_2 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

int main(void)
{
    int ret;

    if (!gpio_is_ready_dt(&led_1)) {
        printk("Error: LED device not ready\n");
        return 0;
    }

    if (!gpio_is_ready_dt(&led_2)) {
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

    printk("Starting blinky on %s\n", CONFIG_BOARD);

    while (1) {
        ret = gpio_pin_toggle_dt(&led_1);
        if (ret < 0) {
            printk("Error: Could not toggle LED\n");
            return 0;
        }
        printk("Toggled Green LED\n");
        k_msleep(500);
        ret = gpio_pin_toggle_dt(&led_2);
        if (ret < 0) {
            printk("Error: Could not toggle LED\n");
            return 0;
        }
        printk("Toggled Red LED\n");
        k_msleep(500);        
    }
    return 0;
}
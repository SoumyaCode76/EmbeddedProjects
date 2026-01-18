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

#define THREAD_1_STACK_SIZE (512U)
#define THREAD_2_STACK_SIZE (512U)

#define THREAD_1_PRIORITY 1
#define THREAD_2_PRIORITY 2

static void thread_1(void * arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    while (1) {
        if (gpio_pin_toggle_dt(&led_1) < 0) {
            printk("Error: Could not toggle LED\n");
            return;
        }
        printk("Toggled Green LED\n");
        printk("Green %d\n", gpio_pin_get_dt(&led_1));        
        k_msleep(1000);
    }
}

static void thread_2(void * arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    while (1) {
        if (gpio_pin_toggle_dt(&myled) < 0) {
            printk("Error: Could not toggle Off-board LED\n");
            return;
        }

        printk("Toggled Off-board LED\n");
        printk("Off-board LED %d\n", gpio_pin_get_dt(&myled));        
        k_msleep(200);
    }
}

// statically create the threads and auto-start them
K_THREAD_DEFINE(
    thread_1_id, 
    THREAD_1_STACK_SIZE,
    thread_1,
    NULL, NULL, NULL,
    THREAD_1_PRIORITY,
    0,
    0
);

K_THREAD_DEFINE(
    thread_2_id,
    THREAD_2_STACK_SIZE,
    thread_2,
    NULL, NULL, NULL,
    THREAD_2_PRIORITY,
    0,
    0
);

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
   

    printk("Main thread going to sleep\n");
    while (1) {
        k_sleep(K_FOREVER);
/*         if (gpio_pin_toggle_dt(&led_1) < 0) {
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
        k_msleep(1000);                 */
    }
    return 0;
}
/*
 * GPIO UART Tracer - Embedded C Example
 * 
 * Reads a GPIO pin state and transmits it over UART
 * for real-time waveform visualization using the Python UART analyzer.
 * 
 * Compatible with: STM32F429I-DISC1 and other Zephyr RTOS boards
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <stdio.h>

/* GPIO Configuration - Button (Input) */
#define BUTTON_NODE    DT_ALIAS(sw0)
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);

/* GPIO Configuration - LED (Output for visual feedback) */
#define LED_NODE       DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

/* Sampling interval in milliseconds */
#define SAMPLE_INTERVAL_MS  100

/* UART output format options - choose one */
#define OUTPUT_FORMAT_SIMPLE    1  // Simple "0" or "1"
#define OUTPUT_FORMAT_LABELED   2  // "GPIO=1" or "GPIO=0"
#define OUTPUT_FORMAT_VERBOSE   3  // "Button: HIGH" or "Button: LOW"
#define OUTPUT_FORMAT_NUMERIC   4  // Numeric value for analog-like display

/* Select output format */
#define OUTPUT_FORMAT OUTPUT_FORMAT_SIMPLE

void main(void)
{
    int ret;
    int pin_state;
    int prev_state = -1;
    uint32_t sample_count = 0;

    printk("\n");
    printk("========================================\n");
    printk("  GPIO UART Tracer - Started\n");
    printk("========================================\n");
    printk("Board: %s\n", CONFIG_BOARD);
    printk("Sample Rate: %d ms\n", SAMPLE_INTERVAL_MS);
    printk("\n");

    /* Configure Button GPIO as input */
    if (!device_is_ready(button.port)) {
        printk("Error: Button device %s is not ready\n", button.port->name);
        return;
    }

    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret != 0) {
        printk("Error %d: failed to configure button pin\n", ret);
        return;
    }

    /* Configure LED GPIO as output (optional - for visual feedback) */
    if (device_is_ready(led.port)) {
        ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
        if (ret == 0) {
            printk("LED configured for visual feedback\n");
        }
    }

    printk("\nStarting continuous GPIO monitoring...\n");
    printk("Press the button to see state changes\n");
    printk("Output will be captured by Python UART analyzer\n\n");

    /* Main monitoring loop */
    while (1) {
        /* Read GPIO pin state */
        pin_state = gpio_pin_get_dt(&button);

        if (pin_state < 0) {
            printk("Error reading GPIO pin\n");
            k_msleep(SAMPLE_INTERVAL_MS);
            continue;
        }

        /* Update LED to mirror button state (visual feedback) */
        if (device_is_ready(led.port)) {
            gpio_pin_set_dt(&led, pin_state);
        }

        /* Output pin state in selected format */
        #if OUTPUT_FORMAT == OUTPUT_FORMAT_SIMPLE
            /* Simple format: just "0" or "1" */
            printk("%d\n", pin_state);
            printk("%d\n", );

        #elif OUTPUT_FORMAT == OUTPUT_FORMAT_LABELED
            /* Labeled format: "GPIO=1" - good for pattern matching */
            printk("GPIO=%d\n", pin_state);

        #elif OUTPUT_FORMAT == OUTPUT_FORMAT_VERBOSE
            /* Verbose format: human-readable */
            printk("Button: %s\n", pin_state ? "HIGH" : "LOW");

        #elif OUTPUT_FORMAT == OUTPUT_FORMAT_NUMERIC
            /* Numeric format: value that changes with state */
            printk("Value: %d\n", pin_state * 100);
        #endif

        /* Optional: Only print on state change (reduces UART traffic) */
        /*
        if (pin_state != prev_state) {
            printk("GPIO=%d (changed)\n", pin_state);
            prev_state = pin_state;
        }
        */

        sample_count++;

        /* Optional: Periodic statistics */
        if (sample_count % 50 == 0) {
            printk("--- Sample #%u, State=%d ---\n", sample_count, pin_state);
        }

        /* Wait before next sample */
        k_msleep(SAMPLE_INTERVAL_MS);
    }
}

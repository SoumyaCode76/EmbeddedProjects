#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>
#include <stdlib.h>

#define BUTTON_NODE DT_NODELABEL(button2)
#define LEDG_NODE DT_NODELABEL(green_led_3)
#define LEDR_NODE DT_NODELABEL(red_led_4)
#define BUTTON_DEBOUNCE_TIME_MS             (500)  // Easy to adjust

typedef struct
{
    uint32_t timestamp;
    uint8_t state;
}button_state_t;

#define BUTTON_STATE_MSG_SIZE       sizeof(button_state_t)
#define BUTTON_STATE_MAX_MSGS       (10)

struct gpio_callback button_pressed_callback = {0};
extern struct k_msgq button_evt;



void button_pressed_isr(const struct device *udev, struct gpio_callback * cb, uint32_t pins);
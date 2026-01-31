#include "main.h"

K_MSGQ_DEFINE(button_evt, BUTTON_STATE_MSG_SIZE, BUTTON_STATE_MAX_MSGS, sizeof(button_state_t));
const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);
const struct gpio_dt_spec led_green = GPIO_DT_SPEC_GET(LEDG_NODE, gpios);
const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(LEDR_NODE, gpios);

volatile button_state_t btn_evt_detected = {0};
volatile uint8_t led_state = 0;

int main(void)
{
    /* Configure the GPIO pins with direction and initial values */
    gpio_pin_configure(button.port, button.pin, GPIO_INPUT);
    gpio_pin_interrupt_configure(button.port, button.pin, GPIO_INT_EDGE_BOTH);

    /* Configure ISR for button press */
    gpio_init_callback(&button_pressed_callback, button_pressed_isr, BIT(button.pin));
    gpio_add_callback(button.port, &button_pressed_callback);

    /* Configure GPIO pins for LEDs */
    gpio_pin_configure(led_green.port, led_green.pin, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure(led_red.port, led_red.pin, GPIO_OUTPUT_INACTIVE);


    printk("Set up button at %s pin %d\n", button.port->name, button.pin);
    uint32_t pressed_timestamp = 0;
    uint32_t released_timestamp = 0;
    uint32_t first_time = 0;

    while (1)
    {
        if(k_msgq_get(&button_evt, &btn_evt_detected, K_NO_WAIT) == 0)
        {
            if(btn_evt_detected.state == 1)
            {
                pressed_timestamp = btn_evt_detected.timestamp;
            }
            else
            {
                released_timestamp = btn_evt_detected.timestamp;
            }

            if(abs(released_timestamp - pressed_timestamp) > BUTTON_DEBOUNCE_TIME_MS)
            {
                led_state = btn_evt_detected.state;
                gpio_pin_set(led_green.port, led_green.pin, led_state);
                if(abs(released_timestamp - pressed_timestamp) > (5 * BUTTON_DEBOUNCE_TIME_MS))
                {
                    if(first_time == 0)
                    {
                        first_time = 1;
                    }
                    else
                    {
                        printk("Long press detected! Resetting MCU...\n\n");
                        __NVIC_SystemReset();
                    }
                }
                else
                {
                    printk("Short press detected\n");
                }
            } 
        }
    }
    return 0;
}

void button_pressed_isr(const struct device *udev, struct gpio_callback *cb, uint32_t pins)
{
    static button_state_t btn_event = {0};
    btn_event.timestamp = k_uptime_get_32();
    btn_event.state = gpio_pin_get(button.port, button.pin);
    k_msgq_put(&button_evt, &btn_event, K_NO_WAIT);
}
#include "main.h"

K_MSGQ_DEFINE(button_evt, BUTTON_STATE_MSG_SIZE, BUTTON_STATE_MAX_MSGS, sizeof(button_state_t));
const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);
const struct gpio_dt_spec led_green = GPIO_DT_SPEC_GET(LEDG_NODE, gpios);
const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(LEDR_NODE, gpios);

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
    button_state_t btn_evt_detected = {0};
    uint32_t pressed_timestamp = 0;
    uint32_t released_timestamp = 0;

    while (1)
    {
/*         int val = gpio_pin_get(button.port, button.pin);
        gpio_pin_set(led_green.port, led_green.pin, val);
        gpio_pin_set(led_red.port, led_red.pin, !val); */

        if(k_msgq_get(&button_evt, &btn_evt_detected, K_NO_WAIT) == 0)
        {
            if(btn_evt_detected.state == 1)
            {
                printk("Button pressed at: %d\n", btn_evt_detected.timestamp);
                pressed_timestamp = btn_evt_detected.timestamp;
            }
            else
            {
                printk("Button released at: %d\n", btn_evt_detected.timestamp);
                released_timestamp = btn_evt_detected.timestamp;
            }

            if(abs(released_timestamp - pressed_timestamp) > BUTTON_DEBOUNCE_TIME_MS)
            {
                gpio_pin_set(led_green.port, led_green.pin, btn_evt_detected.state);
            }
            else if(abs(released_timestamp - pressed_timestamp) > (2 * BUTTON_DEBOUNCE_TIME_MS))
            {
                __NVIC_SystemReset();
            }
        }
        else
        {
            gpio_pin_toggle(led_red.port, led_red.pin);
            k_msleep(100);
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
/*     static uint32_t last_btn_press_detected = 0;
    static button_state_t btn_pressed = {0};
    uint32_t current_sys_cycle = k_uptime_get_32();
    if (current_sys_cycle < last_btn_press_detected)
    {
        last_btn_press_detected = 0;
    }
    if ((current_sys_cycle - last_btn_press_detected) > BUTTON_DEBOUNCE_TIME_MS)
    {
        last_btn_press_detected = current_sys_cycle;
	    
    } */
}
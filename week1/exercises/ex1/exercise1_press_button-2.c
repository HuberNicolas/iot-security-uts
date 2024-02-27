#include "contiki.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include <stdio.h>

char led_on[] = "LED ON";
char led_off[] = "LED OFF";

// Variable to track LED state
static uint8_t led_state = 0;

/*-------------------------------------------------*/
PROCESS(button_process, "button process test 1");
AUTOSTART_PROCESSES(&button_process);
/*-------------------------------------------------*/
PROCESS_THREAD(button_process, ev, data)
{
    PROCESS_BEGIN();
    SENSORS_ACTIVATE(button_sensor);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
        printf("button is pressed\n");

        // Toggle LED state based on previous state
        if(led_state == 0) {
            leds_on(LEDS_RED);
            printf("%s\n", led_on);
            led_state = 1; // Update state to on
        } else {
            leds_off(LEDS_RED);
            printf("%s\n", led_off);
            led_state = 0; // Update state to off
        }
    }

    PROCESS_END();
}


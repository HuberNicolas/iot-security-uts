#include "contiki.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include <stdio.h>

#define BLINK_INTERVAL (CLOCK_SECOND) // Blink interval: 1 second
#define BLINK_DURATION 5 // Total duration for blinking: 5 seconds

PROCESS(button_blink_process, "Button controlled blink process");
AUTOSTART_PROCESSES(&button_blink_process);

PROCESS_THREAD(button_blink_process, ev, data)
{
    PROCESS_BEGIN();

    static struct etimer et;
    int blink_count = 0; // Counter for the number of blinks

    SENSORS_ACTIVATE(button_sensor);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
        printf("Button pressed, start blinking\n");

        for(blink_count = 0; blink_count < BLINK_DURATION * 2; blink_count++) {
            leds_toggle(LEDS_RED); // Toggle LED state
            printf("Toggle LED\n");
            etimer_set(&et, BLINK_INTERVAL / 2);
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        }

        leds_off(LEDS_RED); // Ensure the LED is turned off after the blinking process
        printf("Blinking finished, LED off\n");
    }

    PROCESS_END();
}


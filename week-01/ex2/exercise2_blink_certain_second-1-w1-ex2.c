#include "contiki.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include <stdio.h>

#define SECONDS 1 // Blink interval: 1 second
#define RANGE 10 // Total duration for blinking: 5 seconds

PROCESS(button_blink_process, "Button controlled blink process");
AUTOSTART_PROCESSES(&button_blink_process);

PROCESS_THREAD(button_blink_process, ev, data)
{
    PROCESS_BEGIN();

    printf("Start blink process\n");

    static struct etimer et;
    static int count = 0;

    SENSORS_ACTIVATE(button_sensor);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
        count = 0; // Reset count on each button press

        while(count < RANGE) {
            etimer_set(&et, CLOCK_SECOND * SECONDS);
            leds_toggle(LEDS_RED); // Toggle LED state between on and off
            printf("Toggle LED\n");
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
            count += SECONDS; // Increment count based on the number of SECONDS per interval

            // No need to check if count >= RANGE here, the loop condition handles it
        }

        leds_off(LEDS_RED); // Ensure the LED is turned off after the blinking process
        printf("Blinking finished, LED off.\n");
    }

    PROCESS_END();
}


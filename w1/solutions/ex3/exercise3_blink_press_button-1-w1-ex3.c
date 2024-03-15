#include "contiki.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include <stdio.h>

#define SECONDS 2 // Interval for blinking

// Define a process
PROCESS(button_process, "Button controlled application process");
AUTOSTART_PROCESSES(&button_process);

// Process thread
PROCESS_THREAD(button_process, ev, data)
{
    static struct etimer et; // Timer for controlling blink interval
    static int application_running = 0; // State of the application (0 = stopped, 1 = running)

    PROCESS_BEGIN();

    SENSORS_ACTIVATE(button_sensor); // Activate the button sensor

    while(1) {
        PROCESS_WAIT_EVENT(); // Wait for an event

        if (ev == sensors_event && data == &button_sensor) {
            // Toggle the application state
            application_running = !application_running;

            if (application_running) {
                printf("Application started\n");
                etimer_set(&et, CLOCK_SECOND * SECONDS); // Start the timer for blinking
            } else {
                printf("Application stopped\n");
                leds_off(LEDS_ALL); // Ensure LEDs are turned off when stopping the application
                continue; // Skip the rest of the loop to wait for another button press
            }
        }

        if (application_running) {
            // Handle the blinking logic only if the application is running
            if (ev == PROCESS_EVENT_TIMER && etimer_expired(&et)) {
                printf("LEDS TOGGLE\n");
                leds_toggle(LEDS_ALL); // Toggle LEDs
                etimer_reset(&et); // Reset the timer to continue blinking
            }
        }
    }

    PROCESS_END();
}


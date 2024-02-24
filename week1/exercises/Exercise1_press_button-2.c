#include "contiki.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include <stdio.h>
char led_on[] = "LED ON";
char led_off[] = "LED OFF";
/*-------------------------------------------------*/
PROCESS(button_process, "button process test 1");
AUTOSTART_PROCESSES(&button_process);
/*-------------------------------------------------*/
PROCESS_THREAD(button_process, ev, data)
{
PROCESS_BEGIN();
SENSORS_ACTIVATE(button_sensor);
while(1){
PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
printf("button is pressed\n");
leds_on(LEDS_RED);
printf("%s\n", led_on);
PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
printf("button is pressed again\n");
leds_off(LEDS_RED);
printf("%s\n", led_off);
}
PROCESS_END();
}

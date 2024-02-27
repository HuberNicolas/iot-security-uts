#include "contiki.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include <stdio.h>
#define SECONDS 2
char hello[] = "hello from the mote!";
/*-------------------------------------------------*/
PROCESS(button_process, "button process test 1");
AUTOSTART_PROCESSES(&button_process);
/*-------------------------------------------------*/
PROCESS_THREAD(button_process, ev, data)
{
PROCESS_BEGIN();
static struct etimer et;
SENSORS_ACTIVATE(button_sensor);
while(1){
	PROCESS_WAIT_EVENT();
	if (ev == sensors_event && data == &button_sensor){
		printf("Button is pressed\n");
		/*Start to blink the LEDs when the button is pressed*/
		while(1){
			etimer_set(&et, CLOCK_SECOND*SECONDS);
			PROCESS_WAIT_EVENT();
			if (ev == PROCESS_EVENT_TIMER && etimer_expired(&et)){
			printf("LEDS TOGGLE\n");
			leds_toggle(LEDS_ALL); /*If LEDS are on -> off and vice versa*/
			}
			else if (ev == sensors_event && data == &button_sensor){
				printf("Button is pressed again\n");
				break;
			}
		}
	}
}
PROCESS_END();
}

#include "contiki.h"
#include "dev/leds.h"
#include <stdio.h>
#define SECONDS 2
#define RANGE 20 /*Blink LEDs for 20 seconds only*/
/*-------------------------------------------------*/
PROCESS(blink_process, "Blink LEDs for a certain number of seconds");
AUTOSTART_PROCESSES(&blink_process);
/*-------------------------------------------------*/
PROCESS_THREAD(blink_process, ev, data)
{
PROCESS_BEGIN();
static struct etimer et;
static int count;
count = 0;
while(1) {
etimer_set(&et, CLOCK_SECOND*SECONDS);
PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
count = count + SECONDS;
printf("LEDS ON\n");
leds_on(LEDS_ALL);
if (count >= RANGE)
{
	break;
}
etimer_set(&et, CLOCK_SECOND*SECONDS);
PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
count = count + SECONDS;
printf("LEDS OFF\n");
leds_off(LEDS_ALL);
if (count >= RANGE)
{
	break;
}
etimer_reset(&et);
}
PROCESS_END();
}

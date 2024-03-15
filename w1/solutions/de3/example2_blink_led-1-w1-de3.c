#include "contiki.h"
#include "dev/leds.h"
#include <stdio.h>
#define SECONDS 1
/*-------------------------------------------------*/
PROCESS(hello_timer_process, "hello world with timer example");
AUTOSTART_PROCESSES(&hello_timer_process);
/*-------------------------------------------------*/
PROCESS_THREAD(hello_timer_process, ev, data)
{
PROCESS_BEGIN();
static struct etimer et;
while(1) {
etimer_set(&et, CLOCK_SECOND * SECONDS);
PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
printf("LEDS ON\n");
leds_on(LEDS_ALL);
etimer_set(&et, CLOCK_SECOND * SECONDS);
PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
printf("LEDS OFF\n");
leds_off(LEDS_ALL);
etimer_reset(&et);
}
PROCESS_END();
}

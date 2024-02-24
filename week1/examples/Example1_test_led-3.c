#include "contiki.h"
#include "dev/leds.h"
#include <stdio.h>
char hello[] = "hello from the mote!";
/*-------------------------------------------------*/
PROCESS(led_process, "led process test 1");
AUTOSTART_PROCESSES(&led_process);
/*-------------------------------------------------*/
PROCESS_THREAD(led_process, ev, data)
{
PROCESS_BEGIN();
leds_on(LEDS_RED);
leds_on(LEDS_GREEN);
printf("%s\n", hello);
PROCESS_END();
}

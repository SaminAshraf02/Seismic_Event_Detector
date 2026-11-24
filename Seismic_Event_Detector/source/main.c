#include <stdio.h>
#include <stdint.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

/* assembly driver (leds.s) */
extern void led_setup(void);
extern void led_off(void);
extern void led_idle(void);
extern void led_calibrating(void);
extern void led_event(void);
extern void led_error(void);

static void delay(void)
{
    for (volatile uint32_t i = 0; i < 3000000; i++) { }   /* rough delay, tune by eye */
}

int main(void)
{
	/* Init board hardware. */
	    BOARD_InitBootPins();
	    BOARD_InitBootClocks();
	    BOARD_InitBootPeripherals();
	#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	    /* Init FSL debug console. */
	    BOARD_InitDebugConsole();
	#endif

    led_setup();

    while (1) {
        led_idle();         delay();   /* green   */
        led_calibrating();  delay();   /* blue    */
        led_event();        delay();   /* red     */
        led_error();        delay();   /* magenta */
        led_off();          delay();   /* all off */
    }
}

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "adxl345.h"

/* assembly driver (leds.s) */
extern void led_setup(void);
extern void led_off(void);
extern void led_idle(void);
extern void led_calibrating(void);
extern void led_event(void);
extern void led_error(void);

/* Signed decimal, e.g. -2500 -> "-2500" */
static void print_s32(int32_t v)
{
    char buf[12];
    int i = 11;
    uint32_t m = (v < 0) ? (0u - (uint32_t)v) : (uint32_t)v;

    buf[i] = '\0';
    do {
        buf[--i] = (char)('0' + (m % 10));
        m /= 10;
    } while (m != 0);
    if (v < 0) {
        buf[--i] = '-';
    }
    PRINTF("%s", &buf[i]);
}

/* Milli-g as decimal, e.g. -1038 -> "-1.038" */
static void print_mg(int32_t mg)
{
    uint32_t m = (mg < 0) ? (0u - (uint32_t)mg) : (uint32_t)mg;
    char frac[4];

    frac[0] = (char)('0' + (m / 100) % 10);
    frac[1] = (char)('0' + (m / 10) % 10);
    frac[2] = (char)('0' + m % 10);
    frac[3] = '\0';
    PRINTF("%s%u.%s", (mg < 0) ? "-" : "", m / 1000, frac);
}

int main(void)
{
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    BOARD_InitDebugConsole();
#endif

    led_setup();
    led_calibrating();

    PRINTF("\r\n=== Seismic detector: ADXL345 test ===\r\n");

    if (!adxl_init()) {
        PRINTF("ERROR: ADXL345 not found (check wiring, CS, address)\r\n");
        led_error();
        while (1) { }
    }
    PRINTF("ADXL345 found (ID 0xE5)\r\n");

    PRINTF("Calibrating, keep the board still...\r\n");
    if (!adxl_calibrate(64)) {
        PRINTF("ERROR: calibration read failed\r\n");
        led_error();
        while (1) { }
    }

    adxl_raw_t rest;
    adxl_get_rest(&rest);
    PRINTF("Rest offset (counts): x=");
    print_s32(rest.x);
    PRINTF(" y=");
    print_s32(rest.y);
    PRINTF(" z=");
    print_s32(rest.z);
    PRINTF("\r\n");

    led_idle();

    uint32_t n = 0;
    uint32_t peak = 0;                      /* biggest deviation since last print, counts */

    while (1) {
        adxl_raw_t raw, c;

        if (!adxl_read_raw(&raw)) {
            PRINTF("ERROR: I2C read failed\r\n");
            led_error();
            while (1) { }
        }
        adxl_apply_offset(&raw, &c);

        uint32_t dev = (uint32_t)(abs(c.x) + abs(c.y) + abs(c.z));
        if (dev > peak) peak = dev;

        if (++n % 10 == 0) {                /* print at 10 Hz */
            PRINTF("raw: ");
            print_s32(raw.x); PRINTF(" ");
            print_s32(raw.y); PRINTF(" ");
            print_s32(raw.z);
            PRINTF("  |  g: ");
            print_mg(adxl_to_mg(c.x)); PRINTF(" ");
            print_mg(adxl_to_mg(c.y)); PRINTF(" ");
            print_mg(adxl_to_mg(c.z));
            PRINTF("  |  peak: ");
            print_mg(adxl_to_mg((int32_t)peak));
            PRINTF("\r\n");
            peak = 0;
        }
    }
}

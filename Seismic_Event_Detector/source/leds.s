@ leds.s - onboard RGB LED driver for FRDM-K64F (Milestone 1)
@ Red = PTB22, Green = PTE26, Blue = PTB21 (all active low)

.syntax unified             @ unified syntax used
.cpu cortex-m4              @ cpu is cortex-m4
.thumb                      @ use thumb encoding

.text                       @ put code in the code section

.global led_setup           @ declare as a global variable
.type led_setup, %function  @ set to function type

led_setup:
    push    {lr}            @ save the link register, we call led_off below

    ldr r1, =0x40048038     @ System Clock Gate Control Register 5 (SIM_SCGC5)
    ldr r0, [r1]            @ read current register value
    orr r0, r0, #(1<<10)    @ enable clock for port B (bit 10)
    orr r0, r0, #(1<<13)    @ enable clock for port E (bit 13)
    str r0, [r1]            @ apply the new settings

    ldr r1, =0x4004A054     @ PTB21 Pin Control Register (PORTB_PCR21)
    mov r0, #0x00000100     @ set pin to GPIO mode
    str r0, [r1]            @ apply the new settings
    ldr r1, =0x4004A058     @ PTB22 Pin Control Register (PORTB_PCR22)
    str r0, [r1]            @ set pin to GPIO mode
    ldr r1, =0x4004D068     @ PTE26 Pin Control Register (PORTE_PCR26)
    str r0, [r1]            @ set pin to GPIO mode

    ldr r1, =0x400FF054     @ GPIOB Port Data Direction Register (GPIOB_PDDR)
    ldr r0, [r1]            @ read current direction bits
    orr r0, r0, #(1<<21)    @ set pin 21 to output mode
    orr r0, r0, #(1<<22)    @ set pin 22 to output mode
    str r0, [r1]            @ apply the new settings

    ldr r1, =0x400FF114     @ GPIOE Port Data Direction Register (GPIOE_PDDR)
    ldr r0, [r1]            @ read current direction bits
    orr r0, r0, #(1<<26)    @ set pin 26 to output mode
    str r0, [r1]            @ apply the new settings

    bl      led_off         @ outputs reset to LOW (LED on), so turn all off
    pop     {pc}            @ restore and return from subroutine

.global led_off             @ declare as a global variable
.type led_off, %function    @ set to function type

led_off:
    ldr r1, =0x400FF044     @ GPIOB Port Set Output Register (GPIOB_PSOR)
    mov r0, #(1<<21)        @ pin 21 (blue)
    orr r0, r0, #(1<<22)    @ pin 22 (red)
    str r0, [r1]            @ set output to HIGH, LEDs off
    ldr r1, =0x400FF104     @ GPIOE Port Set Output Register (GPIOE_PSOR)
    mov r0, #(1<<26)        @ pin 26 (green)
    str r0, [r1]            @ set output to HIGH, LED off
    bx  lr                  @ return from subroutine

.global led_idle            @ status: idle = green
.type led_idle, %function   @ set to function type

led_idle:
    push    {lr}            @ save the link register
    bl      led_off         @ clear the previous color
    ldr r1, =0x400FF108     @ GPIOE Port Clear Output Register (GPIOE_PCOR)
    mov r0, #(1<<26)        @ pin 26 (green)
    str r0, [r1]            @ set output to LOW, green on
    pop     {pc}            @ restore and return from subroutine

.global led_calibrating     @ status: calibrating = blue
.type led_calibrating, %function @ set to function type

led_calibrating:
    push    {lr}            @ save the link register
    bl      led_off         @ clear the previous color
    ldr r1, =0x400FF048     @ GPIOB Port Clear Output Register (GPIOB_PCOR)
    mov r0, #(1<<21)        @ pin 21 (blue)
    str r0, [r1]            @ set output to LOW, blue on
    pop     {pc}            @ restore and return from subroutine

.global led_event           @ status: event detected = red
.type led_event, %function  @ set to function type

led_event:
    push    {lr}            @ save the link register
    bl      led_off         @ clear the previous color
    ldr r1, =0x400FF048     @ GPIOB Port Clear Output Register (GPIOB_PCOR)
    mov r0, #(1<<22)        @ pin 22 (red)
    str r0, [r1]            @ set output to LOW, red on
    pop     {pc}            @ restore and return from subroutine

.global led_error           @ status: error = magenta (red + blue)
.type led_error, %function  @ set to function type

led_error:
    push    {lr}            @ save the link register
    bl      led_off         @ clear the previous color
    ldr r1, =0x400FF048     @ GPIOB Port Clear Output Register (GPIOB_PCOR)
    mov r0, #(1<<21)        @ pin 21 (blue)
    orr r0, r0, #(1<<22)    @ pin 22 (red)
    str r0, [r1]            @ set outputs to LOW, red and blue on
    pop     {pc}            @ restore and return from subroutine

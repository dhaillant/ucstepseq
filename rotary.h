/*
 * Rotary encoder library for AVR.
 */

#ifndef rotary_h
#define rotary_h

#include <avr/io.h>


// Enable this to emit codes twice per step.
//#define HALF_STEP

// Enable weak pullups
//#define ENABLE_PULLUPS

// Values returned by 'process'
// No complete step yet.
#define DIR_NONE 0x0
// Clockwise step.
#define DIR_CW 0x10
// Anti-clockwise step.
#define DIR_CCW 0x20



void rotary_setup(void);
unsigned char rotary_process(void);


#endif

// RPH1 and RPH2 pins (see schematics)
#define ROTARY_PIN1 PD2
#define ROTARY_PIN2 PD3

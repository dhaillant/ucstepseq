/*
	AVR ATMega 328 Timer routines

 * based on :
 * http://maxembedded.com/2011/06/avr-timers-timer1/


	
	Copyright (C) 2017  David Haillant

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef timer_h
#define timer_h

#include <avr/io.h>
#include <avr/interrupt.h>
  
// global variable to count the number of overflows
volatile uint8_t timer_ovf_counter;
volatile uint8_t in_edition_mode_flag;

	// we are using 16 bit timer 1
	// MAX counts = 65535
	// CPU clock is 16 MHz, Clock Period is 1/16 MHz = 63 ns
	// One complete timer 1 count is 65535 * 63 ns = 4.096 ms
	// Using prescaler of 8, we get 4.096 ms * 8 = 32.768 ms
	// In order to get 2 s delay, we need 2 s / 32.768 ms ~= 61 loops

// 61 overflows = 2 seconds delay (approx.)
#define TIMER_MAX_OVF_NUMBER 61

// initialize timer, interrupt and variable
void init_timer(void);
void stop_timer(void);

#endif

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

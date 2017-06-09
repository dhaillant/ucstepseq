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

#include "timer.h"


// initialize timer, interrupt and variable
void init_timer(void)
{
    // set up timer with prescaler = 8
    TCCR1B |= (1 << CS11);
  
    // initialize counter
    TCNT1 = 0;
  
    // enable overflow interrupt
    TIMSK1 |= (1 << TOIE1);
  
    // enable global interrupts
    //sei();
  
    // initialize overflow counter variable
    timer_ovf_counter = 0;
	in_edition_mode_flag = TRUE;

	// TEST
	//SET_GATE_high;
	//PORTD &= ~(1 << PD5);
}

void stop_timer(void)
{
	// clear prescale bits
	TCCR1B &= 0b11111000;

	// TEST
	//SET_GATE_low;
	//PORTD |= (1 << PD5);
}
  
// TIMER1 overflow interrupt service routine
// called whenever TCNT1 overflows
ISR(TIMER1_OVF_vect)
{
    // keep a track of number of overflows
    timer_ovf_counter++;
  
    // check for number of overflows here itself
    if (timer_ovf_counter >= TIMER_MAX_OVF_NUMBER) // NOTE: '>=' used instead of '=='
    {
        // DO BUSINESS HERE  V  V  V  V  V  V  V

		// we're no more in edition mode
		in_edition_mode_flag = FALSE;
		
		// we stop the timer
		stop_timer();

        // no timer reset required here as the timer
        // is reset every time it overflows
  
        timer_ovf_counter = 0;   // reset overflow counter
    }
}


/*
 * spi_display.c
 * 
 * Copyright 2016 David Haillant <david_haillant@yahoo.fr>
 * http://www.davidhaillant.com
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
#include <stdio.h>
#include "spi_display.h"
#include "HRL_SPI.h"



// Character Generation

/*
 * Digits wiring :
 * 
 *     A
 *   F   B
 *     G
 *   E   C
 *     D
 *         dp
 * 
 *     2
 *   3   1
 *     0
 *   4   6
 *     5
 *         7
 * 
*/

/*
 * Digits array. Each segment is driven by a bit (0 or 1)
 * 0 means segment is ON
 * 1 means segment is OFF
 * 
 * led_digits[0] is character "0"
 * led_digits[9] is character "9"
 * led_digits[10] is character " " ("empty" or "space")
 */

const uint8_t led_digits[] = {
  //dCDEFABG
  //76543210
  0b10000001,  // 0
  0b10111101,  // 1
  0b11001000,  // 2
  0b10011000,  // 3
  0b10110100,  // 4
  0b10010010,  // 5
  //76543210
  0b10000010,  // 6
  0b10111001,  // 7
  0b10000000,  // 8
  0b10010000,  // 9
  0b11111111   // all off
  //dCDEFABG
};






void setup_spi_display(void)
{
  	// set Pins Directions for OUTPUT
	// (1 means FORCE OUTPUT, 0 is unchanged state)
//	SPI_DISP_DDR |= (DATA | LATCH | CLOCK);			//Set control pins as outputs
//	SPI_DISP_PORT &= ~(DATA | LATCH | CLOCK);		//Set control pins low

	SPI_DISPLAY_DDR |= (1 << SPI_DISPLAY_PIN);			//Set SPI_DISPLAY_PIN as output

	//Setup SPI
//	SPCR = (1<<SPE) | (1<<MSTR);  //Start SPI as Master
	DESELECT_SPI_DISPLAY;
}

/*
int shift_data(uint8_t data)
{
	SPDR = data;
	//Wait for SPI process to finish
	while(!(SPSR & (1<<SPIF)));

	return 0;
}
*/

/*
 * display_number : use SPI to display numbers on 3 digits 7 segments
 * display
 * ---------------------------------------------------------------------
 * uint16_t numberToDisplay : BCD value (value 0x123 will display "123")
*/
void display_number(uint16_t numberToDisplay)
{
	printf("numberToDisplay = %d\n", numberToDisplay);

	//uint8_t digit = 0;				// 1 digit
	//uint8_t digits[] = {0, 0, 0};		// 3 digits
	uint8_t digits[] = {0, 0};		// 2 digits

	// extract digits
/*	digits[0] = (numberToDisplay >> 8) & 0b00001111;  // MSB
	digits[1] = (numberToDisplay >> 4) & 0b00001111;
	digits[2] = (numberToDisplay >> 0) & 0b00001111;  // LSB*/

	digits[0] = (numberToDisplay >> 4) & 0b00001111;  // MSB
	digits[1] = (numberToDisplay >> 0) & 0b00001111;  // LSB

	if (digits[0] == 0)		// if MSB is "0" (eg. "077")
	{
		digits[0] = 10;		// then display nothing instead of "0" (eg. "77")
	}

/*	if (digits[1] == 0)		// if MSB+1 is "0" (eg. "007")
	{
		digits[1] = 10;		// then display nothing instead of "0" (eg. "7")
	}*/

	//init_transfert;
	SELECT_SPI_DISPLAY;

	// shift out the bits from digits:
	for (uint8_t digit = 0; digit < MAX_DIGIT; digit++)
	{
		//shift_data(led_digits[digits[digit]]);
		sendSPI(led_digits[digits[digit]]);
	}

	//end_transfert;
	DESELECT_SPI_DISPLAY;
}

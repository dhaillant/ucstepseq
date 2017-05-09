/*
 * spi_leds.c
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
 */


#include "spi_leds.h"







void setup_spi_leds(void)
{
	SPI_LEDS_DDR |= (1 << SPI_LEDS_PIN);	//Set SPI_DISPLAY_PIN as output
	DESELECT_SPI_LEDS;
}

void update_spi_leds(uint16_t value)
{
	// 2 bytes value,
	// 0x0000 -> all OFF
	SELECT_SPI_LEDS;

	sendSPI(~(value >> 8));					// MSB
	sendSPI(~(value & 0b11111111));			// LSB

	//sendSPI(~value);

	DESELECT_SPI_LEDS;
}

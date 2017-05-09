/*
 * spi_display.h
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




#ifndef spi_display_h
#define spi_display_h

#include <avr/io.h>

#define SPI_DISPLAY_DDR DDRB
#define SPI_DISPLAY_PORT PORTB
#define SPI_DISPLAY_PIN PB2

//#define DISP_REG2 DDRC
//#define DISP_PORT2 PORTC


//#define DATA (1<<PB3)	//MOSI (SI)
//#define LATCH (1<<PB2)	//SS   (RCK)
//#define LATCH (1<<PB1)	//SS   (RCK)
//#define CLOCK (1<<PB5)	//SCK  (SCK)

//#define DIMM (1<<PC3)	// A3 dimmer





// PORT B :

//Pin connected to ST_CP of 74HC595
//int latchPin = 10;
//#define latchBit 2 
//Pin connected to SH_CP of 74HC595
//int clockPin = 13;
//#define clockBit 5
//Pin connected to DS of 74HC595
//int dataPin = 11;
//#define dataBit 3

// PORT C :

//Pin connected to 
//int enPin = "A3";


// take the latchPin low so 
// the LEDs don't change while you're sending in bits:
//#define init_transfert SPI_DISPLAY_PORT &= ~LATCH

//take the latch pin high so the LEDs will light up:
//#define end_transfert SPI_DISPLAY_PORT |= LATCH


#define SELECT_SPI_DISPLAY   SPI_DISPLAY_PORT &= ~(1 << SPI_DISPLAY_PIN)
#define DESELECT_SPI_DISPLAY SPI_DISPLAY_PORT |= (1 << SPI_DISPLAY_PIN)

#define MAX_DIGIT 2

void display_number(uint16_t numberToDisplay);
void setup_spi_display(void);
//int shift_data(uint8_t data);

#endif

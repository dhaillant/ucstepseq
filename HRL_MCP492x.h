#ifndef _HRL_MCP492x_h_
#define _HRL_MCP492x_h_

#include <avr/io.h>
#include "HRL_SPI.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DAC_DDR
#define DAC_DDR DDRB
#endif

#ifndef DAC_PORT
#define DAC_PORT PORTB
#endif

#ifndef DAC_PIN
#define DAC_PIN PB1
#endif


#define SETUP_DAC    DAC_DDR |= (1 << DAC_PIN)   // Set up #CS for the DAC
#define SELECT_DAC   DAC_PORT &= ~(1 << DAC_PIN) // Set #CS for DAC low, selecting it
#define DESELECT_DAC DAC_PORT |= (1 << DAC_PIN) // Set #CS for DAC high, deselecting it

void writeMCP492x(uint16_t data, uint8_t config);

#ifdef __cplusplus
}
#endif
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//#include <math.h>

#include <stdio.h>



#define TRUE  1
#define FALSE 0




// ************************ serial interface ***************************
#define USE_2X
#include "uart.h"
// uncomment to use UART :
//#define USE_UART


// ********************* input / output pins ***************************
#define STEP_PIN  PD6
#define GATE_PIN  PD5
#define RESET_PIN PD7
#define RPUSH_PIN PD4

// input / output Direction Register DDR
#define INOUT_DDR DDRD



// ************************* rotary encoder ****************************
#include "rotary.h"
#define MIN_ROTARY_INC 0
#define MAX_ROTARY_INC 60

int rot_increments = 0;





// ***************************** SPI ***********************************
// SPI driver
#include "HRL_SPI.h"




// 2 seven segment displays
#include "spi_display.h"

// 2 LEDs strips
#include "spi_leds.h"




// **************************** DAC ************************************
// 12 bits SPI DAC MCP4921
#include "HRL_MCP492x.h"
#define MAX_DAC_DATA 0x0FFF
#define MIN_DAC_DATA 0

/*
 * 
 * bit 15 A/B: DACA or DACB Select bit
 *   1 = Write to DACB
 *   0 = Write to DACA
 * bit 14 BUF: VREF Input Buffer Control bit
 *   1 = Buffered
 *   0 = Unbuffered
 * bit 13 GA: Output Gain Select bit
 *   1 = 1x (VOUT = VREF * D/4096)
 *   0 = 2x (VOUT = 2 * VREF * D/4096)
 * bit 12 SHDN: Output Power Down Control bit
 *   1 = Output Power Down Control bit
 *   0 = Output buffer disabled, Output is high impedance
 * bit 11-0 D11:D0: DAC Data bits
 *   12 bit number “D” which sets the output value. Contains a value between 0 and 4095.
 * 
*/


//int dac_config = 0x30;
// 0011 0000
// DACA UNBUF 1x SHDN

int dac_config = 0x50;
// 0101 0000
// DACA BUF 2x SHDN

//int dac_data = 0;


/*
 * 	12 semitones per octave
 * 	1 volt per octave
 * 	1 semitone = 1 / 12 = 0.0833333333 V
 * 
 * 	DAC full scale : 4096
 * 	Voltage reference = 2 x 2.5 V
 * 	DAC resolution = (2 x 2.5) / 4096 = 0.001220703125 V
 * 	semitone resolution = 0.0833333 / 0.001220703125 = 68.2666667 steps
 * 
 * 	5 V -> 5 octaves -> 60 half tones
 * 	real full scale : 60 x 68 = 4080
 * 	error on full scale : 16 x 0.001220703125 = 0.01953125 V
*/

// DAC value for a semitone
#define DAC_SEMITONE 68

// 5 octaves -> 60 semitones (0..59)
#define MIN_SEMITONE 0
#define MAX_SEMITONE 59





// **************************** SEQUENCER ******************************

// max number of steps per pattern (0..7) = 8 steps
#define MAX_STEPS 8
#define LAST_STEP (MAX_STEPS - 1)
// max number of patterns (0..3) = 4 patterns
//#define MAX_PATTERN 3

// notes (0 = lowest note, 1 = 1st higher semitone)
//uint8_t half_tones[MAX_PATTERN][MAX_STEP];
uint8_t step_semitones[MAX_STEPS];

// gated steps, binary value : 0 = no gate, 1 = gate
uint8_t step_gates[MAX_STEPS];

// current active step
uint8_t current_step = 0;

// current active pattern
uint8_t current_pattern = 0;








// is GATE output trigger or gate style?
uint8_t trig_out = FALSE;




unsigned char char2bcd(unsigned char hex)
{
   unsigned char MSD = 0;

   while (hex >= 10)
   {
      hex -= 10;	// hex becomes 1s (LSD)
      MSD += 0x10;	// add 1 to 10s (MSD)
   }
   return MSD + hex;	// pack BCD into char
}


void update_gate_output(uint8_t gate);
void update_cv_output(uint8_t semitone);

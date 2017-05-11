/*
	µC Step Sequencer 1.1
	Step sequencer for Analog Synthesizers, driven by a microcontroller.
	
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


#include "main.h"






uint8_t forward_next_step(void);
void output_gate(uint8_t gate);
//void output_half_tone(uint8_t half_tone);

// 2 bytes for steps and gates leds
// Row 1 : Step		SSSS SSSS xxxx xxxx
// Row 2 : Gates	xxxx xxxx GGGG GGGG
// set the step_n_gate_leds bits to 1 for led on, 0 for led off, then call update_step_n_gate_leds
uint16_t step_n_gate_leds = 0;









/*
void update_pattern_leds(uint8_t patterns)
{
	uint16_t leds = patterns + current_step;
	//update_spi_leds(
}
* */

void update_step_n_gate_leds(void)
{
	update_spi_leds(step_n_gate_leds);
}



// http://www.leniwiec.org/en/2014/04/28/rotary-shaft-encoder-how-to-connect-it-and-handle-it-with-avr-atmega8-16-32-168-328/
// schéma : http://www.leniwiec.org/wp-content/uploads/2014/04/schemat.png

/*
void increase_dac_output(void)
{
	dac_data += 1;
	if (dac_data > (MAX_DAC_DATA))
		dac_data = MAX_DAC_DATA;

	writeMCP492x(dac_data, dac_config);
	//_delay_ms(5000);
	//PORTD ^= _BV(PD4);
	display_number(0xaa8);

	printf("up   dac_data = %d\n", dac_data);
}

void decrease_dac_output(void)
{
	dac_data -= 1;
	if (dac_data < (MIN_DAC_DATA))
		dac_data = MIN_DAC_DATA;

	writeMCP492x(dac_data, dac_config);
	//_delay_ms(5000);
	display_number(0x8aa);

	printf("down dac_data = %d\n", dac_data);
}
*/


//void update_dac_output(int setpoint)
void update_dac_output(uint16_t dac_value)
{
	//uint16_t dac_data;
	//dac_data = setpoint * DAC_SEMITONE;
	//writeMCP492x(dac_data, dac_config);
	writeMCP492x(dac_value, dac_config);

	#ifdef USE_UART
		printf("dac_data = %d\n", dac_data);
	#endif
}




// ************************ Rotary Encoder *****************************
void rotation_CW(void);
void rotation_CCW(void);


//INT0 interrupt 
ISR(INT0_vect )
{ 
	unsigned char result = rotary_process();

	if (result == DIR_CW) 	rotation_CW();
	if (result == DIR_CCW) 	rotation_CCW();
}

//INT1 interrupt 
ISR(INT1_vect )
{ 
	unsigned char result = rotary_process();

	if (result == DIR_CW) 	rotation_CW();
	if (result == DIR_CCW) 	rotation_CCW();
}


// for test
/*void increase_rot_increments(void)
{
	rot_increments += 1;
	if (rot_increments > (MAX_ROTARY_INC))
		rot_increments = MAX_ROTARY_INC;

	#ifdef USE_UART
	printf("up   rot_increments = %d\n", rot_increments);
	#endif

	update_dac_output(rot_increments);
}

void decrease_rot_increments(void)
{
	rot_increments -= 1;
	if (rot_increments < (MIN_ROTARY_INC))
		rot_increments = MIN_ROTARY_INC;

	#ifdef USE_UART
	printf("down rot_increments = %d\n", rot_increments);
	#endif

	update_dac_output(rot_increments);
}*/


void rotation_CW(void)
{
	/*
	 * flag down "step's value changed"
	 * 
	 * if rpush NOT pressed then
	 *     point next step
	 * else if rpush IS pressed then
	 *     increase step's value
	 *     flag up "step's value changed"
	 * end if
	 * 
	 * reset edition timer
	 */
	//update_dac_output(forward_next_step());
	//increase_rot_increments();
}

void rotation_CCW(void)
{
	/*
	 * flag down "step's value changed"
	 * 
	 * if rpush NOT pressed then
	 *     point previous step
	 * else if rpush IS pressed then
	 *     decrease step's value
	 *     flag up "step's value changed"
	 * end if
	 * 
	 * reset edition timer
	 */

	//decrease_rot_increments();
}







#define STEP_PIN_low   bit_is_clear(PIND, STEP_PIN)
#define STEP_PIN_high  bit_is_set(PIND, STEP_PIN)
// pour test, simulation d'une impulsion STEP sur appui du bouton MODE/RESET :
//#define STEP_PIN_low   bit_is_clear(PIND, RESET_PIN)
//#define STEP_PIN_high  bit_is_set(PIND, RESET_PIN)
#define RESET_PIN_low  bit_is_clear(PIND, RESET_PIN)
#define RESET_PIN_high bit_is_set(PIND, RESET_PIN)

#define LOW 0
#define HIGH 1
uint8_t STEP_PIN_previous_state = LOW;


#define SET_GATE_high   PORTD &= ~(1 << GATE_PIN)
#define SET_GATE_low    PORTD |= (1 << GATE_PIN)






// ************************** DISPLAY **********************************
#define SWITCH_OFF_STEP_LEDS 	step_n_gate_leds = step_n_gate_leds & 0xFF00

void display_step(uint8_t step)
{
	// steps are BYTE n°2 (LSB)
	
	// step = 0 -> no step displayed (all leds off)
	// step = 1 -> left led on
	// step = 8 -> right led on

	// switch off step leds
	SWITCH_OFF_STEP_LEDS;

	if(( step > 0) && (step <= 8))
	{
		// switch on one led
		step_n_gate_leds = step_n_gate_leds | (1 << (step - 1));

		update_step_n_gate_leds();
	}
}

void display_gates(uint8_t gates[])
{
	// Row 1 : Step		SSSS SSSS xxxx xxxx
	// Row 2 : Gates	xxxx xxxx GGGG GGGG
	
	// steps are BYTE n°... (LSB)
	
	//step_n_gate_leds

	for (uint8_t i = 0; i < MAX_STEPS; i++)
	{
		step_n_gate_leds = step_n_gate_leds | (step_gates[i] << (i + 8));
	}
	//update_spi_leds(leds);

	update_step_n_gate_leds();
}



// outputs

void update_gate_output(uint8_t gate)
{
	if (gate == 0)
	{
		SET_GATE_low;
	}
	else
	{
		SET_GATE_high;
	}
}

void update_cv_output(uint8_t semitone)
{
	update_dac_output(semitone * DAC_SEMITONE);
}



uint8_t go_step(uint8_t step)
{
	// set current step and update outputs then display accordingly


	current_step = step;

	// update outputs
	update_gate_output(step_gates[current_step]);
	update_cv_output(step_semitones[current_step]);

	// update display
	display_step(current_step);
	update_step_n_gate_leds();
	
	//update_step_leds(current_step);
	//update_spi_leds(1 << current_step);

	return current_step;
}


uint8_t forward_next_step(void)
{
	current_step += 1;
	if (current_step > MAX_STEPS)
		current_step = 0;

	display_step(current_step);

	return current_step;
}



void output_gate(uint8_t step)
{
		if (step_gates[step] == HIGH)
		{
			SET_GATE_high;
			if (trig_out == TRUE)
			{
				_delay_ms(1);
				SET_GATE_low;
			}
		}
		else
		{
			SET_GATE_low;
			//SET_GATE_low;
		}
}


void output_cv(uint8_t step)
{
	update_dac_output(step_semitones[step]);
}


ISR(PCINT2_vect)
{
	// Interrupt service routine. Every single PCINT23..16 (PD7..PD0) change
	// cf. PCMSK2 setup for the actual checked bit changes

	/*
	 * we're watching :
	 * STEP_PIN
	 * RPUSH_PIN
	 * RESET_PIN
	 * 
	 */

/*
	if (RESET_PIN_low) {
		//go_step(0);
		//update_dac_output(0);
		
		_delay_ms(5);
	} else {
	*/
//		if ((STEP_PIN_high) && (STEP_PIN_previous_state == LOW)) {
		if (STEP_PIN_high) {
			// we get a gate input going HIGH
			//STEP_PIN_previous_state = HIGH;
			//forward_next_step();
			//display_number(current_step);
			//display_number(0x03);

			// advance to next step
			// output new values
			
			//update_dac_output(forward_next_step());
			//_delay_ms(100);
		}

//		if ((STEP_PIN_low) && (STEP_PIN_previous_state == HIGH)) {
		if (STEP_PIN_low) {
			// we get a gate input going LOW
			//STEP_PIN_previous_state = LOW;
			//display_number(0x04);
			
			// nothing to do, (gate output should go LOW if in Trig mode, stay unchanged in case of Gate mode)
			// dac output stay unchanged
			//_delay_ms(100);
		}
	//}

}

//http://happyrobotlabs.com/posts/project/artemis-synthesizer-2-interfacing-with-the-mcp4921-spi-dac/
//https://github.com/cwoodall/mcp492x-spidac-interface


void setup(void)
{
	#ifdef USE_UART
		uart_init();
		stdout = &uart_output;
		stdin  = &uart_input;
		puts("UART init OK");
	#endif

	// Initiate SPI in Mode 0 with MSB first, NO interrupts and a clock of F_CPU/2 
	setupSPI(SPI_MODE_0, SPI_MSB, SPI_NO_INTERRUPT, SPI_MASTER_CLK2);

	setup_spi_display();
	setup_spi_leds();
	SETUP_DAC;


	_delay_us(100);


	rotary_setup();

	_delay_us(100);


	// setup input / output pins as input or output
    INOUT_DDR |= (1 << GATE_PIN);	// OUTPUT
    INOUT_DDR &= ~(1 << RESET_PIN);	// INPUT
    INOUT_DDR &= ~(1 << RPUSH_PIN);	// INPUT
    INOUT_DDR &= ~(1 << STEP_PIN);	// INPUT

	// activate pull up resistor
	//PORTD |= (1 << STEP_PIN);		// use pull up if step is active on ground switch (foot pedal...)
	//PORTD |= (1 << RESET_PIN);

	// setup Pin Change interrupts
	PCICR |= (1 << PCIE2);          // Enable PCINT2 interrupt
	PCMSK2 |= (1 << PCINT22);		// watch PD6 pin ("step" clock signal)
	PCMSK2 |= (1 << PCINT23);		// watch PD7 pin ("reset"      signal)


/*
	//dac_data = 0x000;
	dac_data = 12 * DAC_SEMITONE;

	#ifdef USE_UART
	printf("dac_data = %d\n", dac_data);
	#endif

	writeMCP492x(dac_data, dac_config);
//	_delay_ms(5000);
*/
	//update_dac_output(12);

	display_number(0x12);

	// demo sequence
	for (uint8_t i = 0; i < 5; i++)
	{
		//update_spi_leds(0b0101010101010101);
		update_spi_leds(0xAAAA);
		_delay_ms(200);
		update_spi_leds(0x5555);
		_delay_ms(200);
	}
	
	// init gates
/*	for (uint8_t i = 0; i < MAX_STEPS; i++)
	{
		step_gates[i] = LOW;
	}*/

	

	
	trig_out = TRUE;

	// switch off leds
	//update_spi_leds(0);
	//update_spi_leds(0);
	go_step(0);

	/* enable interrupts */
	sei();
}

#ifdef TEST_MODE
void test_gates(void)
{
	// test pattern :
	step_gates[0] = 1;
	step_gates[1] = 0;
	step_gates[2] = 1;
	step_gates[3] = 0;

	step_gates[4] = 1;
	step_gates[5] = 1;
	step_gates[6] = 0;
	step_gates[7] = 0;
}

void test_cv(void)
{
	step_semitones[0] = 100;
	step_semitones[1] = 100;
	step_semitones[2] = 100;
	step_semitones[3] = 100;

	step_semitones[4] = 100;
	step_semitones[5] = 100;
	step_semitones[6] = 100;
	step_semitones[7] = 100;
}


void test_blink_gate_led(void)
{
	while(1)
	{
			PORTD &= ~(1 << GATE_PIN);
			_delay_ms(1000);

			PORTD |= (1 << GATE_PIN);
			_delay_ms(1000);
	}
}


#endif

// detect gate clock
// go to next step
// update outputs
// update displays if not in edition mode

// detect user input
// set edition mode on (start timer)
// modify variables
// update displays (overrides active step display)

// detect end timer edition
// set edition mode off


// unused
void detect_clock_input(void)
{
	// loop until clock input is low (avoid false trigger)
	while (STEP_PIN_high) {}

	// loop until clock input goes up
	while (STEP_PIN_low) {}

}


int main(void)
{
	//_delay_ms(10);
	setup();

	#ifdef TEST_MODE
		test_gates();
		test_cv();

		display_gates(step_gates);
		output_gate(current_step);
		output_cv(current_step);
	#endif



	while(1)
	{
		// manage steps

		// detect positive level on RESET input
		if (RESET_PIN_high)
		{
			// go to step 0 and update outputs then display accordingly
			//PORTD &= ~(1 << GATE_PIN);
			go_step(0);
			output_gate(current_step);
		}
		else
		{
			//PORTD |= (1 << GATE_PIN);
		}


		//display_number(STEP_PIN_previous_state);
		//if (STEP_PIN_previous_state == HIGH)
		if ((STEP_PIN_high) && (STEP_PIN_previous_state == LOW))
		{
			STEP_PIN_previous_state = HIGH;
			forward_next_step();
			output_gate(current_step);
			output_cv(current_step);
			//_delay_ms(50);
		}
		if ((STEP_PIN_low) && (STEP_PIN_previous_state == HIGH))
		{
			STEP_PIN_previous_state = LOW;
			//_delay_ms(50);
		}

		// manage user inputs
		
	}
}

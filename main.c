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

#define TEST_MODE


// ************************ Rotary Encoder *****************************

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

void rotation_CW(void)
{
	#ifdef USE_UART
		printf("CW ");
	#endif

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

	if (in_edition_mode_flag == TRUE)
	{
		if (RPUSH_PIN_released)
		{
			// increase step pointer
			edited_step++;
			if (edited_step > LAST_STEP)
			{
				edited_step = LAST_STEP;
			}
			
			semitone_changed = FALSE;

			#ifdef USE_UART
				printf("edited_step = %d\n", edited_step);
			#endif
		}
		else
		{
			// increase step's semitone value
			semitone_changed = TRUE;

		}
	}
	display_step(edited_step);
	
	start_edition_mode();
}

void rotation_CCW(void)
{
	#ifdef USE_UART
		printf("CCW ");
	#endif

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
	if (in_edition_mode_flag == TRUE)
	{
		if (RPUSH_PIN_released)
		{
			edited_step--;
			if (edited_step < FIRST_STEP)
			{
				edited_step = FIRST_STEP;
			}

			semitone_changed = FALSE;

			#ifdef USE_UART
				printf("edited_step = %d\n", edited_step);
			#endif
		}
		else
		{
			// decrease step's semitone value
			semitone_changed = TRUE;
			
		}
	}
	display_step(edited_step);

	//decrease_rot_increments();
	start_edition_mode();
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
		// we stop the timer
		stop_timer();

        // no timer reset required here as the timer
        // is reset every time it overflows
  
        timer_ovf_counter = 0;   // reset overflow counter
        

        // DO BUSINESS HERE  V  V  V  V  V  V  V

        end_edition_mode();
    }
}

void start_edition_mode(void)
{
	init_timer();
	in_edition_mode_flag = TRUE;

	// show edition mode on decimal point #2 (second digit, far right)
	set_decimal_point(2);

	display_number(0x01);
}

void end_edition_mode(void)
{
	// we're no more in edition mode
	in_edition_mode_flag = FALSE;
	semitone_changed = FALSE;

	// remove edition mode led
	set_decimal_point(0);
	display_number(0x00);

	display_step(current_step);


}










// ************************** DISPLAY **********************************
#define SWITCH_OFF_STEP_LEDS 	step_n_gate_leds = step_n_gate_leds & 0xFF00
#define SWITCH_OFF_GATE_LEDS 	step_n_gate_leds = step_n_gate_leds & 0x00FF

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
	}
	update_step_n_gate_leds();
}

//void display_gates(uint8_t gates[])
void display_gates(void)
{
	// Row 1 : Step		SSSS SSSS xxxx xxxx
	// Row 2 : Gates	xxxx xxxx GGGG GGGG
	
	// steps are BYTE n°... (LSB)
	
	//step_n_gate_leds
	
	// switch off gate leds
	SWITCH_OFF_GATE_LEDS;

	uint8_t gate_leds = 0;

	for (uint8_t i = FIRST_STEP; i < MAX_STEPS; i++)
	{
		//step_n_gate_leds = step_n_gate_leds | (gate_sequence[i] << (i + 8));
		gate_leds = gate_leds | (gate_sequence[i] << (i - 1));
	}

	step_n_gate_leds |= (gate_leds << 8);

	update_step_n_gate_leds();
}

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

void blink_leds(void)
{
	// demo sequence
	for (uint8_t i = 0; i < 5; i++)
	{
		//update_spi_leds(0b0101010101010101);
		update_spi_leds(0xAAAA);
		_delay_ms(200);
		update_spi_leds(0x5555);
		_delay_ms(200);
	}
}





// ************************  outputs  **********************************

#define SET_GATE_high   PORTD &= ~(1 << GATE_PIN)
#define SET_GATE_low    PORTD |= (1 << GATE_PIN)

/*
 * void output_gate(uint8_t step)
{
		if (gate_sequence[step] == HIGH)
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
* */

void output_gate(uint8_t gate)
{
		if (gate == HIGH)
		{
			SET_GATE_high;
			if (trig_out == TRUE)
			{
				_delay_us(TRIGGER_LENGTH);
				SET_GATE_low;
			}
		}
		else
		{
			SET_GATE_low;
			//SET_GATE_low;
		}
}


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

void output_cv(uint8_t semitone)
{
	update_dac_output(semitone * DAC_SEMITONE);
	//update_dac_output(step_semitones[step]);
}


//void update_gate_output(uint8_t gate)
//{
//}

//void update_cv_output(uint8_t semitone)
//{
//}


/*
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
*/

/*
void update_cv_output(uint8_t semitone)
{
	update_dac_output(semitone * DAC_SEMITONE);
}
*/




// ****************************** sequencer ****************************

// go_step activate the step provided, output the corresponding CV and Gate
// display current step and CV value, if not in edition mode
uint8_t go_step(uint8_t step)
{
	#ifdef USE_UART
		printf("go_step(%d);\n", step);
	#endif

	// set current step and update outputs then display accordingly

	if (step > LAST_STEP)
	{
		step = FIRST_STEP;
	}
	current_step = step;
	
	// update outputs
	output_gate(gate_sequence[current_step]);
	//output_cv(semitones_sequence[current_step]);

	// update display
	if (in_edition_mode_flag == FALSE)
	{
		display_step(current_step);
	}
	
	return current_step;
}


uint8_t forward_next_step(void)
{
	// increase current_step pointer
	// update gate output
	// update cv output
	// display current step

	return go_step(current_step + 1);
}

void stop_sequencer(void)
{
	go_step(0);
	display_number(0x00);
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
	PORTD |= (1 << RPUSH_PIN);

	//PORTD |= (1 << STEP_PIN);		// use pull up if step is active on ground switch (foot pedal...)
	//PORTD |= (1 << RESET_PIN);


	stop_sequencer();

	display_number(0xBB);
	blink_leds();
	//display_number(0x12);

	// init gates
/*	for (uint8_t i = 0; i < MAX_STEPS; i++)
	{
		step_gates[i] = LOW;
	}*/

	

	
	//trig_out = TRUE;

	// switch off leds
	//update_spi_leds(0);
	//update_spi_leds(0);

	stop_sequencer();

	/* enable interrupts */
	sei();
}






#ifdef TEST_MODE

void test_gates_sequence(void)
{
	// test sequence :
	gate_sequence[0] = 0;

	gate_sequence[1] = 1;
	gate_sequence[2] = 0;
	gate_sequence[3] = 1;
	gate_sequence[4] = 0;

	gate_sequence[5] = 1;
	gate_sequence[6] = 0;
	gate_sequence[7] = 1;
	gate_sequence[8] = 1;
}

/*
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
*/

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

void test_forward_next_step(void)
{
	while(1)
	{
		forward_next_step();
		_delay_ms(1000);
	}
}

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


int main(void)
{
	setup();

	#ifdef TEST_MODE
		//test_forward_next_step();
		//test_blink_gate_led();
		test_gates_sequence();
		//test_cv();

		//display_gates(step_gates);
		display_gates();
		//output_gate(current_step);
		//output_cv(current_step);
		//display_step(4);
	#endif


	// main loop, where everything happens
	while(1)
	{
		// manage step/reset inputs
		manage_inputs();

		// manage user inputs
		manage_user_inputs();
		
		// update, if necessary, display
		manage_display();
	}
}

void manage_inputs(void)
{
	// detect positive *level* on RESET input
	if (RESET_PIN_high)
	{
		// go to step 0 and update outputs, then display accordingly
		stop_sequencer();
	}
	else
	{
		// detect gate's rising *edges*
		if ((STEP_PIN_high) && (step_pin_previous_state == LOW))
		{
			forward_next_step();

			// keep it for next iteration: only detect rising edges
			step_pin_previous_state = HIGH;
		}

		// and ignore gate's falling *edges*
		if ((STEP_PIN_low) && (step_pin_previous_state == HIGH))
		{
			step_pin_previous_state = LOW;
		}
	}
}


void manage_user_inputs(void)
{
	// is button pushed ?
	if (RPUSH_PIN_pushed)
	{
		display_step(edited_step);

		start_edition_mode();
		_delay_ms(1);

		
		// only triggered when pushed, not maintained nor released
		if (rpush_previous_state != PUSHED)
		{
			rpush_previous_state = PUSHED;

			if (semitone_changed == FALSE)
			{
				
			}
			else
			{
			}
		}
		
	}
	else
	{
		// if button is released (was pushed before) and the knob was not turned to change cv output, then
		if ((rpush_previous_state == PUSHED) && (semitone_changed == FALSE))
		{
			// change gate status for edited step
			if (gate_sequence[edited_step] == HIGH)
			{
				gate_sequence[edited_step] = LOW;
			}
			else
			{
				gate_sequence[edited_step] = HIGH;
			}
			display_gates();
		}
		
		rpush_previous_state = RELEASED;
	}
	
}


/*
 * should NOT use this solution: use instead a simple call to display update functions on demand
void manage_display(void)
{
	if (leds_need_update_flag == TRUE)
	{
		update_step_n_gate_leds();
	}
	
	if display_flag
	{
		update_display());
	}
}
*/

void manage_display(void)
{
	/*if (in_edition_mode_flag == TRUE)
	{
		set_decimal_point(1);
	}*/
}


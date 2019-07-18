// Copyright (c) 2016, Joe Krachey
// All rights reserved.
//
// Redistribution and use in binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in binary form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "main.h"

// ADD CODE

// will be in range 0-100
volatile int8_t DUTY_CYCLE;
volatile bool ALERT_ADC_UPDATE;
volatile bool ALERT_BLUE;



/*******************************************************************************
* Function Name: configure_gpio_pins
********************************************************************************
* Summary: Configures the GPIO pins connected to the PS2 Joystick and the BLUE
*          LED on the launchpad
*
* Return:
*  None
*******************************************************************************/
void configure_hardware(void)
{
  lp_io_init();
  ps2_initialize();
}

/*******************************************************************************
* Function Name: SysTick_Handler
********************************************************************************
* Summary: SysTick Interrupt Service Routine
*
*
* Return: NONE
*  
*
*******************************************************************************/
void SysTick_Handler(void)
{
	uint32_t val;
	static int count = 0;
	
	// set value of GPIO pin connected to the blue LED
	// to a 1 or 0 based on value of DUTY_CYCLE
	// If DUTY_CYCLE = 75, the GPIO pin should be set to 1
	// for 7.5ms and to 0 for 2.5ms
	
	// tell main to turn blue led on
	if (count < DUTY_CYCLE){ALERT_BLUE = true;}
	// tell main to turn blue led off
	else {ALERT_BLUE = false;}
	
	count++;
	
	// Set ALERT_ADC_UPDATE to true once every 10ms
	if (count == 100) 
	{
		ALERT_ADC_UPDATE = true;
		count = 0;
	}

	// Clear the interrupt
	val = SysTick->VAL;
}

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary: Controls the Blue LED on the Tiva Launchpad using the PS2 Joystick to
*          set the duty cycle of the LED.
*
*
* Return:
*  
*
*******************************************************************************/
int 
main(void)
{
  initialize_serial_debug();
  
  put_string("\n\r");
  put_string("********************************************\n\r");
  put_string("ECE353 - SysTick Interrupt In Class Exercise\n\r");
  put_string("*********************************************\n\r");
  
  configure_hardware();
    
  SysTick_Config(SYSTICK_LOAD_VAL);
 
  if( !validate_ice(ICE_SYSTICK) )
  {
     while(1){};
  }
  
  while(1)
  {
	  
		// turn blue LED on
	  if (ALERT_BLUE)
	  {
		  lp_io_set_pin(BLUE_BIT);
		
	  }
		// turn blue LED off
		else {lp_io_clear_pin(BLUE_BIT);}
		
		// if true, update DUTY_CYCLE based on current value
		// of the analog input connected to X direction of PS2 Joystick
    if (ALERT_ADC_UPDATE)
		{
			// ps2_get_x() only sets lower 12 bits, 
			// so max value is (2^12 - 1) = 4095
			// get % by dividing by 4095
			DUTY_CYCLE = ((ps2_get_x()) / 4095.0) * 100;
			
			// toggle ALERT_ADC_UPDATE
			ALERT_ADC_UPDATE = false;
		}
    
  }
}

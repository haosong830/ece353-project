#include "mbed.h"

/*
Kevin Wilson 4/15/19

P14 = SWCLK
P13 = SWDIO
PA10 = MCU_RX
PA9 = MCU_TX
PA7 = SP11 MOSI
PA5 = SP11_CLK
PA3 = push button
PA2 = red LED
PA1 = BUZZER
*/

Ticker led_flipper;

DigitalOut Red_Led(PA_2);

// PA_6 is supposed to be SPI_MISO, for us it is blank
SPI APA102C(PA_7,PA_6,PA_5); 

DigitalIn PushButton(PA_3);

PwmOut Buzzer(PA_1);

Serial pc(PA_9, PA_10, 115200); // UART, TX, RX


//***************************************************************
// Used as a handler for a timer interrupt.  The ISR will be 
// called once every second.  Toggle the pin connected to the 
// Red LED.
//***************************************************************
void ticker_handler(void)
{
    Red_Led = !Red_Led;
}


//***************************************************************
// A start frame for the APA102C LEDs requires 4 bytes to be
// written to the SPI interface.  Each of those bytes should be 
// equal to 0x00.
//***************************************************************
void write_start_frame(void)
{
    APA102C.write(0x00);
	  APA102C.write(0x00);
	  APA102C.write(0x00);
	  APA102C.write(0x00);
}

//***************************************************************
// For each LED in the chain, 4 bytes of data is transmitted.
//
// Byte 0 : 0xFF
// Byte 1 : Blue Value
// Byte 2 : Green Value
// Byte 3 : Red Value
//***************************************************************
void write_led(uint8_t blue, uint8_t green, uint8_t red)
{
    APA102C.write(0xFF);
	  APA102C.write(blue);
	  APA102C.write(green);
	  APA102C.write(red);
}

//***************************************************************
// Write out the end frame (4 bytes of 0xFF)
//***************************************************************
void write_end_frame(void)
{ 
    APA102C.write(0xFF);
	  APA102C.write(0xFF);
	  APA102C.write(0xFF);
	  APA102C.write(0xFF);
}

//***************************************************************
//***************************************************************
void initialize_w_leds(uint8_t num_leds, uint8_t blue, uint8_t green, uint8_t red)
{
    int i;
    
    write_start_frame();
    for(i = 0; i<num_leds; i++)
    {
        write_led(blue, green, red);
    }
    write_end_frame();
}


//***************************************************************
//***************************************************************
int main() {
    
    int i;
    APA102C.format(8,3);
    APA102C.frequency(5000000);
		
	  
    
  //Initialize ticker for blinking red LED
	//The address of the function to be attached (flip) and the interval (1 second)
	led_flipper.attach(&ticker_handler, 1.0);
	
	// Initialize buzzer period to be 2000Hz 
	Buzzer.period(0.0005f);
	
	// turn on all front leds
	initialize_w_leds(47, 0x00, 0x00, 0xFF);
	
	wait(5);
		
    
    while(1) {
				
			// If button pressed, make sound
			if (!PushButton) {
			Buzzer.write(0.50f);
			initialize_w_leds(47, 0xFF, 0x00, 0x00);
			} 
			else {
			Buzzer.write(0.0f);
		  initialize_w_leds(47, 0x00, 0xFF, 0x00);
			}   
    }
		
		
}

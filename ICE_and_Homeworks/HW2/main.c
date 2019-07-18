// Copyright (c) 2015-16, Joe Krachey
// All rights reserved.
//
// Redistribution and use in source or binary form, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in source form must reproduce the above copyright
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

// global variables for saucer x and y positions
uint16_t xPos, yPos;

// declare functions
void loopThroughArray	(void);
void command_Up			(uint32_t num_pixels);
void command_Down		(uint32_t num_pixels);
void command_Left		(uint32_t num_pixels);
void command_Right		(uint32_t num_pixels);
void pauseSaucer		(uint32_t pauseLength);
void drawSaucer 		(uint16_t x, uint16_t y);

//*****************************************************************************
// This is an ARRAY of strings.  If you wanted to access the 3rd string
// ("P10000"), you could do so using COMMANDS[2].
//*****************************************************************************

char *COMMANDS[] =
{
    "P200000",
    "R500",
    "P10000",
    "U1000",
    "P5000",
    "L2000",
    "P2500",
    "D2000",
    "P1250",
    "R94",
    "P1",
    "U142"
};

//*****************************************************************************
//*****************************************************************************
void init_hardware(void)
{
    initialize_serial_debug();
    lcd_config_gpio();
    lcd_config_screen();
    lcd_clear_screen(LCD_COLOR_GREEN);
}


void loopThroughArray(void)
{
	// character indicating which command to do
	char *commandLetter;
	
	// This double pointer will be used to store 
	// the command string from the COMMANDS array.
	char **commandString = COMMANDS;
	
	// The remaining number following the 
	// char specifying which command to do.
	char *numFromCommand_String;
	
	// numFromCommand_String converted to an int using atoi()
	uint32_t numFromCommand_Int;
	
	// Loop through COMMAND array, check first character of 
	// array element, and skip if not U,D,L,R, or P. Perform command if valid.
	// Stop when the pointer doesn't point to anything in the array
	while (*commandString != 0)
	
	{
		/* Add 1 to pointer that points to a command string to remove
		 * the first character and set equal to numFromCommand_String
		 * Example: *commandString 		= R500
		 *							 	  ^pointer starts here
					*commandString + 1 = 500
										 ^pointer starts here							
		*/
		numFromCommand_String = *commandString + 1;
		
		// *commandString holds the whole command but when you set it 
		// equal to a single char pointer, you get the first char of the 
		// command it points to
		commandLetter = *commandString;

		/* Check if U,D,L,R, or P, or invalid. If valid, use atoi() to convert
		 * the string after the first character into an integer, and call appropriate
		 * function to draw saucer.
		*/
		
		// Move saucer up (sub from y location) by specified number of pixels
		if(*commandLetter == 'U')
		{	
			numFromCommand_Int = atoi(numFromCommand_String);
			command_Up(numFromCommand_Int);
		}
		
		// Move saucer up (add to y location) by specified number of pixels
		else if(*commandLetter == 'D')
		{
			numFromCommand_Int = atoi(numFromCommand_String);
			command_Down(numFromCommand_Int);
		}
		
		// Move saucer left (sub from x location) by specified number of pixels
		else if(*commandLetter == 'L')
		{
			numFromCommand_Int = atoi(numFromCommand_String);
			command_Left(numFromCommand_Int);
		}
		
		// Move saucer right (add to x location) by specified number of pixels
		else if(*commandLetter== 'R')
		{
			numFromCommand_Int = atoi(numFromCommand_String);
			command_Right(numFromCommand_Int);
		}
		
		// Pause by specified duration using empty for loop
		else if(*commandLetter == 'P')
		{
			numFromCommand_Int = atoi(numFromCommand_String);
			pauseSaucer(numFromCommand_Int);
		}
		
		// unrecognized command, skip
		else continue;
		
		// increment the double pointer to the next element
		commandString++;
	}
}

// draw the saucer with passed in values of x and y
void drawSaucer(uint16_t x, uint16_t y) 
{		
	// update x and y global variables
	yPos = y;
	xPos = x;
	lcd_draw_image
		(
        x,                       		// X Pos
        space_shipWidthPixels,    		// Image Horizontal Width
        y,                    			// Y Pos
        space_shipHeightPixels,   		// Image Vertical Height
        space_shipBitmaps,        		// Image
        LCD_COLOR_BLUE2,          		// Foreground Color
        LCD_COLOR_WHITE           		// Background Color
		);
}

void command_Up(uint32_t num_pixels)
{	
	// loop variable
	uint16_t i;
	
	// check if going to hit top of screen. Have to cast to int
	// because may get negative number
	if ((int)(yPos - num_pixels) < Y_MIN){
		// draw saucer up until reach top of screen
		for(i = yPos; i >= Y_MIN; i--){
			// keep x fixed, update y
			drawSaucer(xPos,i);
		}
	}
	else {
		// must save because yPos is updated every iteration in the loop
		uint16_t final_y = yPos-num_pixels;
		// draw saucer up the correct # of pixels
		// start at yPos and go until you hit (yPos - num_pixels)
		for(i = yPos; i >= final_y; i--){
			// keep x fixed, update y
			drawSaucer(xPos, i);
		}
	}
}

void command_Down(uint32_t num_pixels)
{
	// loop variable
	uint16_t i;
	
	// check if going to hit bottom top of screen
	if ((yPos + num_pixels) > Y_MAX){
		// draw saucer down until reach bottom of screen
		for(i = yPos; i <= Y_MAX; i++){
			// keep x fixed, update y
			drawSaucer(xPos,i);
		}
	}
	else {
		// must save because yPos is updated every iteration in the loop
		uint16_t final_y = yPos + num_pixels;
		// draw saucer down the correct # of pixels
		// start at yPos and go until you hit (yPos + num_pixels)
		for(i = yPos; i <= final_y; i++){
			// keep x fixed, update y
			drawSaucer(xPos, i);
		}
	}
}

void command_Left(uint32_t num_pixels)
{
	// loop variable
	uint16_t i;
	
	// check if going to hit left of screen
	if ((int)(xPos - num_pixels) < X_MIN){
		// draw saucer left until reach left of screen
		for(i = xPos; i >= X_MIN; i--){
			// update x, keep y fixed
			drawSaucer(i,yPos);
		}
	}
	else {
		// must save because xPos is updated every iteration in the loop
		uint16_t final_x = xPos - num_pixels;
		// draw saucer left the correct # of pixels
		// start at xPos and go until you hit (xPos - num_pixels)
		for(i = xPos; i >= final_x; i--){
			// update x, keep y fixed
			drawSaucer(i,yPos);
		}
	}
}

void command_Right(uint32_t num_pixels)
{
	// loop variable
	uint16_t i;
	
	// check if going to hit right of screen
	if ((xPos + num_pixels) > X_MAX){
		// draw saucer right until reach right of screen
		for(i = xPos; i <= X_MAX; i++){
			// update x, keep y fixed
			drawSaucer(i,yPos);
		}
	}
	else {
		// must save because xPos is updated every iteration in the loop
		uint16_t final_x = xPos + num_pixels;
		// draw saucer right the correct # of pixels
		// start at xPos and go until you hit (xPos + num_pixels)
		for(i = xPos; i <= final_x; i++){
			// update x, keep y fixed
			drawSaucer(i,yPos);
		}
	}
}


void pauseSaucer(uint32_t pauseLength)
{
	uint32_t i;
	// use an empty for loop to wait until we've 
	// paused enough time
	for (i = 0; i <= pauseLength; i++)
	{
		//EMPTY
	}
}

//*****************************************************************************
//*****************************************************************************
int main(void)
{
    init_hardware();


    put_string("\n\r");
    put_string("******************************\n\r");
    put_string("ECE353 HW2 Spring 2019\n\r");
    put_string("Kevin Wilson\n\r");
    put_string("******************************\n\r");
	
	// draw saucer in middle of screen first
	xPos = 120;
	yPos = 160;
	lcd_draw_image
		(
        xPos,                       	// X Pos
        64,    		// Image Horizontal Width
        yPos,                    		// Y Pos
        60,   		// Image Vertical Height
        img,        		// Image
        LCD_COLOR_RED,          		// Foreground Color
        LCD_COLOR_YELLOW           		// Background Color
		);
	
	// loop through the COMMANDS array which 
	// will call appropriate functions to draw the saucer
    loopThroughArray();

 
    // Reach infinite loop
    while(1) {};
}

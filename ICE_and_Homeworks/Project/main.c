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
uint16_t ufo_xPos, ufo_yPos;
extern void hw1_search_memory(uint32_t addr); 

char bulletString[80], scoreString[80];

int16_t x_accel, y_accel, z_accel;
uint16_t x_touch, y_touch;
char en_command[] = "LOAD LED00000FF LED10000FF  LED20000FF  LED30000FF LED40000FF  LED50000FF LED60000FF  LED70000FF HALT";
char clear_command[] = "LOAD LED000000 LED1000000  LED2000000  LED3000000  LED4000000  LED5000000 LED6000000 LED7000000 HALT";

extern bool alert_T4A;
extern bool alert_T1A;
volatile bool readyShoot = false;
volatile uint8_t touch_event = 0;
extern bool alert_btn, fishHit;
extern int score, numBullets;


int i,j;
int count1A = 0;
uint8_t btn_dir;
uint8_t sw_val;

bool debounce = true;

uint16_t fcolor;
uint16_t bcolor;

bool showHUD = true;
bool pause = true;
bool HUD_erased = false;
bool gameStarted = false;
bool colorChange = false;


extern uint16_t colorArray[6];
	


uint8_t colorArrayIndex=0;

//*****************************************************************************
//*****************************************************************************
void init_hardware(void)
{
    DisableInterrupts();
    //************************************************************************
    // Configures the serial debug interface at 115200.
    // UART IRQs can be anbled using the two paramters to the function.
    //************************************************************************
    init_serial_debug(true, true);
		eeprom_init();
    // initialize launchpad
    lp_io_init();
    //enable LCD stuff
    lcd_config_gpio();
    lcd_config_screen();
    lcd_clear_screen(BG_COLOR);
		
		// Initialize the GPIO Port D
		gpio_enable_port(GPIOD_BASE);
		gpio_config_digital_enable(GPIOD_BASE,0xFF);
		gpio_config_enable_output(GPIOD_BASE,0xFF);
	
	// LED timer
		gp_timer_config_32(TIMER1_BASE, PERIODIC, false, true, SEC_ONE);
		
		// Accelerometer timer
		gp_timer_config_16(TIMER4_BASE, PERIODIC, false, true, 1570, TIMER_TAPR_TAPSR_M);

	 // enable io expander
	 io_expander_init();
 
		// I2C touchscreen
		 ft6x06_init();
		 
		 // joystick
		 //ps2_initialize(); 
		
    //enable accelerometer
    accel_initialize();
		
    EnableInterrupts();
}

void DisableInterrupts(void)
{
    __asm
    {
        CPSID  I
    }
}

//*****************************************************************************
//
//*****************************************************************************
void EnableInterrupts(void)
{
    __asm
    {
        CPSIE  I
    }
}




//*****************************************************************************
//*****************************************************************************
void drawInitialImages() {
		lcd_draw_image
    (
        octopus.xPos,                         // X Pos
        octopus.width,        // Image Horizontal Width
        octopus.yPos,                       // Y Pos
        octopus.height,       // Image Vertical Height
        octopus.bitmap,            // Image
        octopus.fColor,              // Foreground Color
        octopus.bColor              // Background Color
    );
		
		lcd_draw_image
    (
        fishArray[0].xPos,                         // X Pos
          fishArray[0].width,        // Image Horizontal Width
          fishArray[0].yPos,                       // Y Pos
          fishArray[0].height,       // Image Vertical Height
          fishArray[0].bitmap,            // Image
          fishArray[0].fColor,              // Foreground Color
          fishArray[0].bColor              // Background Color
    );
		
		lcd_draw_image
    (
        fishArray[1].xPos,                         // X Pos
          fishArray[1].width,        // Image Horizontal Width
          fishArray[1].yPos,                       // Y Pos
          fishArray[1].height,       // Image Vertical Height
          fishArray[1].bitmap,            // Image
          fishArray[1].fColor,              // Foreground Color
          fishArray[1].bColor              // Background Color
    );
			
			
		// black shield
		lcd_draw_box(
			shieldArray[1].xPos, //x start
			shieldArray[1].width, // x len
			shieldArray[1].yPos, //y s start
			shieldArray[1].height, // y len
			shieldArray[1].bColor, //border
			shieldArray[1].fColor, //fill
			shieldArray[1].border_weight
		);
		// blue shield
			lcd_draw_box(
			shieldArray[0].xPos, //x start
			shieldArray[0].width, // x len
			shieldArray[0].yPos, //y s start
			shieldArray[0].height, // y len
			shieldArray[0].bColor, //border
			shieldArray[0].fColor, //fill
			shieldArray[0].border_weight
		);
}

void printStartPage() {
			while(1) {
				fcolor = rand();
				bcolor = rand();  
				for(i = 0; i < 100000; i++){}		//to change color slower
				lcd_draw_image(MID_X,start_width,MID_Y,start_height,start_map,  fcolor , bcolor);
					
				if(alert_btn) {
					btn_dir = io_expander_read_reg(MCP23017_INTCAPB_R);
					//btn_dir = debounce_expander_fsm(btn_dir);
					//printf("%d\n",btn_dir);
					//if(btn_dir == 15) {//when BUTTON is in IDLE state, the value is 0xf
						if(!debounce) {
							eeprom_init_write_read();
							debounce = true;
							break;
						}
					debounce = false;
					alert_btn = false;
				}
			}	
}

// function prints end screen and stays there until user presses reset
void printEndPage() {
		//char startPrompt[80] = "Please press SW2 to begin.\n";
		//print_string_toLCD(startPrompt, 40, 160, LCD_COLOR_WHITE, BG_COLOR);
		uint8_t highScore;
		char finalScoreString[80];
		char highScoreString[80];

		
		// get the current high score out of the eeprom
		eeprom_byte_read(I2C1_BASE,350,&highScore);
		sprintf(highScoreString, "%d", highScore);
	
		// and update if player got new high score
		if (score > highScore) 
		{
			eeprom_byte_write(I2C1_BASE,350,score);
			sprintf(highScoreString, "%d", score);
		}
		
		// the game over screen
		lcd_draw_image(MID_X,240,MID_Y,320,endscreen_Bitmap,LCD_COLOR_BLACK,LCD_COLOR_BLUE2);
		
		sprintf(finalScoreString, "%d", score);
	
		print_string_toLCD(finalScoreString, 180, 130, LCD_COLOR_WHITE, LCD_COLOR_BLUE2);
		print_string_toLCD(highScoreString, 180, 90, LCD_COLOR_WHITE, LCD_COLOR_BLUE2);
		
		printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		printf("Your Score: %d\n", score);
		printf("High Score: %d", highScore);
		
	
		while(1){};
}

void instructionScreen() {
	lcd_clear_screen(LCD_COLOR_BLACK);

		for(i = 0; i < 50; i++) {
		fcolor = rand();
		bcolor = rand();
		lcd_draw_image(20,ARROWS_WIDTH_PIXELS,20,ARROWS_HEIGHT_PIXELS,down_arrowBitmaps,fcolor,LCD_COLOR_BLACK);
		print_string_toLCD("Hide the H U D", MID_X,15,fcolor,LCD_COLOR_BLACK);
		lcd_draw_image(20,ARROWS_WIDTH_PIXELS,120,ARROWS_HEIGHT_PIXELS,up_arrowBitmaps,fcolor,LCD_COLOR_BLACK);
		print_string_toLCD("Pause and Unpause", MID_X,120,fcolor,LCD_COLOR_BLACK);
		lcd_draw_image(20,ARROWS_WIDTH_PIXELS,220,ARROWS_HEIGHT_PIXELS,left_arrowBitmaps,fcolor,LCD_COLOR_BLACK);
		print_string_toLCD("Change colors", MID_X,220,fcolor,LCD_COLOR_BLACK);
		lcd_draw_image(20,ARROWS_WIDTH_PIXELS,300,ARROWS_HEIGHT_PIXELS,right_arrowBitmaps,fcolor,LCD_COLOR_BLACK);
		print_string_toLCD("Change colors", MID_X,300,fcolor,LCD_COLOR_BLACK);
		for(j = 0; j < 200000; j++){}
		}
}

void printGameScreen() {	
			moveShields();
	
	
			moveFish();
			
	
	
			//check numBullets
			if (numBullets == 0)
			{
				printEndPage();
			}
	
			// Check x values of accelerometer
			//printf("touch_event: %d \n", touch_event);
     // sprintf(msg,"X: %d\n\r",x);
			//checkShooting();
      if (x_accel > MOVE_LEFT)
      {
        //put_string("Move left\n\r");
				checkShooting();
				move_Left(octopus.xPos, octopus.yPos, 4, octopus.min_X, octopus.type, &octopus);
				checkShooting();
				//move_Right(shieldArray[0].xPos, shieldArray[0].yPos, 5, shieldArray[0].max_X, shieldArray[0].type, &shieldArray[0]);
				//move_Left(shieldArray[1].xPos, shieldArray[1].yPos, 10,1, shieldArray[1].type, &shieldArray[1]);
      }
      else if (x_accel < MOVE_RIGHT)
      {
				
			  checkShooting();
				move_Right(octopus.xPos, octopus.yPos, 4, octopus.max_X, octopus.type, &octopus);
				checkShooting();
				//move_Right(shieldArray[1].xPos, shieldArray[1].yPos, 5, shieldArray[1].max_X, shieldArray[1].type, &shieldArray[1]);
				//move_Left(shieldArray[0].xPos, shieldArray[0].yPos, 10,1, shieldArray[0].type, &shieldArray[0]);
      
      }
      else
      {
					checkShooting();
      }
			
			
				// check if user wants to see this printed out or not
			if (showHUD) {
				HUD_erased = false;
				// num bullets. Depending on bullet color, BG COLOR will be different.
				lcd_draw_box(0,65, 300, 20, LCD_COLOR_BLACK, LCD_COLOR_RED, 2); // only do if bullets changed
				sprintf(bulletString,"%d bullets:",numBullets);
				print_string_toLCD(bulletString, 7, 310, LCD_COLOR_WHITE, LCD_COLOR_RED);
				
				// score
				lcd_draw_box(0,65, 280, 20, LCD_COLOR_BLACK, LCD_COLOR_GREEN, 2); // only do if score changed
				sprintf(scoreString,"score %d:", score);
				print_string_toLCD(scoreString, 6, 290, LCD_COLOR_BLACK, LCD_COLOR_GREEN);
				// call method to move shields back and forth
			}
			else
			{
				
				if (HUD_erased == false) 
				{
					lcd_draw_box(0,65, 280, 40, BG_COLOR, BG_COLOR, 0);
					HUD_erased = true;
				}
			}
}
/*Thought: have difficulty of Hard and Easy. Both have 30 bullets with a goal of 20 hits. For Hard mode fish will move faster. May have 'bonus' fish*/
int main(void)
{
	int count;
  init_hardware();
	
	//print out setup message
	put_string("\n\r");
  put_string("******************************\n\r");
	put_string("ECE353 SP19 Final Project\n\r");
	put_string("******************************\n\r");
	printStartPage();
	instructionScreen();
	
		// draw all initial things to screen
		
    while(1) //infinite loop of the whole game
    {
	
				// Blink red LEDS
				if(alert_T1A) 
				{
					//put_string("T1A\n");
					if(count1A == 0) 
					{
						lp_io_set_pin(BLUE_BIT);
					}
					else if(count1A == 1) {

						lp_io_clear_pin(BLUE_BIT);
					}
					alert_T1A = false;
					count1A = (count1A + 1) % 2;
				}
		
				if(alert_T4A)
				{
					 if (count == 0)
					 {
						 readyShoot = true;
					 }
					 // check touchscreen
					 touch_event = ft6x06_read_td_status();
					 // read accelerometer
						x_accel = accel_read_x();
						z_accel = accel_read_z();
					 // can shoot abo ut ~0.5s
						count = (count + 1) % 30;
						alert_T4A = false;
				}

				
				// Push buttons
				if(alert_btn) {
					btn_dir = io_expander_read_reg(MCP23017_INTCAPB_R);
						
						switch (btn_dir)
						{
							case BTN_L:
								// loop around array
								if (colorArrayIndex == 0) colorArrayIndex = 5;
								else colorArrayIndex--;
								colorChange = true;
								break;
							
							case BTN_R:
								colorArrayIndex = (colorArrayIndex + 1) % 6;
								colorChange = true;
								break;
								
							case BTN_U:
								showHUD = !showHUD;
								break;
							
							case BTN_D:
								pause = !pause;
								break;
							
							default:
								break;
						}	
					alert_btn = false;
			}
			
			
				 // blink top leds if fish was hit
				if (fishHit == true)
				{
					enableLeds();
					
					for (i = 0; i < 100000; i++) {}

					disableLeds();
			
					fishHit = false;
					//hw1_search_memory((uint32_t) clear_command);
				}
				
				
				// if game not started, draw images 
				
				// if not pausing, print everything out
				if (pause == false) 
				{
					// update bullet color and octopus color
					bullet.fColor = colorArray[colorArrayIndex];
					octopus.fColor = colorArray[colorArrayIndex];
					
					// if haven't been drawn yet, draw initial images
					if (gameStarted == false) {
						lcd_clear_screen(BG_COLOR);
						drawInitialImages();
						gameStarted = true;
					}
					
					// redraw octopus if color changed
					if (colorChange == true) 
					{
						lcd_draw_image
						(
								octopus.xPos,                         // X Pos
								octopus.width,        // Image Horizontal Width
								octopus.yPos,                       // Y Pos
								octopus.height,       // Image Vertical Height
								octopus.bitmap,            // Image
								octopus.fColor,              // Foreground Color
								octopus.bColor              // Background Color
						);
						// wait until another button press
						colorChange = false;
					}
				
					printGameScreen();
				}
		}	
		
			
}
#include "game.h"
extern bool readyShoot;
extern uint8_t touch_event;


int score = 0;
int numBullets = 7;
bool fishHit = false;

// possible colors for the fish
uint16_t colorArray[6] = {LCD_COLOR_RED, LCD_COLOR_GREEN, LCD_COLOR_ORANGE,
													LCD_COLOR_WHITE, LCD_COLOR_BLACK, LCD_COLOR_YELLOW};

_GameCharacter octopus ={58,54, //width, height
												120,OCTOPUS_Y_MAX, //x, y
												octopus_Bitmap,
												LCD_COLOR_WHITE,
												LCD_COLOR_BLUE,
												"character",
												OCTOPUS_X_MAX,
												OCTOPUS_X_MIN,
												false, // unneeded for octopus
												false};

												
												
										
_GameCharacter fishArray[] =
{
	
	//Fish 0
	{
		39, 31, // width,height
		130, 205, // xPos, yPos
		fishLeft_Bitmap,
		LCD_COLOR_BLACK,
		BG_COLOR,
		"character",
		FISH_X_MAX,
		FISH_Y_MIN,
		false,				// move right
		false
	},

	//Fish 1
	{
		39, 31, // width,height
		10, 19,
		fishRight_Bitmap,
		LCD_COLOR_GREEN,
		BG_COLOR,
		"character",
		FISH_X_MAX,
		FISH_Y_MIN,
		true,
		false
	},
	
	//Fish 2
	{
		39, 31, // width,height
		210, 100, // x, y
		fishRight_Bitmap,
		LCD_COLOR_ORANGE,
		BG_COLOR,
		"character",
		FISH_X_MAX,
		FISH_Y_MIN,
		true,
		false
	},
	
	//Fish 3
	{
		39, 31, // width,height
		75, 150, // x, y
		fishLeft_Bitmap,
		LCD_COLOR_YELLOW,
		BG_COLOR,
		"character",
		FISH_X_MAX,
		FISH_Y_MIN,
		false,
		false
	}
	
};
							


_GameObj shieldArray[] =
{	
	// Rectangle 0
	{50, 5, 1, 			// width, height, border
	100, 50, 					//x,y
	LCD_COLOR_YELLOW,		// fill
	BG_COLOR,  //border	
	"object",
	239 - 50,					// max x
	1,								// min y (top of screen)
	true,
	false},
	
	//Rectangle 1
	{30, 7, 1, 				// width, height, border
	10, 120, 						//x,y
	LCD_COLOR_BLACK,		// fill
	BG_COLOR,  	//border	
	"object",
	239 - 30,
	10,									// min y
	false,							// moveRight
	false},
	
	//Rectangle 2
	{30, 6, 1, 				// width, height, border
	5, 250, 					//x,y
	LCD_COLOR_WHITE,		// fill
	BG_COLOR,  				//border	
	"object",
	239 - 70,
	20,								// min y
	false,						// moveRight
	false}
	

																				
};				

uint8_t numShields = sizeof(shieldArray)/ sizeof(shieldArray[0]);
uint8_t numFish = sizeof(fishArray) / sizeof(fishArray[0]);

_GameObj bullet = {10, 10, 1, 			// width, height, border
									0, 0, 					//x,y
									LCD_COLOR_RED,		// fill
									BG_COLOR,  //border	
									"object",
									239 - 10,
									1,							//min y, probably take out
									false}; 
											
void move_Left(uint16_t xPos, 
							uint16_t yPos, 
							uint32_t num_pixels, 
							uint16_t minX,
							const char type[],
							void* ptr)
{
	// loop variable
	uint16_t i;
	
	// check if going to hit left of screen
	if ((int)(xPos - num_pixels) < minX){
		// draw saucer left until reach left of screen
		// flip its direction
		for(i = xPos; i >= minX; i--){
			// update x, keep y fixed
			//lcd_clear_screen(LCD_COLOR_GREEN);
			if (!strcmp(type,"character")) drawCharacter((_GameCharacter*)ptr, i, yPos); 
			else if (!strcmp(type, "object"))  drawObject((_GameObj*) ptr, i, yPos);
		}
	}
	else {
		// must save because xPos is updated every iteration in the loop
		uint16_t final_x = xPos - num_pixels;
		// draw saucer left the correct # of pixels
		// start at xPos and go until you hit (xPos - num_pixels)
		for(i = xPos; i >= final_x; i--){
			// update x, keep y fixed
			//lcd_clear_screen(LCD_COLOR_GREEN);
			if (!strcmp(type,"character")) drawCharacter((_GameCharacter*)ptr, i, yPos); 
			else if (!strcmp(type, "object"))  drawObject((_GameObj*) ptr, i, yPos);
		}
	}
}


void move_Right(uint16_t xPos, 
								uint16_t yPos, 
								uint32_t num_pixels, 
								uint16_t maxX,
								const char type[],
								void* ptr)
{
	// loop variable
	uint16_t i;
	
	// check if going to hit right of screen
	if ((xPos + num_pixels) > maxX){
		// draw saucer right until reach right of screen
		for(i = xPos; i <= maxX; i++){
			// update x, keep y fixed
			//lcd_clear_screen(LCD_COLOR_GREEN);
			if (!strcmp(type,"character")) drawCharacter((_GameCharacter*)ptr, i, yPos);
			else if (!strcmp(type, "object"))  drawObject((_GameObj*) ptr, i, yPos);
		}
	}
	else {
		// must save because xPos is updated every iteration in the loop
		uint16_t final_x = xPos + num_pixels;
		// draw saucer right the correct # of pixels
		// start at xPos and go until you hit (xPos + num_pixels)
		for(i = xPos; i <= final_x; i++){
			// update x, keep y fixed
			//lcd_clear_screen(LCD_COLOR_GREEN);
			if (!strcmp(type,"character")) drawCharacter((_GameCharacter*)ptr, i, yPos); 
			else if (!strcmp(type, "object"))  drawObject((_GameObj*) ptr, i, yPos);
		}
	}
}



void shootBullet (uint16_t xPos, 
								  uint16_t yPos, 
								  _GameObj* obj)
{	
	// loop variable
	uint16_t i,j;
	obj->yPos = yPos;
	obj->xPos = xPos;
	
	// while bullet hasnt reached top of screen or hit a shield
	while ( (obj->yPos > 1) && !obj->hit)
	{
		obj->yPos--;
		
		drawObject(obj, obj->xPos, obj->yPos);
		
		// Check if bullet has hit any shields
		 for (i = 0; i < numShields; i++)
		{
					 if (obj->yPos == (shieldArray[i].yPos + shieldArray[i].height) &&
						   obj->xPos < (shieldArray[i].xPos + shieldArray[i].width) &&
							 (obj->xPos + obj->width) >= shieldArray[i].xPos)
					 {
						 obj->hit = true;
		
					 }	 
		// empty loop to make bullet move slower
		for (j = 0; j<5000; j++){}
		}
		
		// Check if bullet has hit any fish
		 for (i = 0; i < numFish; i++)
		{
					 if (obj->yPos == (fishArray[i].yPos + fishArray[i].height) && // height collison
						   obj->xPos < (fishArray[i].xPos + fishArray[i].width/2) && // less than right side of fish
							 (obj->xPos + obj->width + 13) >= fishArray[i].xPos && 	 // greater than left side of fish]
					     obj->fColor == fishArray[i].fColor)  												// same color bullet as fish
				
					 {
						 obj->hit = true;
						 fishHit = true;
						 fishArray[i].hit = true;
						 numBullets++;
						 score++;
						 break;
					 }	 
		}
		
		
		// empty loop to make bullet move slower
	for (j = 0; j<5000; j++){}
		
		if (fishHit == true)
		{
			for (i = 0; i < numFish; i++)
			{
				// erase old fish
				lcd_draw_image
				(
        fishArray[i].xPos,                         // X Pos
        fishArray[i].width,        // Image Horizontal Width
        fishArray[i].yPos,                       // Y Pos
        fishArray[i].height,       // Image Vertical Height
        fishArray[i].bitmap,            // Image
        fishArray[i].bColor,              // Foreground Color
        fishArray[i].bColor              // Background Color
				);
				
				// randomly switch color of fish
				fishArray[i].fColor = colorArray[(rand() % 6)];
				fishArray[i].xPos = rand() % 200;
			}
		}
	}
	
	
	
	// erase bullet
	lcd_draw_box(obj->xPos, //x start
								obj->width, // x len
								obj->yPos, //y s start
								obj->height, // y len
								BG_COLOR, //border
								BG_COLOR, //fill
								obj->border_weight
								);
	obj->hit = false;
}


// draw the character based on struct
void drawCharacter(_GameCharacter* character, uint16_t x, uint16_t y) 
{		
	// update x and y struct variables
	character->yPos = y;
	character->xPos = x;
	lcd_draw_image
    (
        x,         							// X Pos
        character->width,        // Image Horizontal Width
        y,         							// Y Pos
        character->height,       // Image Vertical Height
        character->bitmap,       // Image
        character->fColor,       // Foreground Color
        character->bColor        // Background Color
    );
}


void drawObject(_GameObj* obj, uint16_t x, uint16_t y)
{
	//printf("x: %d\n", x);
	obj->yPos = y;
	obj->xPos = x;
	
	lcd_draw_box(
			x, //x start
			obj->width, // x len
			y, //y s start
			obj->height, // y len
			obj->bColor, //border
			obj->fColor, //fill
			obj->border_weight
		);
}

void moveShields()
	{
		int numPixels,i;
			// Move the shields
			for (i = 0; i < numShields; i++)
			{
				// generate random number to move by in range 1 to 10
				numPixels = (rand() % 10) + 1;
				
				if (shieldArray[i].moveRight) 
				{
						// check if should switch direction
						if (shieldArray[i].xPos + numPixels >= shieldArray[i].max_X) shieldArray[i].moveRight = false;
						move_Right(shieldArray[i].xPos, shieldArray[i].yPos, numPixels, shieldArray[i].max_X, shieldArray[i].type, &shieldArray[i]);
					
				}
				else
				{
					if (shieldArray[i].xPos - numPixels <= 1) shieldArray[i].moveRight = true;
					move_Left(shieldArray[i].xPos, shieldArray[i].yPos, numPixels, 1, shieldArray[i].type, &shieldArray[i]);
				}
			}
		}
	
void moveFish()
	{
		int numPixels,i;
			// Move the shields
			for (i = 0; i < numFish; i++)
			{
				// generate random number to move by in range 1 to 10
				numPixels = (rand() % 10) + 1;
				if (fishArray[i].moveRight) 
				{
						// check if should switch direction
						if (fishArray[i].xPos + numPixels >= fishArray[i].max_X - 10)
						{
							fishArray[i].moveRight = false;
							fishArray[i].bitmap = fishLeft_Bitmap;
						}

						move_Right(fishArray[i].xPos, fishArray[i].yPos, numPixels, fishArray[i].max_X, fishArray[i].type, &fishArray[i]);
				}
				else
				{
					if (fishArray[i].xPos - numPixels <= 20)
					{
						fishArray[i].moveRight = true;
						
						
						
						fishArray[i].bitmap = fishRight_Bitmap;
					}
					move_Left(fishArray[i].xPos, fishArray[i].yPos, numPixels, 1, fishArray[i].type, &fishArray[i]);
				}
			}
		}


void checkShooting()
{
			int i;
 
				// after moving check if you should shoot
			if (readyShoot && (touch_event > 0))
				 {
					 
					 // have to wait again until ready to shoot
					 readyShoot = false;
					 numBullets--;
					 shootBullet(octopus.xPos -5, octopus.yPos - octopus.height/2 - 9, &bullet);
				 }
}



#include "ws2812b.h"


void ws2812b_rotate( WS2812B_t *base, uint8_t num_leds)
{
	uint8_t i;
	
	// temp value for the highest index
	WS2812B_t finalIndex = base[num_leds - 1];
	
	for (i = num_leds - 1; i > 0; i--)
	{
			// Example: Base[8] = Base[7]
			base[i] = base[i-1];
	}
	
	// Set base[0] to highest index. ROTATE RIGHT
	base[0] = finalIndex;
}
  
  
void ws2812b_pulse( WS2812B_t *base, uint8_t num_leds)
  {
	  // indicates if incrementing or decrementing
	  static uint8_t direction;
	  uint8_t i;

	  for (i = 0; i < num_leds; i++)
	  {
		  if ((direction) && (base[i].red < 0xFF))
			{
				 base[i].red += 1;  
		  }
		  
		  else if ((direction) && (base[i].red == 0xFF))
		  {
			  direction = !direction;
				base[i].red -= 1;  
		  }
		  
		  else if ((!direction) && (base[i].red > 0x00))
		  {
				 base[i].red -= 1; 
		  }
		  
		  else if ((!direction) && (base[i].red == 0x00))
		  {
			   direction = !direction;
				 base[i].red += 1; 
				
		  }
	  }  
  }


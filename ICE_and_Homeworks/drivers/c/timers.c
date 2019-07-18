#include "timers.h"


uint32_t status;
extern int byte_count;
volatile int16_t x,y,z;
extern void hw1_search_memory(uint32_t addr);
volatile bool alert_T1A = false;
volatile bool alert_T4A = false;

//*****************************************************************************
// Verifies that the base address is a valid GPIO base address
//*****************************************************************************
static bool verify_base_addr(uint32_t base_addr)
{
   switch( base_addr )
   {
     case TIMER0_BASE:
     case TIMER1_BASE:
     case TIMER2_BASE:
     case TIMER3_BASE:
     case TIMER4_BASE:
     case TIMER5_BASE:
     {
       return true;
     }
     default:
     {
       return false;
     }
   }
}

IRQn_Type timerA_get_irq_num(uint32_t base)
{
   switch(base)
   {
     case TIMER0_BASE:
     {
       return TIMER0A_IRQn;
     }
     case TIMER1_BASE:
     {
       return TIMER1A_IRQn;
     }
     case TIMER2_BASE:
     {
        return TIMER2A_IRQn;
     }
     case TIMER3_BASE:
     {
       return TIMER3A_IRQn;
     }
     case TIMER4_BASE:
     {
       return TIMER4A_IRQn;
     }
     case TIMER5_BASE:
     {
       return TIMER5A_IRQn;
     }
     default:
     {
       return 0;
     }
   }
}
//*****************************************************************************
// Returns the RCGC and PR masks for a given TIMER base address
//*****************************************************************************
static bool get_clock_masks(uint32_t base_addr, uint32_t *timer_rcgc_mask, uint32_t *timer_pr_mask)
{
  // Set the timer_rcgc_mask and timer_pr_mask using the appropriate
  // #defines in ../include/sysctrl.h
  switch(base_addr)
  {
    case TIMER0_BASE:
    {
      *timer_rcgc_mask = SYSCTL_RCGCTIMER_R0;
      *timer_pr_mask = SYSCTL_PRTIMER_R0;
      break;
    }
    case TIMER1_BASE:
    {
      *timer_rcgc_mask = SYSCTL_RCGCTIMER_R1;
      *timer_pr_mask = SYSCTL_PRTIMER_R1;
      break;
    }
    case TIMER2_BASE:
    {
      *timer_rcgc_mask = SYSCTL_RCGCTIMER_R2;
      *timer_pr_mask = SYSCTL_PRTIMER_R2;
      break;
    }
    case TIMER3_BASE:
    {
      *timer_rcgc_mask = SYSCTL_RCGCTIMER_R3;
      *timer_pr_mask = SYSCTL_PRTIMER_R3;
      break;
    }
    case TIMER4_BASE:
    {
      *timer_rcgc_mask = SYSCTL_RCGCTIMER_R4;
      *timer_pr_mask = SYSCTL_PRTIMER_R4;
      break;
    }
    case TIMER5_BASE:
    {
      *timer_rcgc_mask = SYSCTL_RCGCTIMER_R5;
      *timer_pr_mask = SYSCTL_PRTIMER_R5;
      break;
    }
    default:
    {
      return false;
    }
  }
  return true;
}


//*****************************************************************************
// Waits for 'ticks' number of clock cycles and then returns.
//
//The function returns true if the base_addr is a valid general purpose timer
//*****************************************************************************
bool gp_timer_wait(uint32_t base_addr, uint32_t ticks)
{
  TIMER0_Type *gp_timer;
  
  // Verify the base address.
  if ( ! verify_base_addr(base_addr) )
  {
    return false;
  }

  // Type cast the base address to a TIMER0_Type struct
  gp_timer = (TIMER0_Type *)base_addr;

  //*********************    
  // ADD CODE
  //*********************
  gp_timer->CTL &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);
	gp_timer->TAILR = ticks;
	gp_timer->ICR |= TIMER_ICR_TATOCINT;
	gp_timer->CTL |= TIMER_CTL_TAEN;
	while(!(gp_timer->RIS & TIMER_RIS_TATORIS)){}
  return true;
}



//*****************************************************************************
// Configure a general purpose timer to be a 32-bit timer.  
//
// Paramters
//  base_address          The base address of a general purpose timer
//
//  mode                  bit mask for Periodic, One-Shot, or Capture
//
//  count_up              When true, the timer counts up.  When false, it counts
//                        down
//
//  enable_interrupts     When set to true, the timer generates and interrupt
//                        when the timer expires.  When set to false, the timer
//                        does not generate interrupts.
//
//The function returns true if the base_addr is a valid general purpose timer
//*****************************************************************************
bool gp_timer_config_32(uint32_t base_addr, uint32_t mode, bool count_up, bool enable_interrupts, uint32_t ticks)
{
  uint32_t timer_rcgc_mask;
  uint32_t timer_pr_mask;
  TIMER0_Type *gp_timer;
  
  // Verify the base address.
  if ( ! verify_base_addr(base_addr) )
  {
    return false;
  }
  
  // get the correct RCGC and PR masks for the base address
  get_clock_masks(base_addr, &timer_rcgc_mask, &timer_pr_mask);
  // Turn on the clock for the timer
  SYSCTL->RCGCTIMER |= timer_rcgc_mask;

  // Wait for the timer to turn on
  while( (SYSCTL->PRTIMER & timer_pr_mask) == 0) {};
  
  // Type cast the base address to a TIMER0_Type struct
  gp_timer = (TIMER0_Type *)base_addr;
    
  //*********************    
  // ADD CODE
  //*********************
  gp_timer->CTL &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);
  gp_timer->CFG = TIMER_CFG_32_BIT_TIMER;
  gp_timer->TAMR &= ~TIMER_TAMR_TAMR_M;
  gp_timer->TAMR |= mode;	
	gp_timer->TAILR = ticks;
	if(count_up) gp_timer->TAMR |= TIMER_TAMR_TACDIR;
	else gp_timer->TAMR &= ~TIMER_TAMR_TACDIR;
	if(enable_interrupts) {
		gp_timer->IMR |= TIMER_IMR_TATOIM;
		enableTimerIRQ(base_addr);
	}
	else gp_timer->IMR &= ~TIMER_IMR_TATOIM;
	gp_timer->ICR |= TIMER_ICR_TATOCINT;
	gp_timer->CTL |= TIMER_CTL_TAEN;
  return true;  
}

bool gp_timer_config_16(uint32_t base_addr, uint32_t mode, bool count_up, bool enable_interrupts, uint32_t ticks, uint8_t presclr)
{
  uint32_t timer_rcgc_mask;
  uint32_t timer_pr_mask;
  TIMER0_Type *gp_timer;
  
  // Verify the base address.
  if ( ! verify_base_addr(base_addr) )
  {
    return false;
  }
  
  // get the correct RCGC and PR masks for the base address
  get_clock_masks(base_addr, &timer_rcgc_mask, &timer_pr_mask);
	
  // Turn on the clock for the timer
  SYSCTL->RCGCTIMER |= timer_rcgc_mask;

  // Wait for the timer to turn on
  while( (SYSCTL->PRTIMER & timer_pr_mask) == 0) {};
  
  // Type cast the base address to a TIMER0_Type struct
  gp_timer = (TIMER0_Type *)base_addr;
    
  //*********************    
  // ADD CODE
  //*********************
  gp_timer->CTL &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);

  gp_timer->CFG = TIMER_CFG_16_BIT;
  gp_timer->TAMR &= ~TIMER_TAMR_TAMR_M;
	//gp_timer->TBMR &= ~TIMER_TBMR_TBMR_M;
  gp_timer->TAMR |= mode;	
	gp_timer->TAILR = ticks;
	gp_timer->TAPR |= presclr & TIMER_TAPR_TAPSR_M;
	if(count_up) gp_timer->TAMR |= TIMER_TAMR_TACDIR;
	else gp_timer->TAMR &= ~TIMER_TAMR_TACDIR;
	if(enable_interrupts) {
		gp_timer->IMR |= TIMER_IMR_TATOIM;
		enableTimerIRQ(base_addr);
	}
	else gp_timer->IMR &= ~TIMER_IMR_TATOIM;
	gp_timer->ICR |= TIMER_ICR_TATOCINT;
	gp_timer->CTL |= TIMER_CTL_TAEN;
  return true;  
}

// LED BLINKER
void TIMER1A_Handler(void){
		//puts("entered timer1A handler\n");
		status = TIMER1->MIS & TIMER_MIS_TATOMIS;
		if(status) {
			alert_T1A = true;
		}
		TIMER1->ICR |= TIMER_ICR_TATOCINT;
}

// ACCELEROMETER
void TIMER4A_Handler(void){
	if(TIMER4->MIS & TIMER_MIS_TATOMIS) {
			alert_T4A = true;
			TIMER4->ICR |= TIMER_ICR_TATOCINT;
	}
}



void enableTimerIRQ(uint32_t base)
{
	//IRQn_Type irq;
	//irq = timerA_get_irq_num(base);
	NVIC_SetPriority(TIMER1A_IRQn, 1);
	NVIC_SetPriority(TIMER4A_IRQn, 2);
	NVIC_EnableIRQ(TIMER1A_IRQn);
	NVIC_EnableIRQ(TIMER4A_IRQn);

	//NVIC_SetPriority(irq, 1);
	//NVIC_EnableIRQ(irq);
		
}



	
////*****************************************************************************
//// Configure a general purpose timer1A to be a 32-bit timer that generates
//// interrupts once every 5 secs. This timer is used to print the number of
//// bytes transmitted and received by the RN4870(bluetooth wireless module)
////
//// Paramters
////  base_address          The base address of a general purpose timer1A
////
////  mode                  bit mask for Periodic, One-Shot, or Capture
////
////  count_up              When true, the timer counts up.  When false, it counts
////                        down
////
////  enable_interrupts     When set to true, the timer generates and interrupt
////                        when the timer expires.  When set to false, the timer
////                        does not generate interrupts.
////
////The function returns true if the base_addr is a valid general purpose timer
////*****************************************************************************
//bool gp_timer1A_config_32(uint32_t base_addr, uint32_t mode, bool count_up, bool enable_interrupts)
//{
//  uint32_t timer_rcgc_mask;
//  uint32_t timer_pr_mask;
//  
//  // Verify the base address.
//  if ( ! verify_base_addr_timer1A(base_addr) )
//  {
//    return false;
//  }
//  
//  // get the correct RCGC and PR masks for the base address
//  get_clock_masks(base_addr, &timer_rcgc_mask, &timer_pr_mask);
//  
//  // Turn on the clock for the timer
//  SYSCTL->RCGCTIMER |= timer_rcgc_mask;

//  // Wait for the timer to turn on
//  while( (SYSCTL->PRTIMER & timer_pr_mask) == 0) {};
//  
//    
//  //*********************    
//  // ADD CODE
//  //*********************
//  TIMER1->CTL &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);
//  TIMER1->CFG = TIMER_CFG_32_BIT_TIMER;
//  TIMER1->TAMR &= ~TIMER_TAMR_TAMR_M;
//  TIMER1->TAMR |= mode;	
//	if(count_up) TIMER1->TAMR |= TIMER_TAMR_TACDIR;
//	else TIMER1->TAMR &= ~TIMER_TAMR_TACDIR;
//	if(enable_interrupts) TIMER1->IMR |= TIMER_IMR_TATOIM;
//	else TIMER1->IMR &= ~TIMER_IMR_TATOIM;
//  return true;  
//}

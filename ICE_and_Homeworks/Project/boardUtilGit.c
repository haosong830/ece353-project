// Copyright (c) 2014, Joe Krachey
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

#include "TM4C123GH6PM.h"
#include "boardUtil.h"
#include "../include/sysctrl.h"



void DisableInterrupts(void)
{
  __asm {
         CPSID  I
  }
}
void EnableInterrupts(void)
{
  __asm {
    CPSIE  I
  }
}




#define UART0_BUFFER_SIZE 80
PC_Buffer UART0_Rx_Buffer;
char UART0_Rx_Buffer_Array[UART0_BUFFER_SIZE];

PC_Buffer UART0_Tx_Buffer;
char UART0_Tx_Buffer_Array[UART0_BUFFER_SIZE];

//*****************************************************************************
// Configure PA0 and PA1 to be UART pins
//*****************************************************************************
void uart0_config_gpio(void)
{
   gpio_enable_port(GPIOA_BASE);
   gpio_config_digital_enable( GPIOA_BASE, PA0 | PA1);
   gpio_config_alternate_function( GPIOA_BASE, PA0 | PA1);
   gpio_config_port_control( GPIOA_BASE, GPIO_PCTL_PA0_U0RX | GPIO_PCTL_PA1_U0TX);
}

//*****************************************************************************
//*****************************************************************************
void serialDebugInit(void)
{
  // Configure GPIO Pins
  uart0_config_gpio();
  
  // Configure Circular Buffers
  pc_buffer_init(&UART0_Rx_Buffer , UART0_Rx_Buffer_Array, UART0_BUFFER_SIZE); 
  pc_buffer_init(&UART0_Tx_Buffer , UART0_Tx_Buffer_Array, UART0_BUFFER_SIZE); 
  
  // Initialize UART0 for 8N1, interrupts enabled.
  uart_init_115K(
    UART0_BASE, 
    SYSCTL_RCGCUART_R0, 
    SYSCTL_PRUART_R0, 
    UART0_IRQn,
    1
  );
}

//*****************************************************************************
// Initialize the I2C peripheral
//*****************************************************************************
void i2cInit(void)
{
  
  gpio_enable_port(EEPROM_GPIO_BASE);
  
  // Configure SCL 
  gpio_config_digital_enable(EEPROM_GPIO_BASE, EEPROM_I2C_SCL_PIN);
  gpio_config_alternate_function(EEPROM_GPIO_BASE, EEPROM_I2C_SCL_PIN);
  gpio_config_port_control(EEPROM_GPIO_BASE, EEPROM_I2C_SCL_PIN_PCTL);

  
  // Configure SDA 
  gpio_config_digital_enable(EEPROM_GPIO_BASE, EEPROM_I2C_SDA_PIN);
  gpio_config_open_drain(EEPROM_GPIO_BASE, EEPROM_I2C_SDA_PIN);
  gpio_config_alternate_function(EEPROM_GPIO_BASE, EEPROM_I2C_SDA_PIN);
  gpio_config_port_control(EEPROM_GPIO_BASE, EEPROM_I2C_SDA_PIN_PCTL);
  
  //  Initialize the I2C peripheral
  initializeI2CMaster(EEPROM_I2C_BASE);
  
/**
 * @file SYSTICK.c
 * @author Mariam Mohammed
 * @brief This is the SysTick driver implementation
 * @version 0.1
 * @date 2020-03-28
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Std_Types.h"
#include "SYSTICK_CONF.h"
#include "SYSTICK.h"


typedef struct
{
  u32 CTRL;
  u32 LOAD;
  u32 VAL;
  u32 CALIB;  
} SYSTICK_regMap;

#define SYSTICK_BASE_ADDRESS 0xE000E010
#define SYSTICK_peripheral ((volatile SYSTICK_regMap *) SYSTICK_BASE_ADDRESS)


#define SYSTICK_ENABLE_SETMASK  0x00000001
#define SYSTICK_TICKINT_SETMASK 0x00000002
#define SYSTICK_CLKSRC_SETMASK  0x00000004


static SYSTICK_cbF AppCbF;

/**
 * @brief The initialization of the SysTick
 * 
 */
void SYSTICK_init(void)
{
  (SYSTICK_peripheral->CTRL) |= SYSTICK_TICKINT_SETMASK;
  (SYSTICK_peripheral->CTRL) &= ~SYSTICK_CLKSRC_SETMASK;
  (SYSTICK_peripheral->CTRL) |= SYSTICK_CLKSRC_PRE;
}
/**
 * @brief Starts the Systick
 * 
 */
void SYSTICK_start(void)
{
  (SYSTICK_peripheral->CTRL) |= SYSTICK_ENABLE_SETMASK;
}
/**
 * @brief Stops the timer
 * 
 */
void SYSTICK_stop(void)
{
  (SYSTICK_peripheral->CTRL) &= ~SYSTICK_ENABLE_SETMASK;
}
/**
 * @brief Sets the timer for a specific time
 * 
 * @param time the time in milli seconds
 * @param AHB_clockHz the AHB clock in Kilo Hz
 */
void SYSTICK_setTime(u32 time, u32 AHB_clockHz)
{
  u32 clock = AHB_clockHz;
  if (((SYSTICK_peripheral->CTRL) & SYSTICK_CLKSRC_SETMASK) == SYSTICK_CLKSRC_AHB_DIV_8)
  {
    clock >>= 3;
  }
  
  f32 countFloat = time * (clock/1000000.0);
  u32 count = (u32)countFloat & 0x00FFFFFF;
  (SYSTICK_peripheral->LOAD) = count;
}
/**
 * @brief Sets the callback function
 * 
 * @param cbF the function to set
 */
void SYSTICK_setCallbackFcn(SYSTICK_cbF cbF)
{
  if(cbF)
  {
    AppCbF = cbF;
  }
}
/**
 * @brief The SysTick Handler
 * 
 */
void SysTick_Handler(void)
{
  if(AppCbF)
  {
    AppCbF();
  }
}

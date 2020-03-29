/**
 * @file App.c
 * @author Mariam Mohammed
 * @brief This is an application for testing the UART and the LCD drivers
 * @version 0.1
 * @date 2020-03-28
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Std_Types.h"
#include <stdlib.h>
#include "stdio.h"
#include "HUart_Cfg.h"
#include "HUart.h"
#include "Clcd.h"
#include "Switch_Cfg.h"
#include "Switch.h"
#include "Led_Cfg.h"
#include "Led.h"
#include "App.h"
/**
 * @brief This is the frame type of size 4 byte
 * 
 */
typedef union
{
    uint8_t data[4];
    uint32_t fullFrame;
}frame_t;

frame_t recFrame, sendFrame;

static volatile u32 counter = 0;

/**
 * @brief This is the initialization for the two counter application
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
Std_ReturnType APP_init(void)
{
  Std_ReturnType error = E_OK;
  Led_Init();
  Switch_Init();
  error |= CLcd_Init(CLCD_TWO_LINES, CLCD_CURSOR_OFF, CLCD_BLINKING_OFF);
  HUart_Init();
  HUart_SetRxCb(APP_receiveFcn);
  error |= HUart_Receive(recFrame.data, 4);
  return error;
}

/**
 * @brief The free running task that comes every 1 milli second
 * 
 */
void APP_sendTask(void)
{
  static u8 prevSwitchStat = SWITCH_NOT_PRESSED;
  static u8 currentSwitchState = SWITCH_NOT_PRESSED;

  Switch_GetSwitchStatus(SWITCH_1, &currentSwitchState);

  if (currentSwitchState == SWITCH_PRESSED && prevSwitchStat == SWITCH_NOT_PRESSED)
  {
    counter++;
    prevSwitchStat = SWITCH_PRESSED;
    sendFrame.fullFrame = counter;
    HUart_Send(sendFrame.data, 4);
  }
   prevSwitchStat = currentSwitchState;
}



/**
 * @brief The receive function that will be called after each received frame
 *
 */
void APP_receiveFcn(void)
{
  char strBuffer[20];

  /* Toggle Led */
  static u8 ledStat = LED_ON;
  ledStat = !ledStat;
  
  Led_SetLedStatus(LED_1, ledStat);
  
  /* Display on LCD */
  itoa(recFrame.fullFrame, strBuffer, 10);
  CLcd_WriteString((uint8_t*)strBuffer, 0, 0);
  HUart_Receive(recFrame.data, 4);
}

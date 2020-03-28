/**
 * @file HRcc.h
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is implementation for the RCC Handler
 * @version 0.1
 * @date 2020-03-24
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Std_Types.h"
#include "RCC.h"
#include "Gpio.h"

/**
 * @brief This function initializes the system clock
 * 
 * @return Std_ReturnType 
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
Std_ReturnType HRcc_SystemClockInit(void)
{
    RCC_setClockState(RCC_HSE_ON, RCC_ON);
    RCC_selectSystemClock(RCC_sysClk_HSE);
    return E_OK;
}

/**
 * @brief This function initializes the clock for a specific GPIO port
 * 
 * @param port The GPIO port
 *                  GPIO_PORTX : The pin number you want to configure
 * @return Std_ReturnType  
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
Std_ReturnType HRcc_EnPortClock(uint32_t port)
{
    Std_ReturnType error = E_OK;
    switch(port)
    {
      case GPIO_PORTA:
        RCC_controlAPB2Peripheral(RCC_GPIOA, ENABLE);
        break;
      case GPIO_PORTB:
        RCC_controlAPB2Peripheral(RCC_GPIOB, ENABLE);
        break;
      case GPIO_PORTC:
        RCC_controlAPB2Peripheral(RCC_GPIOC, ENABLE);
        break;
      case GPIO_PORTD:
        RCC_controlAPB2Peripheral(RCC_GPIOD, ENABLE);
        break;
      case GPIO_PORTE:
        RCC_controlAPB2Peripheral(RCC_GPIOE, ENABLE);
        break;
      case GPIO_PORTF:
        RCC_controlAPB2Peripheral(RCC_GPIOF, ENABLE);
        break;
      case GPIO_PORTG:
        RCC_controlAPB2Peripheral(RCC_GPIOG, ENABLE);
        break;
      default:
        error = E_NOT_OK;
        break;
    }
    return error;
}

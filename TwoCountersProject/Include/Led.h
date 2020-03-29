/**
 * @file  Led.h 
 * @brief This file is to be used as an interface for the user of the Led Handler.
 *
 * @author Mark Attia
 * @date January 22, 2020
 *
 */
#ifndef LED_H
#define LED_H

typedef struct
{
    uint32_t pin;
    uint32_t port;
    uint8_t activeState;
} led_t;

#define LED_ON              0
#define LED_OFF             !LED_ON

/**
 * Function:  Led_Init 
 * --------------------
 *  @brief Initializes GPIOs for the LEDs
 * 
 *  
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType Led_Init(void);

/**
 * Function:  Led_SetLedOn 
 * --------------------
 *  @brief Sets the Led on
 * 
 *  @param ledName: The name of the LED
 *  
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType Led_SetLedOn(uint8_t ledName);

/**
 * Function:  Led_SetLedOff 
 * --------------------
 *  @brief Sets the Led off
 * 
 *  @param ledName The name of the LED
 *  
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType Led_SetLedOff(uint8_t ledName);

/**
 * Function:  Led_SetLedStatus 
 * --------------------
 *  @brief Sets the Led off
 * 
 *  @param ledName: The name of the LED
 *                  
 *  @param pinStatus: The status of the pin (GPIO_PIN_SET/GPIO_PIN_RESET)
 *                 LED_ON : Sets the pin value to 1
 *                 LED_OFF : Resets the pin value to 0
 *  
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType Led_SetLedStatus(uint8_t ledName, uint8_t status);


#endif

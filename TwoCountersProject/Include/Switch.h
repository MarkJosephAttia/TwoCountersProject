/**
 * @file  Switch.h 
 * @brief This file is to be used as an interface for the user of the Switch Handler.
 *
 * @author Mark Attia
 * @date January 22, 2020
 *
 */
#ifndef SWITCH_H
#define SWITCH_H

typedef struct
{
    uint32_t pin;
    uint32_t port;
    uint8_t activeState;
} switch_t;

#define SWITCH_NOT_PRESSED              1
#define SWITCH_PRESSED                  0

/**
 * Function:  Switch_Init 
 * --------------------
 *  @brief Initializes GPIOs for the Switches
 * 
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType Switch_Init(void);

/**
 * Function:  Switch_GetSwitchStatus 
 * --------------------
 *  @brief Gets the status of the switch
 * 
 *  @param switchName: The name of the Switch
 *                   
 *  @param state: Save the status of the switch in
 *                 SWITCH_PRESSED : if the switch is pressed
 *                 SWITCH_NOT_PRESSED : if the switch is not pressed
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType Switch_GetSwitchStatus(uint8_t switchName, uint8_t* state);

/**
 * @brief The running task of the switch driver to get the state of all of the switches
 * 
 */
extern void Switch_Task(void);

#endif

/**
 * @file  Gpio.c 
 * @brief This file is to be used as an implementation of the GPIO driver.
 *
 * @author Mark Attia
 * @date February 6, 2020
 *
 */
#include "Std_Types.h"
#include "Gpio.h"

#define     GPIO_CR                     0x00
#define     GPIO_IDR                    0x08
#define     GPIO_ODR                    0x0C
#define     GPIO_BSR                    0x10
#define     GPIO_BRR                    0x14
#define     GPIO_LCK                    0x18

#define GPIO_MODE_INPUT_MASK            0xF0
#define GPIO_MODE_MASK                  0x0C

/**
 * Function:  Gpio_InitPins 
 * --------------------
 *
 *  @brief Initializes pins mode and speed for a specific port
 *
 *  @param gpio: An object of type gpio_t to set pins for
 *  
 *  
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType Gpio_InitPins(gpio_t* gpio)
{
    uint8_t i = 0;
    uint64_t tmpReg = *(uint64_t*)(GPIO_CR + gpio->port);
    uint64_t position;
    uint16_t currentPin;
    uint8_t modeSpeed;
    uint8_t isInput = GPIO_MODE_INPUT_MASK & gpio->mode;
    if(isInput)
    {
        modeSpeed = GPIO_MODE_MASK & gpio->mode;
        for(i=0; i<16; i++)
        {
            currentPin = 1<<i;
            if(gpio->pins & currentPin)
            {
                position = i<<2;
                tmpReg &= (uint64_t)~((uint64_t)0xF << (uint64_t)position);
                tmpReg |=  (uint64_t)modeSpeed << (uint64_t)position;
            }            
        }
        switch(gpio->mode)
        {
            case GPIO_MODE_INPUT_PULL_DOWN:
                *(uint32_t*)(GPIO_BRR + gpio->port) = gpio->pins;
                break;
            case GPIO_MODE_INPUT_PULL_UP:
                *(uint32_t*)(GPIO_BSR + gpio->port) = gpio->pins;
                break;
        }
    }
    else
    {
        modeSpeed = gpio->mode | gpio->speed;
        for(i=0; i<16; i++)
        {
            currentPin = 1<<i;
            if(gpio->pins & currentPin)
            {
                position = i<<2;
                tmpReg &= (uint64_t)~((uint64_t)0xF << (uint64_t)position);
                tmpReg |=  (uint64_t)modeSpeed << (uint64_t)position;
            }            
        }
    }
    *(uint64_t*)(GPIO_CR + gpio->port) = tmpReg;
    return E_OK;
}

/**
 * Function:  Gpio_WritePin 
 * --------------------
 *  @brief Write a value to a pin(0/1)
 *
 *  @param port: The port you want to configure
 *                 GPIO_PORTX : The pin number you want to configure  
 * 
 *  @param pin: The pin you want to configure
 *                 GPIO_PIN_X : The pin number you want to configure
 *                 //You can OR more than one pin\\
 *
 *  @param pinStatus: The status of the pins (GPIO_PIN_SET/GPIO_PIN_RESET)
 *                 GPIO_PIN_SET : Sets the pin value to 1
 *                 GPIO_PIN_RESET : Resets the pin value to 0
 *  
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType Gpio_WritePin(uint32_t port, uint32_t pin, uint32_t pinStatus)
{
    Std_ReturnType errorRet = E_NOT_OK;
    switch(pinStatus)
    {
        case GPIO_PIN_SET:
            *(uint32_t*)(GPIO_BSR + port) = pin;
            errorRet = E_OK;
            break;
        case GPIO_PIN_RESET:
            *(uint32_t*)(GPIO_BRR + port) = pin;
            errorRet = E_OK;
            break;
    }
    return errorRet;
}

/**
 * Function:  Gpio_ReadPin 
 * --------------------
 *  @brief Reads a value to a pin(0/1)
 *
 *  @param port: The port you want to read from
 *                 GPIO_PORTX : The pin number you want to read from  
 * 
 *  @param pin: The pin you want to read
 *                 GPIO_PIN_X : The pin number you want to read
 *                 //You can OR more than one pin\\
 *
 *  @param state: To return a status in
 *                  GPIO_PIN_SET : The pin is set to 1
 *                  GPIO_PIN_RESET : The pin is set to 0
 *  @returns: A status
 *              E_OK : if the function is executed correctly
 *              E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType Gpio_ReadPin(uint32_t port, uint32_t pin, uint8_t* state)
{
    *state = !(*(uint32_t*)(GPIO_IDR + port) & pin);
    return E_OK;
}
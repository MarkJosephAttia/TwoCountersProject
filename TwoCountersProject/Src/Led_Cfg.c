/**
 * @file Led_Cfg.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief Those are the User's configurations for the LED Driver
 * @version 0.1
 * @date 2020-03-28
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Std_Types.h"
#include "Gpio.h"
#include "Led_Cfg.h"
#include "Led.h"

const led_t Led_leds[LED_NUMBER_OF_LEDS] = {
    {GPIO_PIN_13, GPIO_PORTC, GPIO_PIN_RESET}
};

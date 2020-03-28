/**
 * @file CLcd_Cfg.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief The user's configuations
 * @version 0.1
 * @date 2020-03-26
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Std_Types.h"
#include "Gpio.h"
#include "CLcd.h"

const clcd_t CLcd_clcd = {
    .enPin = GPIO_PIN_2,
    .enPort = GPIO_PORTA,
    .rwPin = GPIO_PIN_1,
    .rwPort = GPIO_PORTA,
    .rsPin = GPIO_PIN_0,
    .rsPort = GPIO_PORTA,
    .dPin = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6},
    .dPort = {GPIO_PORTA, GPIO_PORTA, GPIO_PORTA, GPIO_PORTA}
};

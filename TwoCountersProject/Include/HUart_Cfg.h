/**
 * @file HUart_Cfg.h
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief These are the user's configurations for the HUART driver
 * @version 0.1
 * @date 2020-03-27
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef HUART_CFG_H
#define HUART_CFG_H

#define HUART_SYSTEM_CLK             8000000

#define HUART_DEFAULT_BAUDRATE       9600
#define HUART_DEFAULT_STOP_BITS      HUART_STOP_ONE_BIT
#define HUART_DEFAULT_PARITY         HUART_NO_PARITY
#define HUART_DEFAULT_FLOW_CONTROL   HUART_FLOW_CONTROL_DIS

#define HUART_DEFAULT_MODULE         HUART_MODULE_1

#endif
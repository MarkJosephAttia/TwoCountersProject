/**
 * @file HUart.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the implementation for the UART handler
 * @version 0.1
 * @date 2020-03-29
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Std_Types.h"
#include "Uart.h"
#include "HUart_Cfg.h"
#include "HUart.h"
#include "NVIC.h"
#include "RCC.h"
#include "Gpio.h"

/* Protection */
#ifndef HUART_DEFAULT_MODULE
#define HUART_DEFAULT_MODULE       HUART_MODULE_1
#endif

#define UART_QUEUE_LENGTH             5

#define UART_NUMBER_OF_MODULES        5

#define HUART_NOT_INITIALIZED         1
#define HUART_INITIALIZED             0
#define HUART_NOT_CONFIGURED          0
#define HUART_CONFIGURED              1

typedef struct
{
    uint32_t baudRate;
    uint32_t stopBits;
    uint32_t parity;
    uint32_t flowControl;

}hUartConfig_t;

typedef struct
{
    uint8_t* data;
    uint16_t len;

}hUartPacket_t;

typedef struct
{
    uint8_t nPackets;
    hUartPacket_t packet[UART_QUEUE_LENGTH];
}hUartQueue_t;

static volatile hUartQueue_t HUart_rxQueue[UART_NUMBER_OF_MODULES];
static volatile hUartQueue_t HUart_txQueue[UART_NUMBER_OF_MODULES];

static volatile hUartConfig_t HUart_config[UART_NUMBER_OF_MODULES];

static volatile uint8_t HUart_module =  HUART_DEFAULT_MODULE;
static volatile uint8_t isInitialized[UART_NUMBER_OF_MODULES] = {HUART_NOT_INITIALIZED, HUART_NOT_INITIALIZED, HUART_NOT_INITIALIZED, HUART_NOT_INITIALIZED, HUART_NOT_INITIALIZED};
static volatile uint8_t isConfigured[UART_NUMBER_OF_MODULES] =  {HUART_NOT_CONFIGURED, HUART_NOT_CONFIGURED, HUART_NOT_CONFIGURED, HUART_NOT_CONFIGURED, HUART_NOT_CONFIGURED};

/**
 * @brief A push request into a queue
 * 
 * @param queue The desired queue
 * @param packet The packet to push
 * @return Std_ReturnType 
 */
static Std_ReturnType HUart_QueuePush(volatile hUartQueue_t* queue, hUartPacket_t* packet)
{
    Std_ReturnType error = E_NOT_OK;
    if(queue->nPackets < UART_QUEUE_LENGTH)
    {
        queue->packet[queue->nPackets].data = packet->data;
        queue->packet[queue->nPackets].len = packet->len;
        queue->nPackets++;
        error = E_OK;
    }
    return error;
}

/**
 * @brief A get request from the queue
 * 
 * @param queue The desired queue
 * @param packet The packet to push
 * @return Std_ReturnType 
 */
static Std_ReturnType HUart_QueueGet(volatile hUartQueue_t* queue, hUartPacket_t* packet)
{
    Std_ReturnType error = E_NOT_OK;
    if(queue->nPackets > 0)
    {
        packet->data = queue->packet[0].data;
        packet->len = queue->packet[0].len;
        error = E_OK;
    }
    return error;
}
/**
 * @brief A pop request from the queue
 * 
 * @param hUartQueue_t the queue to pop from
 * @return Std_ReturnType 
 */
static Std_ReturnType HUart_QueuePop(volatile hUartQueue_t* queue)
{
    Std_ReturnType error = E_NOT_OK;
    uint8_t i;
    if(queue->nPackets > 0)
    {
        for(i=0; i<queue->nPackets-1; i++)
        {
            queue->packet[i].data = queue->packet[i + 1].data;
            queue->packet[i].len = queue->packet[i + 1].len;
        }
        queue->nPackets--;
        error = E_OK;
    }
    return error;
}

/**
 * @brief Initializes the UART Module
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
Std_ReturnType HUart_Init(void)
{
    gpio_t gpio;
    if(HUART_NOT_CONFIGURED == isConfigured[HUart_module])
    {
        HUart_config[HUart_module].baudRate = HUART_DEFAULT_BAUDRATE;
        HUart_config[HUart_module].stopBits = HUART_DEFAULT_STOP_BITS;
        HUart_config[HUart_module].parity = HUART_DEFAULT_PARITY;
        HUart_config[HUart_module].flowControl = HUART_FLOW_CONTROL_DIS;
    }
    switch(HUart_module)
    {
        case HUART_MODULE_1:
            RCC_controlAPB2Peripheral(RCC_GPIOA, ENABLE);
            gpio.pins = GPIO_PIN_9;
            gpio.port = GPIO_PORTA;
            gpio.mode = GPIO_MODE_AF_OUTPUT_PP;
            gpio.speed = GPIO_SPEED_50_MHZ;
            Gpio_InitPins(&gpio);
            gpio.pins = GPIO_PIN_10;
            gpio.mode = GPIO_MODE_INPUT_PULL_UP;
            Gpio_InitPins(&gpio);
            RCC_controlAPB2Peripheral(RCC_USART1, ENABLE);
            NVIC_controlInterrupt(NVIC_IRQNUM_USART1, NVIC_ENABLE);
            break;
        case HUART_MODULE_2:
            RCC_controlAPB2Peripheral(RCC_GPIOA, ENABLE);
            gpio.pins = GPIO_PIN_2;
            gpio.port = GPIO_PORTA;
            gpio.mode = GPIO_MODE_AF_OUTPUT_PP;
            gpio.speed = GPIO_SPEED_50_MHZ;
            Gpio_InitPins(&gpio);
            gpio.pins = GPIO_PIN_3;
            gpio.mode = GPIO_MODE_INPUT_PULL_UP;
            Gpio_InitPins(&gpio);
            RCC_controlAPB1Peripheral(RCC_USART2, ENABLE);
            NVIC_controlInterrupt(NVIC_IRQNUM_USART2, NVIC_ENABLE);
            break;
        case HUART_MODULE_3:
            RCC_controlAPB2Peripheral(RCC_GPIOB, ENABLE);
            gpio.pins = GPIO_PIN_10;
            gpio.port = GPIO_PORTB;
            gpio.mode = GPIO_MODE_AF_OUTPUT_PP;
            gpio.speed = GPIO_SPEED_50_MHZ;
            Gpio_InitPins(&gpio);
            gpio.pins = GPIO_PIN_11;
            gpio.mode = GPIO_MODE_INPUT_PULL_UP;
            Gpio_InitPins(&gpio);
            RCC_controlAPB1Peripheral(RCC_USART3, ENABLE);
            NVIC_controlInterrupt(NVIC_IRQNUM_USART3, NVIC_ENABLE);
            break;
        case HUART_MODULE_4:
            RCC_controlAPB1Peripheral(RCC_UART4, ENABLE);
            NVIC_controlInterrupt(NVIC_IRQNUM_UART4, NVIC_ENABLE);
            break;
        case HUART_MODULE_5:
            RCC_controlAPB1Peripheral(RCC_UART5, ENABLE);
            NVIC_controlInterrupt(NVIC_IRQNUM_UART5, NVIC_ENABLE);
            break;
    }
    Uart_Init(HUart_config[HUart_module].baudRate, HUart_config[HUart_module].stopBits, HUart_config[HUart_module].parity, HUart_config[HUart_module].flowControl, HUART_SYSTEM_CLK, HUart_module);
    isInitialized[HUart_module] = HUART_INITIALIZED;
    return E_OK;
}
/**
 * @brief Sets configurations for the UART module
 * *The UART must be initialized after setting configurations to apply the changes
 * 
 * @param baudRate the baud rate of the UART (uint32_t)
 * @param stopBits The number of the stop bits
 *                 HUART_ONE_STOP_BIT
 *                 HUART_TWO_STOP_BITS
 * @param parity The parity of the transmission
 *                 HUART_ODD_PARITY
 *                 HUART_EVEN_PARITY
 *                 HUART_NO_PARITY
 * @param flowControl the flow control
 *                 HUART_FLOW_CONTROL_EN
 *                 HUART_FLOW_CONTROL_DIS
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
Std_ReturnType HUart_Config(uint32_t baudRate, uint32_t stopBits, uint32_t parity, uint32_t flowControl)
{
    HUart_config[HUart_module].baudRate = baudRate;
    HUart_config[HUart_module].stopBits = stopBits;
    HUart_config[HUart_module].parity = parity;
    HUart_config[HUart_module].flowControl = flowControl;
    isConfigured[HUart_module] = HUART_CONFIGURED;
    return E_OK;
}
/**
 * @brief Sets the module that you will be using
 * 
 * @param uartModule The UART module
 *                  HUART_MODULE_1
 *                  HUART_MODULE_2
 *                  HUART_MODULE_3
 *                  HUART_MODULE_4
 *                  HUART_MODULE_5
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
Std_ReturnType HUart_SetModule(uint8_t uartModule)
{
    HUart_module = uartModule;
    return E_OK;
}
/**
 * @brief Sends data through the UART
 *
 * @param data The data to send
 * @param length the length of the data in bytes
 * @return Std_ReturnType A Status
 *                  E_OK: If the driver is ready to send
 *                  E_NOT_OK: If the driver can't send data right now
 */
Std_ReturnType HUart_Send(uint8_t *data, uint16_t length)
{
    Std_ReturnType error = E_NOT_OK;
    hUartPacket_t pack;
    if(HUART_INITIALIZED == isInitialized[HUart_module])
    {
        pack.data = data;
        pack.len = length;
        error = HUart_QueuePush(&HUart_txQueue[HUart_module], &pack);
    }
    return error;
}
/**
 * @brief Receives data through the UART
 *
 * @param data The buffer to receive data in
 * @param length the length of the data in bytes
 * @return Std_ReturnType A Status
 *                  E_OK: If the driver is ready to receive
 *                  E_NOT_OK: If the driver can't receive data right now
 */
Std_ReturnType HUart_Receive(uint8_t *data, uint16_t length)
{
    Std_ReturnType error = E_NOT_OK;
    hUartPacket_t pack;
    if(HUART_INITIALIZED == isInitialized[HUart_module])
    {
        pack.data = data;
        pack.len = length;
        error = HUart_QueuePush(&HUart_rxQueue[HUart_module], &pack);
    }
    return error;
}
/**
 * @brief Sets the callback function that will be called when receive is
 * completed
 *
 * @param func the callback function
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
Std_ReturnType HUart_SetRxCb(hUartRxCb_t func)
{
    Uart_SetRxCb(func, HUart_module);
    return E_OK;
}
/**
 * @brief Sets the callback function that will be called when transmission is
 * completed
 *
 * @param func the callback function
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
Std_ReturnType HUart_SetTxCb(hUartTxCb_t func)
{
    Uart_SetTxCb(func, HUart_module);
    return E_OK;
}

/**
 * @brief The HUart Running task to handle the UART Requests
 * 
 */
void HUart_Task(void)
{
    uint8_t i;
    hUartPacket_t packet;
    for(i=0; i<UART_NUMBER_OF_MODULES; i++)
    {
        if(E_OK == HUart_QueueGet(&HUart_rxQueue[i], &packet))
        {
            if(E_OK == Uart_Receive(packet.data, packet.len, i))
            {
                HUart_QueuePop(&HUart_rxQueue[i]);
            }
        }
        if(E_OK == HUart_QueueGet(&HUart_txQueue[i], &packet))
        {
            if(E_OK == Uart_Send(packet.data, packet.len, i))
            {
                HUart_QueuePop(&HUart_txQueue[i]);
            }
        }
    }
}

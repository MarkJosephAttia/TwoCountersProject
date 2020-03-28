/**
 * @file Uart.c
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the implementation for the UART driver
 * @version 0.1
 * @date 2020-03-26
 *
 * @copyright Copyright (c) 2020
 *
 */
#include "Std_Types.h"
#include "Uart_Cfg.h"
#include "Uart.h"
#include "Gpio.h"
#include "NVIC.h"
#include "RCC.h"


typedef struct 
{
  uint32_t SR;
  uint32_t DR;
  uint32_t BRR;
  uint32_t CR1;
  uint32_t CR2;
  uint32_t CR3;
  uint32_t GTPR;
} uart_t;

typedef struct 
{
  uint8_t *ptr;
  uint32_t pos;
  uint32_t size;
  uint8_t state;
} dataBuffer_t;

#define UART_INT_NUMBER 37

#define UART_BUFFER_IDLE 0
#define UART_BUFFER_BUSY 1

/*Transmit data register
              empty*/
#define UART_TXE_CLR 0xFFFFFF7F
/*Transmission complete*/
#define UART_TC_CLR 0xFFFFFFBF
/*Read data register not
              empty*/
#define UART_RXNE_CLR 0xFFFFFFDF
/*Parity error*/
#define UART_PE_CLR 0xFFFFFFFE
/*Data value*/
#define UART_DR_CLR 0xFFFFFE00
/*STOP bits*/
#define UART_STOP_CLR 0xFFFFCFFF
/*TXE interrupt enable*/
#define UART_TXEIE_CLR 0xFFFFFF7F
/*Parity selection*/
#define UART_PS_CLR 0xFFFFFDFF
/*Word length*/
#define	UART_M_CLR				0xFFFFEFFF

/*Transmit data register
              empty*/
#define UART_TXE_GET 0x00000080
/*Transmission complete*/
#define UART_TC_GET 0x00000040
/*Read data register not
              empty*/
#define UART_RXNE_GET 0x00000020
/*Parity error*/
#define UART_PE_GET 0x00000001

/*USART enable*/
#define UART_UE_SET 0x00002000
/*Parity control enable*/
#define UART_PCE_SET 0x00000400

/*PE interrupt enable*/
#define UART_PEIE_SET 0x00000100
/*TXE interrupt enable*/
#define UART_TXEIE_SET 0x00000080
/*Transmission complete interrupt
              enable*/
#define UART_TCIE_SET 0x00000040
/*RXNE interrupt enable*/
#define UART_RXNEIE_SET 0x00000020
/*IDLE interrupt enable*/
#define UART_IDLEIE_SET 0x00000010
/*Transmitter enable*/
#define UART_TE_SET 0x00000008
/*Receiver enable*/
#define UART_RE_SET 0x00000004
/*Word length*/
#define	UART_M_SET 0x00001000

/*RTS enable*/
#define UART_RTSE_CLR 0xFFFFFEFF

#define UART_NO_PRESCALER 0x1

static volatile dataBuffer_t txBuffer;
static volatile dataBuffer_t rxBuffer;

static volatile txCb_t appTxNotify;
static volatile rxCb_t appRxNotify;

static volatile uart_t *UART1 = (volatile uart_t *)(0x40013800);

/**
 * @brief The interrupt handler for the UART 1 module
 * 
 */
void USART1_IRQHandler(void) 
{
  if (UART_TXE_GET & UART1->SR) 
  {
    if (txBuffer.size != txBuffer.pos) 
    {
      UART1->DR = txBuffer.ptr[txBuffer.pos++];
    } 
    else 
    {
      txBuffer.ptr = NULL;
      txBuffer.size = 0;
      txBuffer.pos = 0;
      txBuffer.state = UART_BUFFER_IDLE;
      if (appTxNotify) 
      {
        appTxNotify();
      }
      UART1->CR1 &= UART_TXEIE_CLR;
    }
  }

  if (UART_RXNE_GET & UART1->SR) 
  {
    UART1->SR &= UART_RXNE_CLR;
    if (UART_BUFFER_BUSY == rxBuffer.state) 
    {
      rxBuffer.ptr[rxBuffer.pos] = UART1->DR;
      rxBuffer.pos++;

      if (rxBuffer.pos == rxBuffer.size) 
      {
        rxBuffer.ptr = NULL;
        rxBuffer.size = 0;
        rxBuffer.pos = 0;
        rxBuffer.state = UART_BUFFER_IDLE;
        if (appRxNotify) 
        {
          appRxNotify();
        }
      }
    }
  }
}

/**
 * @brief Initializes the UART
 *
 * @param baudRate the baud rate of the UART (uint32_t)
 * @param stopBits The number of the stop bits
 *                 UART_ONE_STOP_BIT
 *                 UART_TWO_STOP_BITS
 * @param parity The parity of the transmission
 *                 UART_ODD_PARITY
 *                 UART_EVEN_PARITY
 *                 UART_NO_PARITY
 * @param flowControl the flow control
 *                 UART_FLOW_CONTROL_EN
 *                 UART_FLOW_CONTROL_DIS
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
extern Std_ReturnType Uart_Init(uint32_t baudRate, uint32_t stopBits, uint32_t parity, uint32_t flowControl) 
{
  gpio_t gpio;
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

  UART1->BRR = ((uint32_t)((f64)UART_SYSTEM_CLK / ((f64)baudRate * 16.0))) << 4;
  if (UART_NO_PARITY == parity) 
  {
    UART1->CR1 &= UART_M_CLR;
    UART1->CR1 &= UART_NO_PARITY;
  } 
  else 
  {
    UART1->CR1 |= UART_M_SET;
    UART1->CR1 |= UART_PCE_SET;
    UART1->CR1 &= UART_PS_CLR;
    UART1->CR1 |= parity;
  }
  UART1->CR2 &= UART_STOP_CLR;
  UART1->CR2 |= stopBits;
  UART1->CR3 &= UART_RTSE_CLR;
  UART1->CR3 |= flowControl;
  UART1->GTPR |= UART_NO_PRESCALER;
  rxBuffer.state = UART_BUFFER_IDLE;
  txBuffer.state = UART_BUFFER_IDLE;
  UART1->CR1 |= UART_UE_SET | UART_TXEIE_SET | UART_RXNEIE_SET | UART_TE_SET | UART_RE_SET;
  NVIC_controlInterrupt(NVIC_IRQNUM_USART1, NVIC_ENABLE);
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
Std_ReturnType Uart_Send(uint8_t *data, uint16_t length) 
{
  Std_ReturnType error = E_NOT_OK;
  if (data && (length > 0) && txBuffer.state == UART_BUFFER_IDLE) 
  {
    txBuffer.state = UART_BUFFER_BUSY;
    txBuffer.ptr = data;
    txBuffer.pos = 0;
    txBuffer.size = length;

    UART1->DR = txBuffer.ptr[txBuffer.pos++];
    UART1->CR1 |= UART_TXEIE_SET;
    error = E_OK;
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
Std_ReturnType Uart_Receive(uint8_t *data, uint16_t length) 
{
  Std_ReturnType error = E_NOT_OK;
  if (rxBuffer.state == UART_BUFFER_IDLE) 
  {
    rxBuffer.ptr = data;
    rxBuffer.size = length;
    rxBuffer.pos = 0;
    rxBuffer.state = UART_BUFFER_BUSY;
    error = E_OK;
  }
  return error;
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
Std_ReturnType Uart_SetTxCb(txCb_t func) 
{
  appTxNotify = func;
  return E_OK;
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
Std_ReturnType Uart_SetRxCb(rxCb_t func) 
{
  appRxNotify = func;
  return E_OK;
}

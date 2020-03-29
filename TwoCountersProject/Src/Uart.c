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
#include "Uart.h"

#define UART_NUMBER_OF_MODULES        5

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

const uint32_t Uart_Address[UART_NUMBER_OF_MODULES] = {
  0x40013800,
  0x40004400,
  0x40004800,
  0x40004C00,
  0x40005000
};

static volatile dataBuffer_t txBuffer[UART_NUMBER_OF_MODULES];
static volatile dataBuffer_t rxBuffer[UART_NUMBER_OF_MODULES];

static volatile txCb_t appTxNotify[UART_NUMBER_OF_MODULES];
static volatile rxCb_t appRxNotify[UART_NUMBER_OF_MODULES];
/**
 * @brief The Interrupt Handler for the UART driver
 * 
 * @param uartModule the module number of the UART
 *                 UART1
 *                 UART2
 *                 UART3
 *                 UART4
 *                 UART5
 */
static void UART_IRQHandler(uint8_t uartModule)
{
  volatile uart_t* Uart = (volatile uart_t*)Uart_Address[uartModule];
  if (UART_TXE_GET & Uart->SR) 
  {
    if (txBuffer[uartModule].size != txBuffer[uartModule].pos) 
    {
      Uart->DR = txBuffer[uartModule].ptr[txBuffer[uartModule].pos++];
    } 
    else 
    {
      txBuffer[uartModule].ptr = NULL;
      txBuffer[uartModule].size = 0;
      txBuffer[uartModule].pos = 0;
      txBuffer[uartModule].state = UART_BUFFER_IDLE;
      if (appTxNotify[uartModule]) 
      {
        appTxNotify[uartModule]();
      }
      Uart->CR1 &= UART_TXEIE_CLR;
    }
  }

  if (UART_RXNE_GET & Uart->SR) 
  {
    Uart->SR &= UART_RXNE_CLR;
    if (UART_BUFFER_BUSY == rxBuffer[uartModule].state) 
    {
      rxBuffer[uartModule].ptr[rxBuffer[uartModule].pos] = Uart->DR;
      rxBuffer[uartModule].pos++;

      if (rxBuffer[uartModule].pos == rxBuffer[uartModule].size) 
      {
        rxBuffer[uartModule].ptr = NULL;
        rxBuffer[uartModule].size = 0;
        rxBuffer[uartModule].pos = 0;
        rxBuffer[uartModule].state = UART_BUFFER_IDLE;
        if (appRxNotify[uartModule]) 
        {
          appRxNotify[uartModule]();
        }
      }
    }
  }
}
/**
 * @brief The UART 1 Handler
 * 
 */
void USART1_IRQHandler(void)
{
  UART_IRQHandler(UART1);
}
/**
 * @brief The UART 2 Handler
 * 
 */
void USART2_IRQHandler(void)
{
  UART_IRQHandler(UART2);
}
/**
 * @brief The UART 3 Handler
 * 
 */
void USART3_IRQHandler(void)
{
  UART_IRQHandler(UART3);
}
/**
 * @brief The UART 4 Handler
 * 
 */
void UART4_IRQHandler(void)
{
  UART_IRQHandler(UART4);
}
/**
 * @brief The UART 5 Handler
 * 
 */
void UART5_IRQHandler(void)
{
  UART_IRQHandler(UART5);
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
 * @param sysClk the clock of the system
 * @param uartModule the module number of the UART
 *                 UART1
 *                 UART2
 *                 UART3
 *                 UART4
 *                 UART5
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
extern Std_ReturnType Uart_Init(uint32_t baudRate, uint32_t stopBits, uint32_t parity, uint32_t flowControl, uint32_t sysClk, uint8_t uartModule) 
{
  volatile uart_t* Uart = (volatile uart_t*)Uart_Address[uartModule];
  Uart->BRR = ((uint32_t)((f64)sysClk / ((f64)baudRate * 16.0))) << 4;
  if (UART_NO_PARITY == parity) 
  {
    Uart->CR1 &= UART_M_CLR;
    Uart->CR1 &= UART_NO_PARITY;
  } 
  else 
  {
    Uart->CR1 |= UART_M_SET;
    Uart->CR1 |= UART_PCE_SET;
    Uart->CR1 &= UART_PS_CLR;
    Uart->CR1 |= parity;
  }
  Uart->CR2 &= UART_STOP_CLR;
  Uart->CR2 |= stopBits;
  Uart->CR3 &= UART_RTSE_CLR;
  Uart->CR3 |= flowControl;
  Uart->GTPR |= UART_NO_PRESCALER;
  rxBuffer[uartModule].state = UART_BUFFER_IDLE;
  txBuffer[uartModule].state = UART_BUFFER_IDLE;
  Uart->CR1 |= UART_UE_SET | UART_TXEIE_SET | UART_RXNEIE_SET | UART_TE_SET | UART_RE_SET;
  return E_OK;
}

/**
 * @brief Sends data through the UART
 *
 * @param data The data to send
 * @param length the length of the data in bytes
 * @param uartModule the module number of the UART
 *                 UART1
 *                 UART2
 *                 UART3
 *                 UART4
 *                 UART5
 * @return Std_ReturnType A Status
 *                  E_OK: If the driver is ready to send
 *                  E_NOT_OK: If the driver can't send data right now
 */
Std_ReturnType Uart_Send(uint8_t *data, uint16_t length, uint8_t uartModule) 
{
  Std_ReturnType error = E_NOT_OK;
  volatile uart_t* Uart = (volatile uart_t*)Uart_Address[uartModule];
  if (data && (length > 0) && txBuffer[uartModule].state == UART_BUFFER_IDLE) 
  {
    txBuffer[uartModule].state = UART_BUFFER_BUSY;
    txBuffer[uartModule].ptr = data;
    txBuffer[uartModule].pos = 0;
    txBuffer[uartModule].size = length;

    Uart->DR = txBuffer[uartModule].ptr[txBuffer[uartModule].pos++];
    Uart->CR1 |= UART_TXEIE_SET;
    error = E_OK;
  }
  return error;
}
/**
 * @brief Receives data through the UART
 *
 * @param data The buffer to receive data in
 * @param length the length of the data in bytes
 * @param uartModule the module number of the UART
 *                 UART1
 *                 UART2
 *                 UART3
 *                 UART4
 *                 UART5
 * @return Std_ReturnType A Status
 *                  E_OK: If the driver is ready to receive
 *                  E_NOT_OK: If the driver can't receive data right now
 */
Std_ReturnType Uart_Receive(uint8_t *data, uint16_t length, uint8_t uartModule) 
{
  Std_ReturnType error = E_NOT_OK;
  if (rxBuffer[uartModule].state == UART_BUFFER_IDLE) 
  {
    rxBuffer[uartModule].ptr = data;
    rxBuffer[uartModule].size = length;
    rxBuffer[uartModule].pos = 0;
    rxBuffer[uartModule].state = UART_BUFFER_BUSY;
    error = E_OK;
  }
  return error;
}
/**
 * @brief Sets the callback function that will be called when transmission is
 * completed
 *
 * @param func the callback function
 * @param uartModule the module number of the UART
 *                 UART1
 *                 UART2
 *                 UART3
 *                 UART4
 *                 UART5
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
Std_ReturnType Uart_SetTxCb(txCb_t func, uint8_t uartModule) 
{
  appTxNotify[uartModule] = func;
  return E_OK;
}
/**
 * @brief Sets the callback function that will be called when receive is
 * completed
 *
 * @param func the callback function
 * @param uartModule the module number of the UART
 *                 UART1
 *                 UART2
 *                 UART3
 *                 UART4
 *                 UART5
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
Std_ReturnType Uart_SetRxCb(rxCb_t func, uint8_t uartModule) 
{
  appRxNotify[uartModule] = func;
  return E_OK;
}

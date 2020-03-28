/**
 * @file Uart.h
 * @author Mark Attia (markjosephattia@gmail.com)
 * @brief This is the user interface for the UART driver
 * @version 0.1
 * @date 2020-03-26
 *
 * @copyright Copyright (c) 2020
 *
 */
#ifndef UART_H
#define UART_H

#define UART_ODD_PARITY 0x00000200
#define UART_EVEN_PARITY 0x00000000
#define UART_NO_PARITY 0xFFFFFBFF
#define UART_STOP_ONE_BIT 0x00000000
#define UART_STOP_TWO_BITS 0x00003000

#define UART_FLOW_CONTROL_EN 0x00000100
#define UART_FLOW_CONTROL_DIS 0x00000000

typedef void (*txCb_t)(void);
typedef void (*rxCb_t)(void);

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
extern Std_ReturnType Uart_Init(uint32_t baudRate, uint32_t stopBits,
                                uint32_t parity, uint32_t flowControl);
/**
 * @brief Sends data through the UART
 *
 * @param data The data to send
 * @param length the length of the data in bytes
 * @return Std_ReturnType A Status
 *                  E_OK: If the driver is ready to send
 *                  E_NOT_OK: If the driver can't send data right now
 */
extern Std_ReturnType Uart_Send(uint8_t *data, uint16_t length);
/**
 * @brief Receives data through the UART
 *
 * @param data The buffer to receive data in
 * @param length the length of the data in bytes
 * @return Std_ReturnType A Status
 *                  E_OK: If the driver is ready to receive
 *                  E_NOT_OK: If the driver can't receive data right now
 */
extern Std_ReturnType Uart_Receive(uint8_t *data, uint16_t length);
/**
 * @brief Sets the callback function that will be called when transmission is
 * completed
 *
 * @param func the callback function
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
extern Std_ReturnType Uart_SetTxCb(txCb_t func);
/**
 * @brief Sets the callback function that will be called when receive is
 * completed
 *
 * @param func the callback function
 * @return Std_ReturnType A Status
 *                  E_OK: If the function executed successfully
 *                  E_NOT_OK: If the did not execute successfully
 */
extern Std_ReturnType Uart_SetRxCb(rxCb_t func);

#endif

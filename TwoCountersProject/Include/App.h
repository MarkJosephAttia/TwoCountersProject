/**
 * @file App.h
 * @author Mariam Mohammed
 * @brief This is the user interface for the two counters application
 * @version 0.1
 * @date 2020-03-29
 * 
 * @copyright Copyright (c) 2020
 * 
 */
 #ifndef APP_H_
 #define APP_H_

/**
 * @brief This is the initialization for the two counter application
 *  @returns: A status
 *                 E_OK : if the function is executed correctly
 *                 E_NOT_OK : if the function is not executed correctly
 */
extern Std_ReturnType APP_init(void);
/**
 * @brief The free running task that comes every 1 milli second
 * 
 */
extern void APP_sendTask(void);
/**
 * @brief The receive function that will be called after each received frame
 *
 */
extern void APP_receiveFcn(void);

#endif
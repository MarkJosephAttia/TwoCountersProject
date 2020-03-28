/**
 * @brief The user interface for the Two Counters Application
 * 
 */
 #ifndef APP_H_
 #define APP_H_

/**
 * @brief This is the initialization for the two counter application
 * 
 */
extern void APP_init(void);
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
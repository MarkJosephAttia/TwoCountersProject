/**
 * @brief This is the user interface for the Systick driver
 * 
 */
/* Masks to be used in SYSTICK_CONF.h : SYSTICK_CLK_SRC */
#define SYSTICK_CLKSRC_AHB_DIV_8  0x00000000
#define SYSTICK_CLKSRC_AHB        0x00000004



typedef void (*SYSTICK_cbF)(void);

/**
 * @brief The initialization of the SysTick
 * 
 */
void SYSTICK_init(void);
/**
 * @brief Starts the Systick
 * 
 */
void SYSTICK_start(void);
/**
 * @brief Stops the timer
 * 
 */
void SYSTICK_stop(void);
/**
 * @brief Sets the timer for a specific time
 * 
 * @param time the time in milli seconds
 * @param AHB_clockHz the AHB clock in Kilo Hz
 */
void SYSTICK_setTime(u32 time, u32 AHB_clockHz);
/**
 * @brief Sets the callback function
 * 
 * @param cbF the function to set
 */
void SYSTICK_setCallbackFcn(SYSTICK_cbF cbF);
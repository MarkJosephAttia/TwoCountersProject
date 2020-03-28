/**
 * @brief This is a user interface for the Scheduler Driver
 * 
 */
 #ifndef SCHED1_H
 #define SCHED1_H 
typedef void (*taskRunnable)(void);

typedef struct
{
  taskRunnable runnable;
  u32 periodicTime;
  u32 priority;
} Task;

/**
 * @brief The initialization function
 * 
 */
void SCHED_init(void);
/**
 * @brief This function creates a task dynamically in the run time
 * 
 * @param appTask This is the application task desired to create
 */
void SCHED_createTask (Task *appTask);
/**
 * @brief Starts The running scheduel
 * 
 */
void SCHED_start(void);
#endif
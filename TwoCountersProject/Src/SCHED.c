/**
 * @file SCHED.c
 * @author Mariam Mohammed
 * @brief This is the implementation of the scheduler
 * @version 0.1
 * @date 2020-03-28
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Std_Types.h"

#include "RCC.h"
#include "SYSTICK.h"

#include "SCHED1.h"
#include "SCHED_CONF.h"

typedef struct
{
  Task *appTask;
  u32 RemainToExec;
  u32 periodicTimeTicks;
} SysTask;

static SysTask sysTasks[SCHED_MAX_TASK_NUM];
static volatile u8 OS_FLAG = 0;

/**
 * @brief The scheduler
 * 
 */
static void SCHED_schedule(void)
{
  u32 currentTask = 0;
  for (currentTask = 0; currentTask < SCHED_MAX_TASK_NUM; currentTask++)
  {
    if((sysTasks[currentTask].RemainToExec) == 0)
    {
      (sysTasks[currentTask].appTask)->runnable();
      (sysTasks[currentTask].RemainToExec) = sysTasks[currentTask].periodicTimeTicks;
    }
    sysTasks[currentTask].RemainToExec--;
  }
}

/**
 * @brief Sets the OS flag
 * 
 */
static void SCHED_setFlag(void)
{
  OS_FLAG = 1;
}


/**
 * @brief The initialization function
 * 
 */
void SCHED_init(void)
{
  RCC_configurePrescalers(RCC_AHB_PRESCALER, SCHED_AHB_PREVAL);
  SYSTICK_init();
  u32 prescalerVal = SCHED_AHB_PREVAL;
  u32 clockDiv = 1;
  if (prescalerVal != RCC_AHB_NDIVIDED)
  {
    if (prescalerVal <= RCC_AHB_DIV_16)
    {
      prescalerVal >>= 0x4;
      prescalerVal &= 0x7;
      prescalerVal++;
      clockDiv = (1 << prescalerVal);
    }
    else
    {
      prescalerVal >>= 0x4;
      prescalerVal &= 0x7;
      prescalerVal += 2;
      clockDiv = (1 << prescalerVal);
    }
  }
  u32 AHB_clock = SCHED_AHB_CLOCK / clockDiv;
  SYSTICK_setTime(SCHED_TICK_TIME_US, AHB_clock);
  SYSTICK_setCallbackFcn(SCHED_setFlag);
  SYSTICK_start();
}
/**
 * @brief This function creates a task dynamically in the run time
 * 
 * @param appTask This is the application task desired to create
 */
void SCHED_createTask (Task *appTask)
{
  if (appTask)
  {
    sysTasks[appTask->priority].appTask = appTask;
    sysTasks[appTask->priority].RemainToExec = 0;
    sysTasks[appTask->priority].periodicTimeTicks = (appTask->periodicTime)/SCHED_TICK_TIME_US;
  }
}
/**
 * @brief Starts The running scheduel
 * 
 */
void SCHED_start(void)
{
  while(1)
  {
    if(OS_FLAG)
    {
      OS_FLAG = 0;
      SCHED_schedule();
    }
  }
}

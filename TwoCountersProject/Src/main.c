/**
 * @file main.c
 * @author Mariam Mohammed
 * @brief Here is the implementation for the main function fo the application and also the tasks
 * @version 0.1
 * @date 2020-03-28
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Std_Types.h"
#include "SCHED1.h"

#include "HRcc.h"
#include "CLcd.h"
#include "App.h"
#include "Switch.h"

Task t1 = {APP_sendTask, 1000, 0};
Task t2 = {CLcd_Task, 1000, 1};
Task t3 = {Switch_Task, 1000, 2};

void main(void)
{
	HRcc_SystemClockInit();

	SCHED_createTask(&t1);
	SCHED_createTask(&t2);
	SCHED_createTask(&t3);

	APP_init();
	SCHED_init();
	SCHED_start();

}

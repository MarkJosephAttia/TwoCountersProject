/**
 * @file SCHED_CONF.h
 * @author Mariam Mohammed
 * @brief Those are the configurations for the Scheduler Driver
 * @version 0.1
 * @date 2020-03-29
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef SCHED_CONF_H
#define SCHED_CONF_H

#define SCHED_MAX_TASK_NUM      3

/* Masks for clock configuration */
#define SCHED_AHB_PREVAL RCC_AHB_NDIVIDED
#define SCHED_AHB_CLOCK 8000000
#define SCHED_TICK_TIME_US 1000

#endif
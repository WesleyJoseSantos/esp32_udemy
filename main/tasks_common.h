/**
 * @file tasks_common.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-08-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __TASKS_COMMON__H__
#define __TASKS_COMMON__H__

#define WIFI_APP_TASK_STACK_SIZE 4096
#define WIFI_APP_TASK_PRIORITY 5
#define WIFI_APP_TASK_CORE_ID 0

#define HTTP_SERVER_TASK_STACK_SIZE 8192
#define HTTP_SERVER_TASK_PRIORITY 4
#define HTTP_SERVER_TASK_CORE_ID 0

#define HTTP_SERVER_MONITOR_STACK_SIZE 8192
#define HTTP_SERVER_MONITOR_PRIORITY 4
#define HTTP_SERVER_MONITOR_CORE_ID 0

#endif  //!__TASKS_COMMON__H__
/**
 * @file http_server.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __HTTP_SERVER__H__
#define __HTTP_SERVER__H__

typedef enum http_server_message
{
    HTTP_SERVER_MSG_WIFI_CONNECT_INIT = 0,
    HTTP_SERVER_MSG_WIFI_CONNECT_SUCCESS,
    HTTP_SERVER_MSG_WIFI_CONNECT_FAIL,
    HTTP_SERVER_MSG_OTA_UPDATE_SUCCESSFUL,
    HTTP_SERVER_MSG_OTA_UPDATE_FAILED,
    HTTP_SERVER_MSG_OTA_UPDATE_INITIALIZED,
} http_server_message_t;

typedef struct http_server_queue_message
{
    http_server_message_t msg_id;
} http_server_queue_message_t;

/**
 * @brief Sends a message to the queue
 * 
 * @param msg_id message id
 * @return BaseType_t return pdTRUE if an item was successfully sent to tue queue, otherwise pdFALSE
 */
BaseType_t http_server_monitor_send_message(http_server_message_t msg_id);

/**
 * @brief Starts the http server
 * 
 */
void http_server_start(void);

/**
 * @brief Stop the http server
 * 
 */
void http_server_stop(void);

#endif  //!__HTTP_SERVER__H__




/**
 * @file wifi_app.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-08-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __WIFI_APP__H__
#define __WIFI_APP__H__

#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

// WiFi application settings
#define WIFI_AP_SSID                "ESP32_AP"          // AP name
#define WIFI_AP_PASS                "password"          // AP password
#define WIFI_AP_CHANNEL             1                   // AP channel
#define WIFI_AP_SSID_HIDDEN         0                   // AP visibility
#define WIFI_AP_MAX_CONN            5                   // AP max clients
#define WIFI_AP_BEACON_INTERVAL     100                 // AP beacon interval
#define WIFI_AP_IP_ADDRESS          "192.168.0.1"       // AP default IP address
#define WIFI_AP_GATEWAY             "192.168.0.1"       // AP default gateway
#define WIFI_AP_NETMASK             "255.255.255.0"     // AP netmask
#define WIFI_AP_BANDWITH            WIFI_BW_HT20        // AP bandwidth 20 MHz
#define WIFI_STA_POWER_SAVE         WIFI_PS_NONE        // AP power save mode not used
#define MAX_SSID_LENGTH             32                  // IEEE standard maximum
#define MAX_PASS_LENGTH             64                  // IEEE standard maximum
#define MAX_CONN_RETRIES            5                   // Retry number on disconnect

// netif object for the station and access point
extern esp_netif_t *esp_netif_sta;
extern esp_netif_t *esp_netif_ap;

/**
 * @brief Message IDs for the WiFi application task
 * @note Expand this based on your application requirements
 * 
 */
typedef enum wifi_app_message
{
    WIFI_APP_MSG_START_HTTP_SERVER = 0,
    WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER,
    WIFI_APP_MSG_STA_CONNECTED_GOT_IP,
} wifi_app_message_t;

/**
 * @brief Structure for the message queue
 * @note Expand this based on application requirements e.h and another type and parameter as required
 * 
 */
typedef struct wifi_app_queue_message
{
    wifi_app_message_t msg_id;
} wifi_app_queue_message_t;

/**
 * @brief Sends a message to the queu
 * 
 * @param msg_id message id from the wifi_app_message_t enum
 * @return BaseType_t pvTRUE if an intem was sucessfully added to the queue otherwise pdFALSE
 * @note Expand the parameter list based on your requirements
 */
BaseType_t wifi_app_send_message(wifi_app_message_t msg_id);

/**
 * @brief Starts the WiFi RTOS task
 * 
 */
void wifi_app_start(void);

#endif  //!__WIFI_APP__H__
/**
 * @file wifi_app.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-08-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "lwip/netdb.h"

#include "rgb_led.h"
#include "tasks_common.h"
#include "wifi_app.h"

static const *TAG = "wifi_app";

static QueueHandle_t wifi_app_queue_handle;

esp_netif_t *esp_netif_sta = NULL;
esp_netif_t *esp_netif_ap = NULL;

static void wifi_app_task(void *pvParameter)
{
    wifi_app_queue_message_t msg;

    wifi_app_event_handler_init();
    wifi_app_default_wifi_init();
    wifi_app_soft_ap_config();

    ESP_ERROR_CHECK(esp_wifi_start());

    wifi_app_send_message(WIFI_APP_MSG_START_HTTP_SERVER);

    for (;;)
    {
        if(xQueueReceive(wifi_app_queue_handle, &msg, portMAX_DELAY))
        {
            switch (msg.msg_id)
            {
            case WIFI_APP_MSG_START_HTTP_SERVER:
                ESP_LOGI(TAG, "WIFI_APP_MSG_START_HTTP_SERVER");
                // http_server_start();
                rgb_led_http_server_started();
                break;
            case WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER:
                ESP_LOGI(TAG, "WIFI_APP_MSG_START_HTTP_SERVER");
                break;
            case WIFI_APP_MSG_STA_CONNECTED_GOT_IP:
                ESP_LOGI(TAG, "WIFI_APP_MSG_STA_CONNECTED_GOT_IP");
                rgb_led_wifi_connected();
                break;
            default:
                break;
            }
        }
    }
    
}

BaseType_t wifi_app_send_message(wifi_app_message_t msg_id)
{
    wifi_app_queue_message_t msg;
    msg.msg_id = msg_id;
    return xQueueSend(wifi_app_queue_handle, &msg, portMAX_DELAY);
}

void wifi_app_start(void)
{
    ESP_LOGI(TAG, "Starting wifi application");

    rgb_led_wifi_app_started();

    esp_log_level_set("wifi", ESP_LOG_NONE);

    wifi_app_queue_handle = xQueueCreate(3, sizeof(wifi_app_queue_message_t));

    xTaskCreatePinnedToCore(&wifi_app_task, "wifi_app_task", WIFI_APP_TASK_STACK_SIZE, NULL, WIFI_APP_TASK_PRIORITY, WIFI_APP_TASK_CORE_ID);
}

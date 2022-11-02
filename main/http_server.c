/**
 * @file http_server.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "esp_ota_ops.h"
#include "sys/param.h"

#include "esp_http_server.h"
#include "esp_log.h"

#include "http_server.h"
#include "tasks_common.h"
#include "wifi_app.h"
#include "esp_timer.h"
#include "esp_system.h"

static const char *TAG = "http_server";

static int fw_update_status = OTA_UPDATE_PENDING;

static httpd_handle_t http_server_handle = NULL;

static TaskHandle_t task_http_server_monitor = NULL;

static QueueHandle_t http_server_monitor_queue_handle;

const esp_timer_create_args_t fw_update_reset_args = {
    .callback = &http_server_fw_update_reset_callback,
    .arg = NULL,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "fw_update_reset"
};
esp_timer_handle_t fw_update_reset;

extern const uint8_t jquery_3_3_1_js_start[] asm("_binary_jquery_3_3_1_js_start");
extern const uint8_t jquery_3_3_1_js_end[] asm("_binary_jquery_3_3_1_js_end");

extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

extern const uint8_t app_css_start[] asm("_binary_app_css_start");
extern const uint8_t app_css_end[] asm("_binary_app_css_end");

extern const uint8_t app_js_start[] asm("_binary_app_js_start");
extern const uint8_t app_js_end[] asm("_binary_app_js_end");

extern const uint8_t favicon_ico_start[] asm("_binary_favicon_ico_start");
extern const uint8_t favicon_ico_end[] asm("_binary_favicon_ico_end");

static void http_server_fw_update_firmware_reset_timer(void)
{
    if(fw_update_status == OTA_UPDATE_SUCCESSFUL)
    {
        ESP_LOGI(TAG, "FW updated successful starting FW update reset timer");

        ESP_ERROR_CHECK(esp_timer_create(&fw_update_reset_args, &fw_update_reset));
        ESP_ERROR_CHECK(esp_timer_start_once(fw_update_reset, 800000));
    }
    else
    {
        ESP_LOGI(TAG, "FW update unsuccessful");
    }
}

static void http_server_monitor(void *pvParameters)
{
    http_server_queue_message_t msg;

    for (;;)
    {
        if(xQueueReceive(http_server_monitor_queue_handle, &msg, portMAX_DELAY))
        {
            switch (msg.id)
            {
            case HTTP_SERVER_MSG_WIFI_CONNECT_INIT:
                ESP_LOGI(TAG, "HTTP_SERVER_MSG_WIFI_CONNECT_INIT");
                /* code */
                break;
            case HTTP_SERVER_MSG_WIFI_CONNECT_SUCCESS:
                ESP_LOGI(TAG, "HTTP_SERVER_MSG_WIFI_CONNECT_SUCCESS");
                /* code */
                break;
            case HTTP_SERVER_MSG_WIFI_CONNECT_FAIL:
                ESP_LOGI(TAG, "HTTP_SERVER_MSG_WIFI_CONNECT_FAIL");
                /* code */
                break;
            case HTTP_SERVER_MSG_OTA_UPDATE_SUCCESSFUL:
                ESP_LOGI(TAG, "HTTP_SERVER_MSG_OTA_UPDATE_SUCCESSFUL");
                fw_update_status = OTA_UPDATE_SUCCESSFUL;
                http_server_fw_update_firmware_reset_timer();
                /* code */
                break;
            case HTTP_SERVER_MSG_OTA_UPDATE_FAILED:
                ESP_LOGI(TAG, "HTTP_SERVER_MSG_OTA_UPDATE_FAILED");
                fw_update_status = OTA_UPDATE_FAIL;
                /* code */
                break;
            default:
                break;
            }
        }
    }    
}

static httpd_handle_t http_server_configure(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    xTaskCreatePinnedToCore(&http_server_monitor, "http_server_monitor", HTTP_SERVER_MONITOR_STACK_SIZE, NULL, HTTP_SERVER_MONITOR_PRIORITY, &task_http_server_monitor, HTTP_SERVER_MONITOR_CORE_ID);

    http_server_monitor_queue_handle = xQueueCreate(3, sizeof(http_server_queue_message_t));

    config.core_id = HTTP_SERVER_TASK_CORE_ID;
    config.task_priority = HTTP_SERVER_TASK_PRIORITY;
    config.stack_size = HTTP_SERVER_TASK_STACK_SIZE;
    config.max_uri_handlers = 20;
    config.recv_wait_timeout = 10;
    config.send_wait_timeout = 10;

    ESP_LOGI(TAG, "http_server_configure: Starting server on port: '%d' with task priority '%d'", config.server_port, config.task_priority);

    if(httpd_start(&http_server_handle, &config) == ESP_OK)
    {
        ESP_LOGI(TAG, "http_server_configure: Registering URI handlers");

        httpd_uri_t jquery_js = {
            .uri = "/jquery-3.3.1.min.js",
            .method = HTTP_GET,
            .handler = http_server_jquery_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(http_server_handle, &jquery_js);

        httpd_uri_t index_html = {
            .uri = "/index.html",
            .method = HTTP_GET,
            .handler = http_server_index_html_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(http_server_handle, &index_html);

        httpd_uri_t app_css = {
            .uri = "/app.css",
            .method = HTTP_GET,
            .handler = http_server_app_css_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(http_server_handle, &app_css);

        httpd_uri_t app_js = {
            .uri = "/app.js",
            .method = HTTP_GET,
            .handler = http_server_app_js_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(http_server_handle, &app_js);

        httpd_uri_t favicon_ico = {
            .uri = "/favicon.ico",
            .method = HTTP_GET,
            .handler = http_server_favicon_ico_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(http_server_handle, &favicon_ico);

        httpd_uri_t ota_update = {
            .uri = "/ota_update",
            .method = HTTP_POST,
            .handler = http_server_ota_update_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(http_server_handle, &ota_update);

        httpd_uri_t ota_status = {
            .uri = "/ota_status",
            .method = HTTP_POST,
            .handler = http_server_ota_status_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(http_server_handle, &ota_status);

        return http_server_handle;
    }

    return NULL;
}

void http_server_start(void)
{
    if(http_server_handle == NULL)
    {
        http_server_handle = http_server_configure();
    }
}

void http_server_stop(void)
{
    if(http_server_handle)
    {
        httpd_stop(http_server_handle);
        ESP_LOGI(TAG, "http_server_stop: stopping HTTP server");
        http_server_handle = NULL;
    }
    if(task_http_server_monitor)
    {
        vTaskDelete(task_http_server_monitor);
        ESP_LOGI(TAG, "http_server_stop: stopping HTTP server monitor");
        task_http_server_monitor = NULL;
    }
}

BaseType_t http_server_monitor_send_message(http_server_message_t msg_id)
{
    http_server_queue_message_t msg;
    msg.id = msg_id;
    return xQueueSend(http_server_monitor_queue_handle, &msg, portMAX_DELAY);
}

void http_server_fw_update_reset_callback(void *arg)
{
    ESP_LOGI(TAG, "Timer timed-out, restarting the device");
    esp_restart();
}

static esp_err_t http_server_jquery_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char*) jquery_3_3_1_js_start, jquery_3_3_1_js_start - jquery_3_3_1_js_end);
    return ESP_OK;
}

static esp_err_t http_server_index_html_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char*) index_html_start, index_html_start - index_html_end);
    return ESP_OK;
}

static esp_err_t http_server_app_css_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char*) app_css_start, app_css_start - app_css_end);
    return ESP_OK;
}

static esp_err_t http_server_app_js_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char*) app_js_start, app_js_start - app_js_end);
    return ESP_OK;
}

static esp_err_t http_server_favicon_ico_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req, (const char*) favicon_ico_start, favicon_ico_start - favicon_ico_end);
    return ESP_OK;
}

static esp_err_t http_server_ota_update_handler(httpd_req_t *req)
{
    esp_ota_handle_t ota_handle;
    char ota_buf[1024];
    int content_len = req->content_len;
    int content_rec = 0;
    int recv_len;
    bool is_req_body_started = false;
    bool flash_successful = false;

    const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);

    do
    {
        if(recv_len = httpd_req_recv(req, ota_buf, MIN(content_len, sizeof(ota_buf))) < 0)
        {
            if(recv_len == HTTPD_SOCK_ERR_TIMEOUT)
            {
                ESP_LOGI(TAG, "http_server_ota_update_handler: Socket Timeout");
                continue;   ///> Retry receiveing if timeout occourred
            }
            ESP_LOGI(TAG, "http_server_ota_update_handler: OTA other error: %d", recv_len);
            return ESP_FAIL;
        }
        printf("http_server_ota_update_handler: OTA RX: %d, of %d\r", content_rec, content_len);

        if(is_req_body_started == false)
        {
            is_req_body_started = true;
            char *body_start_p = strstr(ota_buf, "\r\n\r\n") + strlen("\r\n\r\n");
            int body_part_len = recv_len - (body_start_p - ota_buf);

            printf("http_server_ota_update_handler: OTA file size: %d\r\n", content_len);

            esp_err_t err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ota_handle);
            if(err != ESP_OK)
            {
                printf("http_server_ota_update_handler: Error with OTA begin, cancelling OTA\r\n");
                return ESP_FAIL;
            }
            else
            {
                printf("http_server_ota_update_handler: Writing to partition subtype %d at offser 0x%x\r\n", update_partition->subtype, update_partition->address);                
            }

            //Write first part of the data
            esp_ota_write(ota_handle, body_start_p, body_part_len);
            content_rec += body_part_len;
        }
        else
        {
            // Write OTA data
            esp_ota_write(ota_handle, ota_buf, recv_len);
            content_rec += recv_len;
        }

    } while (recv_len > 0 && content_rec < content_len);

    if(esp_ota_end(ota_handle) == ESP_OK)
    {
        // Lets update the partition
        if(esp_ota_set_boot_partition(update_partition) == ESP_OK)
        {
            const esp_partition_t *boot_partition = esp_ota_get_boot_partition();
            ESP_LOGI(TAG, "http_server_ota_update_handler: Next boot partition subtype %d at offser 0x%x\r\n", boot_partition->subtype, boot_partition->address);
            flash_successful = true;
        }
        else
        {
            ESP_LOGI(TAG, "http_server_ota_update_handler: FLASHED ERROR!!!");
        }
    }
    else
    {
        ESP_LOGI(TAG, "http_server_ota_update_handler: esp_ota_end ERROR!!!");
    }

    if(flash_successful)
    {
        http_server_monitor_send_message(HTTP_SERVER_MSG_OTA_UPDATE_SUCCESSFUL);
    }
    else
    {
        http_server_monitor_send_message(HTTP_SERVER_MSG_OTA_UPDATE_FAILED);
    }

    return ESP_OK;
}

static esp_err_t http_server_ota_status_handler(httpd_req_t *req)
{
    char ota_json[100];

    ESP_LOGI(TAG, "ota_status requested");
    sprintf(ota_json, "{\"ota_update_status\":%d,\"compile_time\":\"%s\",\"compile_date\":\"%s\"}", fw_update_status, __TIME__, __DATE__);
    httpd_resp_send(req, ota_json, strlen(ota_json));

    return ESP_OK;
}
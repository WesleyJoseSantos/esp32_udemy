/**
 * @file rgb_led.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-08-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __RGB_LED__H__
#define __RGB_LED__H__

// RGB LED GPIOs
#define RGB_LED_RED_GPIO    21
#define RGB_LED_GREEN_GPIO  22
#define RGB_LED_BLUE_GPIO   23

// RGB LED color mix channels
#define RGB_LED_CHANNEL_NUM 3

// RGB LED configuration
typedef struct
{
    int channel;
    int gpio;
    int mode;
    int timer_index;
} ledc_info_t;

ledc_info_t ledc_ch[RGB_LED_CHANNEL_NUM];

/**
 * @brief Color to indicate WiFi application has started
 */
void rgb_led_wifi_app_started(void);

/**
 * @brief Color to indicate HTTP server has started
 */
void rgb_led_http_server_started(void);

/**
 * @brief Color to inticate that the ESP32 is connected to an acess point
 */
void rgb_led_wifi_connected(void);

#endif  //!__RGB_LED__H__
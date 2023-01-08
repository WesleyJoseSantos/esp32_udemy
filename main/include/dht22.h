/**
 * @file DHT22.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef DHT22_H_  
#define DHT22_H_

#define DHT_OK 0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR -2

#define DHT_GPIO			25

/**
 * Starts DHT22 sensor task
 */
void dht22_task_start(void);

// == function prototypes =======================================

void 	dht22_set_gpio(int gpio);
void 	dht22_error_handler(int response);
int 	dht22_read();
float 	dht22_get_humidity();
float 	dht22_get_temperature();
int 	dht22_get_signal_level( int usTimeOut, bool state );

#endif

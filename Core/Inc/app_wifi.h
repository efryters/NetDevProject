/*
 * app_wifi.h
 *
 *  Created on: Apr. 3, 2020
 *      Author: efryters
 */

#ifndef INC_APP_WIFI_H_
#define INC_APP_WIFI_H_

#define TERMINAL_USE
#if defined (TERMINAL_USE)
#define TERMOUT(...)  printf(__VA_ARGS__)
#else
#define TERMOUT(...)
#endif

typedef enum {
	WIFI_RESET,
	WIFI_INIT,
	WIFI_CONNECT_SSID,
	WIFI_GET_IP,
	WIFI_CONNECT_SERVER,
	WIFI_ERROR = 100,
	WIFI_HALT


} WifiState_t;

/* Exported Functions */
void AppMain();


#endif /* INC_APP_WIFI_H_ */

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
	WIFI_INIT_PLT,
	WIFI_INIT,
	WIFI_CONNECT_SSID,
	WIFI_GET_IP,
	WIFI_CONNECT_SERVER,
	WIFI_WAIT_FOR_PUNCH,
	WIFI_INPUT_PUNCH_DATA,
	WIFI_SEND_MSG,
	WIFI_ERROR = 100,
	WIFI_HALT


} WifiState_t;


typedef struct {

	uint8_t MAC_Addr[6];
	uint8_t IP_Addr[4];
	uint16_t trials;
	uint8_t socket;

	uint8_t RemoteIP[4];
	uint16_t RemotePort;

	const char* Ssid;
	const char* passwd;

} WifiInfo_s;

typedef struct {
	char employeeID[5];
	char employeePin[5];
	char* sendOut;
} PunchInfo_s;


/* Exported Functions */
void AppMain();


#endif /* INC_APP_WIFI_H_ */

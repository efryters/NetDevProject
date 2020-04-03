/*
 * app_wifi.c
 *
 *  Created on: Apr. 3, 2020
 *      Author: efryters
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "stdbool.h"
#include "KeyPad.h"
#include "wifi.h"
#include "app_wifi.h"
#include "app_wifi_conf.h"


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

/* Private Variables */
extern I2C_HandleTypeDef hi2c1;
uint8_t RemoteIP[] = { 172, 16, 1, 151 };
#define RemotePORT	27015

volatile bool punch = true;


/* Private function prototypes */
int init_wifi_hw(WifiInfo_s *wifiInfo);
int wifi_connect_ssid();
int wifi_get_ip(WifiInfo_s *wifiInfo);
int wifi_connect_server();


/*
 * State machine for application
 */
void AppMain()
{

	char test[] = "Blue button!";
	uint16_t Datalen;
	WifiInfo_s wifiInfo = {0};
	uint8_t g8WifiState = WIFI_RESET;
	lcd_device lcd = { &hi2c1, 0x20 << 1, false };


	for (int i = 0; i < 4; i++)
		wifiInfo.RemoteIP[i] = RemoteIP[i];

	wifiInfo.RemotePort = RemotePORT;

	wifiInfo.Ssid = SSID;
	wifiInfo.passwd = PASSWORD;

	wifiInfo.trials = CONNECTION_TRIAL_MAX;

	while(1){


		switch (g8WifiState) {

		// Entry point for application. Occurs once upon reset
		case WIFI_RESET:

			g8WifiState = WIFI_INIT_PLT;
			break;

		// Initialize non-wifi components
		case WIFI_INIT_PLT:

			lcd_init(&lcd);
			KeyPad_Init();
			lcd_send_command(LCD_CLEARDISPLAY, false);

			g8WifiState = WIFI_INIT;
			break;

		// Initialize the wifi hardware
		case WIFI_INIT:

			lcd_write_string("Init HW");
			if (init_wifi_hw(&wifiInfo) != WIFI_STATUS_OK) {
				g8WifiState = WIFI_HALT;
			}

			g8WifiState = WIFI_CONNECT_SSID;
			break;

		// Connect to  the SSID
		case WIFI_CONNECT_SSID:

			lcd_write_string("Connect to SSID");
			if (wifi_connect_ssid() != WIFI_STATUS_OK) {
				g8WifiState = WIFI_HALT;
			}

			g8WifiState = WIFI_GET_IP;
			break;

		// Get IP
		case WIFI_GET_IP:

			lcd_write_string("Getting IP");
			if (wifi_get_ip(&wifiInfo) != WIFI_STATUS_OK) {
				g8WifiState = WIFI_HALT;
			}

			g8WifiState = WIFI_CONNECT_SERVER;
			break;

		// Connect to the server
		case WIFI_CONNECT_SERVER:

			lcd_write_string("Connect to server");
			if (wifi_connect_server(&wifiInfo) != WIFI_STATUS_OK)
			{
				g8WifiState = WIFI_HALT;
			}

			g8WifiState = WIFI_WAIT_FOR_PUNCH;
			break;

		case WIFI_WAIT_FOR_PUNCH:

			// Setup interrupt for the blue button
			BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);
			lcd_write_string("Press blue btn to punch.");
			while(punch) {}
			// Turn it off now cuz we don't want this happening outside this case.
			BSP_PB_DeInit(BUTTON_USER); HAL_Delay(1000); punch = true;

			g8WifiState = WIFI_INPUT_PUNCH_DATA;
			break;

		case WIFI_INPUT_PUNCH_DATA:

			int status = 0;
			/// TODO: get this part working
			lcd_write_string("Punch data here...");
			status = WIFI_SendData(wifiInfo.socket, test, sizeof(test) - 1, &Datalen, WIFI_WRITE_TIMEOUT);
			WIFI_CloseClientConnection(wifiInfo.socket);
			while(1)
				HAL_Delay(5000);


			g8WifiState = WIFI_SEND_MSG;

			break;


		case WIFI_SEND_MSG:



			break;

		// Error state and recover
		case WIFI_ERROR:

			break;

		// Bad problem, reset device to recover.
		case WIFI_HALT:

			while(true) { lcd_write_string("Hard fault. Reset device."); HAL_Delay(2000);  }

			break;

		}

	}

}


int init_wifi_hw(WifiInfo_s *wifiInfo)
{
	int status = 0;

	status = WIFI_Init();
	if (status != WIFI_STATUS_OK)
	{
		return status;
	}

	WIFI_GetMAC_Address(wifiInfo->MAC_Addr);
	return WIFI_STATUS_OK;
}

int wifi_connect_ssid()
{
	int status = 0;

	status = WIFI_Connect(SSID, PASSWORD, WIFI_ECN_WPA2_PSK);
	if (status != WIFI_STATUS_OK)
	{
		return status;
	}

	return WIFI_STATUS_OK;
}

int wifi_get_ip(WifiInfo_s *wifiInfo)
{
	int status = 0;

	status = WIFI_GetIP_Address(wifiInfo->IP_Addr);
	if (status != WIFI_STATUS_OK)
	{
		return status;
	}

	return WIFI_STATUS_OK;
}

int wifi_connect_server(WifiInfo_s *wifiInfo)
{
	int status = 0;
	int trials = wifiInfo->trials;

	while (trials--)
	{
		status = WIFI_OpenClientConnection(0, WIFI_TCP_PROTOCOL, "TCP_CLIENT", wifiInfo->RemoteIP, wifiInfo->RemotePort, 0);
		if (status != WIFI_STATUS_OK)
		{
			return status;
		}

		wifiInfo->socket = 0;
	}

		return WIFI_STATUS_OK;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	switch (GPIO_Pin) {
	case (GPIO_PIN_1): {
		SPI_WIFI_ISR();
		break;
	}

	case (GPIO_PIN_13): {
		punch = false;
		//lcd_write_string("Interrupted.");
	}
	default: {
		break;
	}
	}
}

///*
//void do_wifi()
//{
//	/*Initialize  WIFI module */
//	if (WIFI_Init() == WIFI_STATUS_OK) {
//		TERMOUT("> WIFI Module Initialized.\r\n");
//		if (WIFI_GetMAC_Address(MAC_Addr) == WIFI_STATUS_OK) {
//			TERMOUT("> es-wifi module MAC Address : %X:%X:%X:%X:%X:%X\r\n",
//					MAC_Addr[0], MAC_Addr[1], MAC_Addr[2], MAC_Addr[3],
//					MAC_Addr[4], MAC_Addr[5]);
//		} else {
//			TERMOUT("> ERROR : CANNOT get MAC address\r\n");
//			BSP_LED_On(LED2);
//		}
//		lcd_send_command(LCD_CLEARDISPLAY, false);
//		lcd_write_string("Connecting wifi.");
//		if (WIFI_Connect(SSID, PASSWORD, WIFI_ECN_WPA2_PSK) == WIFI_STATUS_OK) {
//			TERMOUT("> es-wifi module connected \r\n");
//
//			if (WIFI_GetIP_Address(IP_Addr) == WIFI_STATUS_OK) {
//				TERMOUT("> es-wifi module got IP Address : %d.%d.%d.%d\r\n",
//						IP_Addr[0], IP_Addr[1], IP_Addr[2], IP_Addr[3]);
//
//				TERMOUT("> Trying to connect to Server: %d.%d.%d.%d:%d ...\r\n",
//						RemoteIP[0], RemoteIP[1], RemoteIP[2], RemoteIP[3],
//						RemotePORT);
//
//				lcd_send_command(LCD_CLEARDISPLAY, false);
//				lcd_write_string("Connected! Trying server...");
//
//				while (Trials--) {
//					if (WIFI_OpenClientConnection(0, WIFI_TCP_PROTOCOL,
//							"TCP_CLIENT", RemoteIP, RemotePORT, 0)
//							== WIFI_STATUS_OK) {
//						TERMOUT("> TCP Connection opened successfully.\r\n");
//						lcd_send_command(LCD_CLEARDISPLAY, false);
//						lcd_write_string("Connected to the server!");
//						Socket = 0;
//						break;
//					}
//				}
//				if (Socket == -1) {
//					TERMOUT("> ERROR : Cannot open Connection\r\n");
//					lcd_send_command(LCD_CLEARDISPLAY, false);
//					lcd_write_string("Can't connect to server!");
//					BSP_LED_On(LED2);
//				}
//			} else {
//				TERMOUT("> ERROR : es-wifi module CANNOT get IP address\r\n");
//				BSP_LED_On(LED2);
//			}
//		} else {
//			TERMOUT("> ERROR : es-wifi module NOT connected\r\n");
//			BSP_LED_On(LED2);
//		}
//	} else {
//		TERMOUT("> ERROR : WIFI Module cannot be initialized.\r\n");
//		BSP_LED_On(LED2);
//	}
//
//}


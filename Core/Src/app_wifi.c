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


/* Private Variables */
uint8_t g8WifiState = WIFI_RESET;

/* Private function prototypes */
int init_wifi_hw();

/*
 * State machine for application
 */
void AppMain()
{

	while(1){


		switch (g8WifiState) {

		// Entry point for application. Occurs once upon reset
		case WIFI_RESET:

			g8WifiState = WIFI_INIT;
			break;

		// Initialize the hardware
		case WIFI_INIT:

			if (init_wifi_hw() != WIFI_STATUS_OK) {
				g8WifiState = WIFI_HALT
			}
			break;

		// Connect to  the SSID
		case WIFI_CONNECT_SSID:


			break;

		// Get IP
		case WIFI_GET_IP:

			break;

		// Connect to the server
		case WIFI_CONNECT_SERVER:

			break;

		// Error state and recover
		case WIFI_ERROR:

			break;

		// Bad problem, reset device to recover.
		case WIFI_HALT:
			lcd_write_string("Hard fault. Reset device.");
			while(true) {  }

			break;

		}



	}

}


int init_wifi_hw()
{
	int status = 0;

	status = WIFI_Init();
	if (status != WIFI_STATUS_OK)
	{
		return status;
	}

	return WIFI_STATUS_OK;
}



void do_wifi()
{
	/*Initialize  WIFI module */
	if (WIFI_Init() == WIFI_STATUS_OK) {
		TERMOUT("> WIFI Module Initialized.\r\n");
		if (WIFI_GetMAC_Address(MAC_Addr) == WIFI_STATUS_OK) {
			TERMOUT("> es-wifi module MAC Address : %X:%X:%X:%X:%X:%X\r\n",
					MAC_Addr[0], MAC_Addr[1], MAC_Addr[2], MAC_Addr[3],
					MAC_Addr[4], MAC_Addr[5]);
		} else {
			TERMOUT("> ERROR : CANNOT get MAC address\r\n");
			BSP_LED_On(LED2);
		}
		lcd_send_command(LCD_CLEARDISPLAY, false);
		lcd_write_string("Connecting wifi.");
		if (WIFI_Connect(SSID, PASSWORD, WIFI_ECN_WPA2_PSK) == WIFI_STATUS_OK) {
			TERMOUT("> es-wifi module connected \r\n");

			if (WIFI_GetIP_Address(IP_Addr) == WIFI_STATUS_OK) {
				TERMOUT("> es-wifi module got IP Address : %d.%d.%d.%d\r\n",
						IP_Addr[0], IP_Addr[1], IP_Addr[2], IP_Addr[3]);

				TERMOUT("> Trying to connect to Server: %d.%d.%d.%d:%d ...\r\n",
						RemoteIP[0], RemoteIP[1], RemoteIP[2], RemoteIP[3],
						RemotePORT);

				lcd_send_command(LCD_CLEARDISPLAY, false);
				lcd_write_string("Connected! Trying server...");

				while (Trials--) {
					if (WIFI_OpenClientConnection(0, WIFI_TCP_PROTOCOL,
							"TCP_CLIENT", RemoteIP, RemotePORT, 0)
							== WIFI_STATUS_OK) {
						TERMOUT("> TCP Connection opened successfully.\r\n");
						lcd_send_command(LCD_CLEARDISPLAY, false);
						lcd_write_string("Connected to the server!");
						Socket = 0;
						break;
					}
				}
				if (Socket == -1) {
					TERMOUT("> ERROR : Cannot open Connection\r\n");
					lcd_send_command(LCD_CLEARDISPLAY, false);
					lcd_write_string("Can't connect to server!");
					BSP_LED_On(LED2);
				}
			} else {
				TERMOUT("> ERROR : es-wifi module CANNOT get IP address\r\n");
				BSP_LED_On(LED2);
			}
		} else {
			TERMOUT("> ERROR : es-wifi module NOT connected\r\n");
			BSP_LED_On(LED2);
		}
	} else {
		TERMOUT("> ERROR : WIFI Module cannot be initialized.\r\n");
		BSP_LED_On(LED2);
	}

}

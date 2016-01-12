/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "esp_common.h"
#include "uart.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#define DEMO_AP_SSID "TP-LINK_AE9170"
#define DEMO_AP_PASSWORD "2014HGKJ"

uint8 buffer[1400]="0123456789";

void task2(void *pvParameters)
{
	struct station_config * config = (struct station_config *)zalloc(sizeof(struct station_config));
	LOCAL int32 sock_fd;
	struct sockaddr_in server_addr;
	//struct hostent *host;
	
	
	//printf("Hello, welcome to task2!\r\n");
	wifi_set_opmode(STATION_MODE);
	sprintf(config->ssid, DEMO_AP_SSID);
	sprintf(config->password, DEMO_AP_PASSWORD);
	wifi_station_set_config(config);
	free(config);
	wifi_station_connect();
	
	vTaskDelay(1000);
	printf("start udp ... \n");
	//host=(struct hostent *)gethostbyname("192.168.0.247");
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr =  inet_addr("192.168.1.102");//;*((struct in_addr *)host->h_addr);
	server_addr.sin_port = htons(8080);
	server_addr.sin_len = sizeof(server_addr);
	do{
		sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock_fd == -1) 
		{
			printf("ESP8266 UDP task > failed to create sock!\n");
			vTaskDelay(1000/portTICK_RATE_MS);
		}
	}while(sock_fd == -1);
	printf("ESP8266 UDP task > socket OK!\n");
		
	while (1) 
	{		
		//printf("Hello, welcome to task2!\r\n");
		sendto(sock_fd, buffer, sizeof(buffer),0,(struct sockaddr *)&server_addr,sizeof(struct sockaddr));
		//vTaskDelay(50);
	}
	vTaskDelete(NULL);
}
/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
	uart_init_new();
    printf("SDK version:%s\n", system_get_sdk_version());
	
	xTaskCreate(task2, "tsk2", 2048, NULL, 2, NULL);
}


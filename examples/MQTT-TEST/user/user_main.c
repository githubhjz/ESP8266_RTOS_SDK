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
#include "MQTTClient.h"

#define DEMO_AP_SSID "TP-LINK_AE9170"
#define DEMO_AP_PASSWORD "2014HGKJ"

uint8 buffer[256]="0123456789";


/*void scan_done(void *arg, STATUS status)
{
	uint8 ssid[33];
	char temp[128];
	if (status == OK)
	{
		struct bss_info *bss_link = (struct bss_info *)arg;
		bss_link = bss_link->next.stqe_next;//ignore the first one , it's invalid.
		while (bss_link != NULL)
		{
			memset(ssid, 0, 33);
			if (strlen(bss_link->ssid) <= 32)
				memcpy(ssid, bss_link->ssid, strlen(bss_link->ssid));
			else
				memcpy(ssid, bss_link->ssid, 32);
			printf("(%d,\"%s\",%d,\""MACSTR"\",%d)\r\n",
			bss_link->authmode, ssid, bss_link->rssi,
			MAC2STR(bss_link->bssid),bss_link->channel);
			bss_link = bss_link->next.stqe_next;
		}
	}
	else
	{
		printf("scan fail !!!\r\n");
	}
}*/
struct opts_struct//client opts
{
    char* clientid;
    int nodelimiter;
    char* delimiter;
    enum QoS qos;
    char* username;
    char* password;
    char* host;
    int port;
    int showtopics;
} opts =
{
    (char*)"IoT GateWay ", 0, (char*)"\n", QOS2, NULL, NULL, (char*)"192.168.1.102", 1883, 0
};

Network n;//server net
Client c;//client


void mqtt_task(void *pvParameters)
{
	//wifi
	struct station_config * config = (struct station_config *)zalloc(sizeof(struct station_config));
	struct scan_config config2;
	//mqtt
	unsigned char *sendbuf,*readbuf;
	int rc = 0;
	
	//connet wifi
	wifi_set_opmode(STATION_MODE);
	sprintf(config->ssid, DEMO_AP_SSID);
	sprintf(config->password, DEMO_AP_PASSWORD);
	wifi_station_set_config(config);
	free(config);
	wifi_station_connect();
	vTaskDelay(200);
	printf("wifi connect\n"); 
	//mqtt
	sendbuf = (unsigned char *)pvPortMalloc(128);
	readbuf = (unsigned char *)pvPortMalloc(128);
	
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
		
	NewNetwork(&n);
    while(ConnectNetwork(&n, opts.host, opts.port));//rt_thread_delay(100);;
    MQTTClient(&c, &n, 10, sendbuf, 256, readbuf, 256);
	
	data.willFlag = 0;
    data.MQTTVersion = 3;
    data.clientID.cstring = opts.clientid;
    data.username.cstring = opts.username;
    data.password.cstring = opts.password;
    data.keepAliveInterval = 10;
    data.cleansession = 1;
	
	rc = MQTTConnect(&c, &data);
	printf("Connecting to %s %d err :%d\n", opts.host, opts.port,rc); 
	
	while(1)
	{
		
		vTaskDelay(1000/portTICK_RATE_MS);
	}
	
}
	
void task2(void *pvParameters)
{
	struct station_config * config = (struct station_config *)zalloc(sizeof(struct station_config));
	struct scan_config config2;
	LOCAL int32 sock_fd;
	struct sockaddr_in server_addr;
	//struct hostent *host;
	
	
	/*memset(&config2, 0, sizeof(config2));
	config2.ssid = DEMO_AP_SSID;
	wifi_station_scan(&config2,scan_done);*/
	
	//printf("Hello, welcome to task2!\r\n");
	wifi_set_opmode(STATION_MODE);
	sprintf(config->ssid, DEMO_AP_SSID);
	sprintf(config->password, DEMO_AP_PASSWORD);
	wifi_station_set_config(config);
	free(config);
	wifi_station_connect();
	
	
	vTaskDelay(1000);
	printf("start TCP ... \n");
	//host=(struct hostent *)gethostbyname("192.168.0.247");
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr =  inet_addr("192.168.1.102");//;*((struct in_addr *)host->h_addr);
	server_addr.sin_port = htons(8080);
	server_addr.sin_len = sizeof(server_addr);
	
	do{
		sock_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (sock_fd == -1) 
		{
			//printf("ESP8266 UDP task > failed to create sock!\n");
			vTaskDelay(1000/portTICK_RATE_MS);
		}
	}while(sock_fd == -1);
	
	if (0 !=connect(sock_fd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr_in)))
	{	
		printf("ESP8266 TCP client task > connect fail!\n");
	}
	else
		printf("ESP8266 TCP client task > connect suceess!\n");
		
	while (1) 
	{		
		//printf("Hello\r\n");
		sendto(sock_fd, buffer, sizeof(buffer),0,(struct sockaddr *)&server_addr,sizeof(struct sockaddr));
		//vTaskDelay(50);
		//recv(sock_fd, buffer, sizeof(buffer),0);
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
	
	//xTaskCreate(task2, "tsk2", 512, NULL, 2, NULL);
	xTaskCreate(mqtt_task, "mqtt", 512, NULL, 2, NULL);
	
}


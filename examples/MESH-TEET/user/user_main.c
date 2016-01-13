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

#include "mesh.h"

/*#define DEMO_AP_SSID "TP-LINK_AE9170"
#define DEMO_AP_PASSWORD "2014HGKJ"*/

//uint8 buffer[1400]="0123456789";
//extern void ieee80211_mesh_quick_init();

#define IPSTR "%d.%d.%d.%d"

//---------------------------------------------------------
/*bool ICACHE_FLASH_ATTR mesh_root_if()
{
    struct ip_info ipconfig;
    wifi_get_ip_info(STATION_IF, &ipconfig);
	printf("=====================\r\n\n");
	printf("in mesh_root_if: ip: "IPSTR" \r\n",IP2STR(&(ipconfig.ip)));
	printf("in mesh_root_if: netmask: "IPSTR"\r\n",IP2STR(&ipconfig.netmask));
	printf("in mesh_root_if: gateway: "IPSTR"\r\n",IP2STR(&ipconfig.gw));
	printf("=====================\r\n\n");
	
    struct ip_info apconfig;
    wifi_get_ip_info(SOFTAP_IF, &apconfig);
	printf("=====================\r\n\n");
	printf("in mesh_root_if: ap ip: "IPSTR"\r\n",IP2STR(&apconfig.ip));
	printf("in mesh_root_if: ap netmask: "IPSTR"\r\n",IP2STR(&apconfig.netmask));
	printf("in mesh_root_if: ap gateway: "IPSTR"\r\n",IP2STR(&apconfig.gw));
	printf("=====================\r\n\n");
    if(ipconfig.ip.addr == 0) return false;

    if(espconn_mesh_local_addr(&ipconfig.ip)) return false;
    else return true;
}*/

//void * mesh_EnableCb(sint8 status)
LOCAL void  mesh_EnableCb(int8_t status)
{
    if(status == MESH_LOCAL)
	{
		printf("mesh_EnableCb\r\n");
    }
}

void task2(void *pvParameters)
{
/*	struct espconn pespconn;
	
	pespconn.type = ESPCONN_UDP;
	pespconn.
	();*/
	
	//espconn_mesh_enable(NULL, MESH_LOCAL);
	//ieee80211_mesh_quick_init();
	//espconn_mesh_connect(pespconn);

	//espconn_mesh_init();
	vTaskDelay(200);
	//printf("mesh state %d !\r\n",espconn_mesh_get_status());
		
	while (1) 
	{		
		//printf("mesh state %d !\r\n",espconn_mesh_get_status());
		printf("mesh state");
		vTaskDelay(50);
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
	
	xTaskCreate(task2, "tsk2", 512, NULL, 2, NULL);
}


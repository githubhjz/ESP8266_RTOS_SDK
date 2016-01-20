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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "uart.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "MQTTClient.h"
#include "stdio.h"

#define DEMO_AP_SSID "TP-LINK_AE9170"
#define DEMO_AP_PASSWORD "2014HGKJ"

typedef struct
{
	unsigned char	type;
	unsigned char  size;
	unsigned char  buff[128];
}mq_typedef;//message queue

//uint8 buffer[256]="0123456789";
char topic_uart0_tx[]  = "/Dust/T/XXXXXXXXXXXX/1",topic_uart0_rx[] ="/Dust/R/XXXXXXXXXXXX/1";

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
    (char*)"IoT GateWay WL ", 0, (char*)"\n", QOS2, NULL, NULL, (char*)"192.168.1.102", 1883, 0
};

Network n;//server net
static Client c;//client
xTimerHandle xTimer;//mqtt port timer
static unsigned char mqtt_state = 0,old_mqtt_state = 0;//
MQTTMessage send_messge= {QOS0,0,10};//send buffer;
xQueueHandle xQueueUart0;//

void vTimerCallback( xTimerHandle pxTimer )
{
	extern unsigned long MilliTimer;
	
	MilliTimer += 10;
}

void mqtt_pub_task(void *pvParameters)
{
	//wifi
	struct station_config * config = (struct station_config *)zalloc(sizeof(struct station_config));
	struct scan_config config2;
	//mqtt
	unsigned char *sendbuf,*readbuf;
	int rc = 0;
	//mqtt port timer
	int32_t x=1;
	//send buffer
	unsigned char *send=(unsigned char *)pvPortMalloc(128);
	unsigned int test=0;
	mq_typedef uart0rcv;
	//connet wifi
	wifi_set_opmode(STATION_MODE);
	sprintf(config->ssid, DEMO_AP_SSID);
	sprintf(config->password, DEMO_AP_PASSWORD);
	wifi_station_set_config(config);
	free(config);
	wifi_station_connect();
	vTaskDelay(200);
	printf("wifi connect\n"); 
	//mqtt timer
	xTimer = xTimerCreate(    "Timer",       // Just a text name, not used by the kernel.
						    1,   // The timer period in ticks.
							pdTRUE,        // The timers will auto-reload themselves when they expire.
							( void * ) x,  // Assign each timer a unique id equal to its array index.
							vTimerCallback // Each timer calls the same callback when it expires.
						  );
	xTimerStart( xTimer, 0 );
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
	mqtt_state = 1;
	vTaskDelay(80);
	while(1)
	{
		xQueueReceive(xQueueUart0,&uart0rcv,portMAX_DELAY);
		
		send_messge.payload 	= uart0rcv.buff;
		send_messge.payloadlen 	= uart0rcv.size;//strlen(send);
		rc = MQTTPublish(&c,topic_uart0_tx,&send_messge);
		if(rc)
		{	
			mqtt_state = 0;
			old_mqtt_state = 0;
			MQTTDisconnect(&c);
			printf("pub ,errno:%d\n",errno);			
			//net_disconnect(&n);				
			ConnectNetwork(&n, opts.host, opts.port);
			MQTTClient(&c, &n, 10, sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));
			rc = MQTTConnect(&c, &data);
			printf("connecting to %s %d err :%d\n", opts.host, opts.port,rc); 
			
			if(0 == rc)
				mqtt_state = 1;
		}	
		//vTaskDelay(200);
	}
	
}


void uart0_sub_message_arrived(MessageData* md)
{
    MQTTMessage* message = md->message;
    if (opts.showtopics)
        printf("%.*s\t", md->topicName->lenstring.len, md->topicName->lenstring.data);
    if (opts.nodelimiter)
        printf("%.*s", (int)message->payloadlen, (char*)message->payload);
    else
	{
       // printf("%.*s%s", (int)message->payloadlen, (char*)message->payload, opts.delimiter);
	   //printf("%s", (char*)message->payload);
	   uart0Send((char*)message->payload,message->payloadlen);
	}
}	
void mqtt_sub_task(void *pvParameters)
{
	int rc = 0;
	while(1)
	{
		if(mqtt_state)
		{
			if(!old_mqtt_state)
			{
				//topic_rx
				printf("Subscribing to %s\n", topic_uart0_rx);
				rc = MQTTSubscribe(&c, topic_uart0_rx, opts.qos, uart0_sub_message_arrived);
				printf("Subscribed %d\n", rc);
				if(rc)
				{	
					rc = MQTTSubscribe(&c, topic_uart0_rx, opts.qos, uart0_sub_message_arrived);
					printf("Re-Subscribed %d\n", rc);
				}
				
				old_mqtt_state = 1;			
			}//*/
			MQTTYield(&c, 4);
			//vTaskDelay(2);
		}
		else
		{
			vTaskDelay(100);
			printf("sub idle\n");
		}
		
	}
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
	
	//queue
	xQueueUart0 = xQueueCreate( 2, sizeof(mq_typedef) );
	//task
	//xTaskCreate(task2, "tsk2", 512, NULL, 2, NULL);
	xTaskCreate(mqtt_pub_task, "mqtt_pub", 512, NULL, 2, NULL);
	xTaskCreate(mqtt_sub_task, "mqtt_sub", 512, NULL, 2, NULL);
	
}


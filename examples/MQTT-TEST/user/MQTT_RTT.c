/*
 * File      : MQTT_RTT.c
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-09-24     zhjxy      the first version
 */
#include "MQTT_RTT.h"
#include "lwip/api.h"
#include "lwip/tcp.h"
#include <lwip/netdb.h> 
#include "lwip/sockets.h" 

unsigned long MilliTimer;

void SysTickIntHandler(void) {
	MilliTimer++;
}

char expired(Timer* timer) {
	long left = timer->end_time - MilliTimer;
	return (left < 0);
}


void countdown_ms(Timer* timer, unsigned int timeout) {
	timer->end_time = MilliTimer + timeout;
}


void countdown(Timer* timer, unsigned int timeout) {
	timer->end_time = MilliTimer + (timeout * 1000);
}


int left_ms(Timer* timer) {
	long left = timer->end_time - MilliTimer;
	return (left < 0) ? 0 : left;
}


void InitTimer(Timer* timer) {
	timer->end_time = 0;
}


int net_read(Network* n, unsigned char* buffer, int len, int timeout_ms) 
{	
//	fd_set fdset;
//	struct timeval timeVal;
//	int rc = 0;
//	int recvLen = 0;
//	int errcnt=0;
//	
//	FD_ZERO(&fdset);
//	FD_SET(n->my_socket,&fdset);
//	
//	timeVal.tv_sec = 0;
//	timeVal.tv_usec = timeout_ms * 1000;
//	if (select(n->my_socket + 1, &fdset, NULL, NULL, &timeVal) == 1)
//	{	
//		do {
//			rc = recv(n->my_socket, buffer + recvLen, len - recvLen, 0);
//			if(rc < 1)//zhj
//			{
////				if (errcnt++ >= 20)
////					return recvLen;
//				break;
//			}
//			else
//				recvLen += rc;
//		} while(recvLen < len);
//	}
//	return recvLen;

	int bytes = 0;
	int rc=0;

	if(timeout_ms < 10)
		timeout_ms  = 100;
	setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout_ms, sizeof(int));
	while (bytes < len)
	{
		rc = recv(n->my_socket, &buffer[bytes], (size_t)(len - bytes), 0);
		if (rc < 1)
			break;
		else
			bytes += rc;
	}
	return bytes;	//errno

}


int net_write(Network* n, unsigned char* buffer, int len, int timeout_ms) 
{
//	int rc = 0;
//	fd_set fd_setw;
//	struct timeval timeVal;
//	int readySock;
//	
//	FD_ZERO(&fd_setw);
//	FD_SET(n->my_socket,&fd_setw);
//	
//	timeVal.tv_sec = 0;
//	timeVal.tv_usec = timeout_ms * 1000;
//	do
//		readySock = select(n->my_socket + 1, NULL, &fd_setw, NULL, &timeVal);
//	while(readySock != 1);
//	rc = send(n->my_socket, buffer, len, 0);
//	return rc;
	
	struct timeval tv;
	int	rc=0;
	tv.tv_sec = 0;  /* 30 Secs Timeout */
	tv.tv_usec = timeout_ms * 1000;  // Not init'ing this can cause strange errors

	setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
	rc = send(n->my_socket, buffer, len,0);
	return rc;	
}


void net_disconnect(Network* n) 
{
	char cDontLinger=0; 
	setsockopt(n->my_socket, SOL_SOCKET, SO_DONTLINGER, (char *)&cDontLinger,sizeof( cDontLinger));
	closesocket(n->my_socket);
}


void NewNetwork(Network* n) 
{
	n->my_socket = 0;
	n->mqttread = net_read;
	n->mqttwrite = net_write;
	n->disconnect = net_disconnect;
}

//int TLSConnectNetwork(Network *n, char* addr, int port, SlSockSecureFiles_t* certificates, unsigned char sec_method, unsigned int cipher, char server_verify) {


//	return 0;
//}

int ConnectNetwork(Network* n, char* addr, int port)
{
	int err=0;		
	struct hostent *host;
	struct sockaddr_in srv_addr;
	int opt = 1;
	
	host = gethostbyname(addr);	
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(port);
	srv_addr.sin_addr = *((struct in_addr *) host->h_addr);//*((struct in_addr *)htonl(0xc0a80164));//
	memset(&(srv_addr.sin_zero), 0, sizeof(srv_addr.sin_zero));
	
	n->my_socket = socket(AF_INET, SOCK_STREAM, 0);
	if( n->my_socket < 0 ) 	
		return -1;// error
	
	err = connect(n->my_socket, (struct sockaddr *) &srv_addr,sizeof(struct sockaddr));
	if( err < 0 )	
	{	// error
		closesocket(n->my_socket);
		//return err;
		return -1;
	}
	setsockopt(n->my_socket,SOL_SOCKET,SO_KEEPALIVE,&opt,sizeof(int));//保活定时器//TCP_KEEPINTVL_DEFAULT 
	
	//start systick
	MilliTimer = 0;//         
	return err;
}

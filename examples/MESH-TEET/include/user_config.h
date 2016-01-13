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

#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__


// #define ESP_MESH_SUPPORT   1  /*set 1: enable mesh*/
// #define MESH_HEADER_EN     1
// #define MESH_HEADER_MODE  (ESP_MESH_SUPPORT&&MESH_HEADER_EN)
// /*============================================================*/
// /*====                                                    ====*/
// /*====        PARAMS FOR ESP-MESH                         ====*/
// /*====                                                    ====*/
// /*============================================================*/

// #if ESP_MESH_SUPPORT

    // /*This is the configuration of mesh*/
	// #define MESH_SERVER_CONF  1
	// #if MESH_SERVER_CONF
	    // #define MESH_SERVER_PORT  7000
	// #else
        // #define MESH_SERVER_PORT  8000
	// #endif

    // /*set 1 to use the GROUP_ID below,otherwise, a default GROUP_ID would be used  */
    // #define MESH_SET_GROUP_ID  1
	
	// /*A unique ID for a mesh network, suggest to use the MAC of one of the device in the mesh.*/
	// static const uint8 MESH_GROUP_ID[6] = {0x18,0xfe,0x34,0x00,0x00,0x11};

	// /*number of retry attempts after mesh enable failed;*/
    // #define MESH_INIT_RETRY_LIMIT 0       

	// /*set max hops of mesh*/
    // #define MESH_MAX_HOP_NUM  5      

	// /*limit time of mesh init, 0:no limit*/
    // #define MESH_INIT_TIME_LIMIT  0 

	// /* timeout for mesh enable */
    // #define MESH_TIME_OUT_MS   30000     

	// /*0:do not set a timeout for mesh init, mesh would try searching and joining in a available network*/
    // #define MESH_INIT_TIMEOUT_SET  0	  

	// /*time expire to check mesh init status*/
    // #define MESH_STATUS_CHECK_MS  1000    

	// /*length of binary upgrade stream in a single packet*/
    // #define MESH_UPGRADE_SEC_SIZE 640   

	// /*SET THE DEFAULT MESH SSID PATTEN;THE FINAL SSID OF SOFTAP WOULD BE "MESH_SSID_PREFIX_X_YYYYYY"*/
    // #define MESH_SSID_PREFIX "ESP_MESH"

	// /*AUTH_MODE OF SOFTAP FOR EACH MESH NODE*/
    // #define MESH_AUTH  AUTH_WPA2_PSK     

	// /*SET PASSWORD OF SOFTAP FOR EACH MESH NODE*/
    // #define MESH_PASSWORD "123123123"    

// #endif
#endif


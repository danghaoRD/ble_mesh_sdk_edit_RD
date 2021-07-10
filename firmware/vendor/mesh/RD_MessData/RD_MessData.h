/*
 * RD_MessData.h
 *
 *  Created on: Jul 10, 2021
 *      Author: HAO DANG
 */

#ifndef RD_MESSDATA_H_
#define RD_MESSDATA_H_

#include "proj/tl_common.h"
#include "proj/mcu/watchdog_i.h"
#include "vendor/common/user_config.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/sig_mesh/app_mesh.h"

//#include "../RD_SwitchAC4Ch_Touch/RD_SwitchAC4Ch_Touch.h"
//#include "../RD_Flash/RD_Flash.h"



#define SENSOR_OPCODE_SWITCH 1

#define RD_OPCODE_TYPE_SEND				(0xE0)
#define RD_OPCODE_TYPE_RSP				(0xE1)
#define RD_HEADER_SET_TYPE_DEVICE		(0x0001)
#define RD_HEADER_SAVE_GW_ADD			(0x0002)
#define RD_HEADER_RSP_TYPE_DEVICE		(0x0003)
#define RD_HEADER_FACTORY_TEST			(0X0004)

#define RD_OPCODE_SCENE_SEND			(0xE2)
#define RD_OPCODE_SCENE_RSP				(0xE3)
#define RD_HEADER_SCENE_REMOTE_CONTROL	(0x0100)
#define RD_HEADER_SCENE_REMOTE_SAVE		(0x0101)
#define RD_HEADER_SCENE_REMOTE_DELETE	(0x0102)

#ifndef 	X1_TOUCH_SWITCH
#define 	X1_TOUCH_SWITCH				1
#endif
#ifndef 	X2_TOUCH_SWITCH
#define 	X2_TOUCH_SWITCH				2
#endif
#ifndef 	X3_TOUCH_SWITCH
#define 	X3_TOUCH_SWITCH				3
#endif
#ifndef 	X4_TOUCH_SWITCH
#define 	X4_TOUCH_SWITCH				4
#endif
#ifndef 	HIPOW_TOUCH_SWITCH
#define 	HIPOW_TOUCH_SWITCH			5
#endif
#ifndef 	TYPE_HARDWARE_SWITCH
#define 	TYPE_HARDWARE_SWITCH		X4_TOUCH_SWITCH
#endif

#if(TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH)
	#define RD_HEADER_CONTROL_SWITCH4TOUCH		(0x000B)
	#define RD_HEADER_SW4TOUCH_SAVE_SCENE		(0x010B)
	#define RD_HEADER_SW4TOUCH_DELETE_SCENE		(0x020B)
	#define RD_HEADER_SWITCH4TOUCH_STT			(0x030B)
	#define RD_HEADER_SENSOR_SW4T_STT			(0x000B)
#endif

#if(TYPE_HARDWARE_SWITCH == X1_TOUCH_SWITCH)
	#define RD_HEADER_CONTROL_SWITCH4TOUCH		(0x000E)
	#define RD_HEADER_SW4TOUCH_SAVE_SCENE		(0x010E)
	#define RD_HEADER_SW4TOUCH_DELETE_SCENE		(0x020E)
	#define RD_HEADER_SWITCH4TOUCH_STT			(0x030E)
	#define RD_HEADER_SENSOR_SW4T_STT			(0x000E)
#endif
#define RD_OPCODE_POWER 				(0x0001)

#define RD_VENDOR_EN	1

#define RD_MAXRESPONESEND (2)

#define HEADER_CONTROL	0x0104

#define GW_ADDRESS		0x0001

typedef struct
		{
			uint8_t Header[2];
			uint8_t Relay_Stt[4];
			uint8_t Future[2];
		} STT_Relay_Mess;
STT_Relay_Mess STT_Relay_Mess_Config;

typedef struct
		{
			uint8_t Header[2];
			uint8_t Scene_ID[2];
			uint8_t Relay_Stt[4];
		} Save_Scene_Mess;
Save_Scene_Mess Save_Scene_Mess_Config;

typedef struct
		{
			uint8_t Header[2];
			uint8_t Scene_ID[2];
			uint8_t Future[4];
		} Delete_Scene_Mess;
Delete_Scene_Mess Delete_Scene_Mess_Config;

typedef struct{
	unsigned char Header[2];
	unsigned char MainType;
	unsigned char Feature;
	unsigned char TypeScene;
	unsigned char Reserved[3];
}RD_TypeDevice_Message;
 RD_TypeDevice_Message	vrts_RD_TypeDeviceMessage_Config;


// RD_EDIT RD_VENDOR cac ham xu ly.h
#define mesh_cmd_sig_RD_respone_status      		(0)
#define mesh_cmd_sig_RD_respone_status_type      	(0)
#define mesh_cmd_sig_RD_respone_status_SCENE      	(0)
#define mesh_cmd_sig_RD_respone_status_Switch		(0)
int mesh_cmd_sig_set_switch(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par) ; //SENSOR_GET
int RD_Messenger_ProcessCommingProcess_SCENE(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int RD_Messenger_ProcessCommingProcess_Type(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);

void Send_Relay_Stt_Message(void);



#endif /* RD_MESSDATA_H_ */

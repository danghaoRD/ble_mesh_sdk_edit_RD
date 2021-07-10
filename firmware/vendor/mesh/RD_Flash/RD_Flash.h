/*
 * RD_Flash.h
 *
 *  Created on: Jul 10, 2021
 *      Author: HAO DANG
 */

#ifndef RD_FLASH_H_
#define RD_FLASH_H_
#include "proj/tl_common.h"
#include "proj/mcu/watchdog_i.h"
#include "vendor/common/user_config.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/sig_mesh/app_mesh.h"

#include "../RD_MessData/RD_MessData.h"
//#include "../RD_SwitchAC4Ch_Touch/RD_SwitchAC4Ch_Touch.h"



#define FLASH_ADDR               		0x78000//0x023000
#define DATA_POWER_OFF_FLASH_ADDR      	0x79000//0x023000
#define FLASH_BUFF_LEN           		67
#define FLASH_DATA_SIZE			 		67
#define FLASH_DATA_POWER_OFF_SIZE		9
extern unsigned char  Buff_Flash_Read[FLASH_BUFF_LEN];
extern unsigned char  Buff_Flash_Write[FLASH_BUFF_LEN] ;

typedef struct
{
	uint8_t Factory_Check;
	uint8_t Future[4];

	uint8_t Scene1ID[2];
	uint8_t Data_Scene1[4];

	uint8_t Scene2ID[2];
	uint8_t Data_Scene2[4];

	uint8_t Scene3ID[2];
	uint8_t Data_Scene3[4];

	uint8_t Scene4ID[2];
	uint8_t Data_Scene4[4];

	uint8_t Scene5ID[2];
	uint8_t Data_Scene5[4];

	uint8_t Scene6ID[2];
	uint8_t Data_Scene6[4];

	uint8_t Scene7ID[2];
	uint8_t Data_Scene7[4];

	uint8_t Scene8ID[2];
	uint8_t Data_Scene8[4];

	uint8_t Scene9ID[2];
	uint8_t Data_Scene9[4];

	uint8_t Scene10ID[2];
	uint8_t Data_Scene10[4];

	uint8_t GW_Add[2];

} Sw_Flash_Data;
Sw_Flash_Data Sw_Flash_Data_Val;


typedef struct
{

	uint8_t Stt_LastPower[4];
	uint8_t Factory_Check;
	uint8_t	Counter_Eraser[4];		// 32 bit ( max 100.000 )
} Sw_Flash_Data_Save_Power_Off;
Sw_Flash_Data_Save_Power_Off Sw_Flash_Data_Save_Power_Off_Val;
#define SIZE_SCENE_DATA 	6  // 6byte 2sceneID+4relay
// Check and Init Flash
//
void RD_Flash_Init();
// Delete user Flash
void RD_Flash_CleanSenceFlashPower(unsigned long AddFlashPower);
void RD_Flash_CleanSenceFlash();
/*
 * save Scene to user Flash
 *	Iniput: SceneID
 *	Stt_Relay_x;  0-Off  1--On   >=2 don't save
 */
int RD_Flash_SaveScene(uint16_t Scene_ID, uint8_t Stt_Relay1, uint8_t Stt_Relay2, uint8_t Stt_Relay3, uint8_t Stt_Relay4);

/*
 * call scene saved
 */
void RD_Flash_ActiveSceneFromFlash(uint16_t Scene_ID);
/*
 * delete scene saved
 */
int RD_Flash_DeleteScene(uint16_t Scene_ID );
/*
 * Save new Gateway address
 */
void RD_Flash_SaveGwAdd(uint16_t Gw_Add);
/*
 * Save last data of Relays Stt  before Power OFF
 */
void RD_Flash_SavePowerOff(void);

#endif /* RD_FLASH_H_ */

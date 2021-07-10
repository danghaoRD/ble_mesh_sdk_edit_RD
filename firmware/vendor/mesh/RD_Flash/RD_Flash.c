/*
 * RD_Flash.c
 *
 *  Created on: Jul 10, 2021
 *      Author: HAO DANG
 */
#include "RD_Flash.h"
extern uint16_t RD_GATEWAYADDRESS;
uint8_t Jump_Flash=0x00;												// val to set add Flash for Power Off data
uint32_t Counter_Flash_Power_Eraser =0x00000000;						// counter Eraser Flash sector.  Higher 80000. move Add Flash Power Off ++
void RD_Flash_Init()
{
	/*----------------------------------------Flash of Scene------------------------------------------*/
	uint8_t *Flash_Data_Buff;
	Flash_Data_Buff  = (uint8_t *) (&Sw_Flash_Data_Val);

	flash_read_page(FLASH_ADDR, FLASH_DATA_SIZE, Flash_Data_Buff);
	if(Sw_Flash_Data_Val.Factory_Check == 0xFF)															// fist time to read reset memory
	{
		RD_Flash_CleanSenceFlash();
	}
	else
	{
		RD_GATEWAYADDRESS = (Sw_Flash_Data_Val.GW_Add[1] << 8 ) && Sw_Flash_Data_Val.GW_Add[0];			// check GW add
		if(RD_GATEWAYADDRESS == 0x0000)
		{
			RD_GATEWAYADDRESS=0x0001;
		}

		char UART_TempSend[128];
		sprintf(UART_TempSend,"Flash data 0x780000:0x%x  \n GW add:   \n  ",Sw_Flash_Data_Val.Factory_Check/
				RD_GATEWAYADDRESS);
		uart_CSend(UART_TempSend);
	}
	/*-------------------------------------Data Power OFF------------------------------------------------------*/
	uint8_t *Flash_Data_Power_Off_Buff;
	Flash_Data_Power_Off_Buff  = (uint8_t *) (&Sw_Flash_Data_Save_Power_Off_Val);

	flash_read_page(DATA_POWER_OFF_FLASH_ADDR, FLASH_DATA_POWER_OFF_SIZE, Flash_Data_Power_Off_Buff);
	if(Sw_Flash_Data_Save_Power_Off_Val.Factory_Check == 0xFF)															// fist time to read reset memory
	{
		RD_Flash_CleanSenceFlashPower(DATA_POWER_OFF_FLASH_ADDR);
	}
	else
	{
		Jump_Flash = Sw_Flash_Data_Save_Power_Off_Val.Factory_Check;
		if(Jump_Flash != 0x00)																							// Flash Add = DATA_POWER_OFF_FLASH_ADDR+ jump Flash // Flash Add = DATA_POWER_OFF_FLASH_ADDR
		{
			flash_read_page((DATA_POWER_OFF_FLASH_ADDR + (0x1000*Jump_Flash)), FLASH_DATA_POWER_OFF_SIZE, Flash_Data_Power_Off_Buff);
			if(Sw_Flash_Data_Save_Power_Off_Val.Factory_Check == 0xFF)													// fist time to read reset memory
			{
				RD_Flash_CleanSenceFlashPower((DATA_POWER_OFF_FLASH_ADDR + (0x1000*Jump_Flash)));
			}
		}

//		relay1_Stt = Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[0];													// update data last power
//		relay2_Stt = Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[1];
//		relay3_Stt = Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[2];
//		relay4_Stt = Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[3];
		Counter_Flash_Power_Eraser = 	(Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[0] << 24) | (Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[1] << 16) | \
									 	(Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[2] << 8)  | (Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[3]);
//		char UART_TempSend[128];
//		sprintf(UART_TempSend,"Flash POWER Off: Add: 0x%x---- Count Eraser:0x%x  \n Fac: 0x%x---Relay Stt %x-%x-%x-%x",(DATA_POWER_OFF_FLASH_ADDR + (0x1000*Jump_Flash)), Counter_Flash_Power_Eraser,  Sw_Flash_Data_Save_Power_Off_Val.Factory_Check/
//				  relay1_Stt, relay2_Stt, relay3_Stt, relay4_Stt);
//		uart_CSend(UART_TempSend);
	}

}

void RD_Flash_CleanSenceFlash()
{
	uint8_t Buff_Flash_Null[FLASH_DATA_SIZE]={0};
	uart_CSend("Clean Sence Flash");
   	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, FLASH_DATA_SIZE,Buff_Flash_Null );
	flash_read_page(FLASH_ADDR, FLASH_DATA_SIZE, (uint8_t *) (&Sw_Flash_Data_Val));
}
void RD_Flash_CleanSenceFlashPower(unsigned long AddFlashPower)
{
	uint8_t Buff_Flash_Null[FLASH_DATA_POWER_OFF_SIZE]={0};
	uart_CSend("Clean Power Off Flash");
   	flash_erase_sector(AddFlashPower);
	flash_write_page(AddFlashPower, FLASH_DATA_POWER_OFF_SIZE,Buff_Flash_Null );
	flash_read_page(AddFlashPower, FLASH_DATA_POWER_OFF_SIZE, (uint8_t *) (&Sw_Flash_Data_Save_Power_Off_Val));
}
int RD_Flash_SaveScene(uint16_t Scene_ID, uint8_t Stt_Relay1, uint8_t Stt_Relay2, uint8_t Stt_Relay3, uint8_t Stt_Relay4)
{
	uint8_t *Flash_Data_Buff;
	Flash_Data_Buff  = (uint8_t *) (&Sw_Flash_Data_Val);

	#if UART_ON
	char UART_TempSend[128];
	sprintf(UART_TempSend,"  location: %d \n",Flash_Data_Buff);
	uart_CSend(UART_TempSend);
	#endif
	uint8_t Scene_Saved =0;
	uint16_t SceneID_Buff=0x0000;

//	for(int i=5; i<= (SCENE_MAX*SIZE_SCENE_DATA) ; i=i+SIZE_SCENE_DATA) 					// check Scene saved
//	{
//		SceneID_Buff = (*(Flash_Data_Buff+i) <<8) | *(Flash_Data_Buff+i+1);
//		//if(*(Flash_Data_Buff+i) == Scene_ID)					// Scene saved before
//		if(SceneID_Buff == Scene_ID)					// Scene saved before
//		{
//			Scene_Saved = i;
//			#if UART_ON
//			char UART_TempSend[128];
//			sprintf(UART_TempSend,"saved before: location: %d \n", ((i-5)/6+1));
//			uart_CSend(UART_TempSend);
//			#endif
//			break;
//		}
//
//		//if((Scene_Saved==0)&&(*(Flash_Data_Buff+i) == 0x00))	// Sector empty
//		if((Scene_Saved==0)&&(SceneID_Buff == 0x0000))	// Sector empty
//		{
//			Scene_Saved = i;
//		#if UART_ON
//		char UART_TempSend[128];
//		sprintf(UART_TempSend,"sector %d empty: \n", ((i-5)/6+1));
//		uart_CSend(UART_TempSend);
//		#endif
//		}
//	}

	if(Scene_Saved == 0)// full scenen
	{
		#if UART_ON
				uart_CSend("Full Scene\n");
		#endif
		return 0;  // fail  full scene
	}
	else
	{
		*(Flash_Data_Buff+Scene_Saved )  = Scene_ID>>8;
		*(Flash_Data_Buff+Scene_Saved+1) = Scene_ID;
//		if(Stt_Relay1 ==1 )
//		{
//			*(Flash_Data_Buff+Scene_Saved+2) = relay1_Stt;
//		}
//		else
//			*(Flash_Data_Buff+Scene_Saved+2) = 2;					// ignore
//
//		if(Stt_Relay2 == 1)
//		{
//			*(Flash_Data_Buff+Scene_Saved+3) = relay2_Stt;
//		}
//		else
//			*(Flash_Data_Buff+Scene_Saved+3) = 2;					// ignore
//
//		if(Stt_Relay3 == 1)
//		{
//			*(Flash_Data_Buff+Scene_Saved+4) = relay3_Stt;
//		}
//		else
//			*(Flash_Data_Buff+Scene_Saved+4) = 2;					// ignore
//
//		if(Stt_Relay4 == 1)
//		{
//			*(Flash_Data_Buff+Scene_Saved+5) = relay4_Stt;
//		}
//		else
//			*(Flash_Data_Buff+Scene_Saved+5) = 2;					// ignore
//	   	flash_erase_sector(FLASH_ADDR);
//		flash_write_page(FLASH_ADDR, FLASH_DATA_SIZE,Flash_Data_Buff );
	#if UART_ON
		char UART_TempSend[128];
		sprintf(UART_TempSend,"Save Scene: %d\n", ((Scene_Saved-5)/6+1));
		uart_CSend(UART_TempSend);
		return 1;  // success
	#endif
	}


}
void RD_Flash_SaveGwAdd(uint16_t Gw_Add)
{
	uint8_t Gw_Add_Buff[2]={0};
	Gw_Add_Buff[0] = Gw_Add & 0xFF;
	Gw_Add_Buff[1] = (Gw_Add >> 8) & 0xFF;
	uint8_t *Flash_Data_Buff;

	Sw_Flash_Data_Val.GW_Add[0] = Gw_Add_Buff[0];
	Sw_Flash_Data_Val.GW_Add[1] = Gw_Add_Buff[1];

	Flash_Data_Buff  = (uint8_t *) (&Sw_Flash_Data_Val);
   	flash_erase_sector(FLASH_ADDR);
	flash_write_page(FLASH_ADDR, FLASH_DATA_SIZE,Flash_Data_Buff );
#if UART_ON
	char UART_TempSend[128];
	sprintf(UART_TempSend,"Save GW Address: 0x%x%x\n", Sw_Flash_Data_Val.GW_Add[0], Sw_Flash_Data_Val.GW_Add[1 ]);
	uart_CSend(UART_TempSend);

#endif


}
void RD_Flash_ActiveSceneFromFlash(uint16_t Scene_ID)
{
	flash_read_page(FLASH_ADDR, FLASH_DATA_SIZE, (uint8_t *) (&Sw_Flash_Data_Val));
	uint8_t *Flash_Data_Buff;
	Flash_Data_Buff  = (uint8_t *) (&Sw_Flash_Data_Val);

//	#if UART_ON
//	char UART_TempSend[128];
//	sprintf(UART_TempSend,"  location: %d \n",Flash_Data_Buff);
//	uart_CSend(UART_TempSend);
//	#endif

	uint8_t Scene_Saved =0;
	uint16_t SceneID_Buff=0x0000;

//	for(int i=5; i<= (SCENE_MAX*SIZE_SCENE_DATA) ; i=i+SIZE_SCENE_DATA) 					// check Scene saved
//	{
//		SceneID_Buff = (*(Flash_Data_Buff+i) <<8) | *(Flash_Data_Buff+i+1);
//		//if(*(Flash_Data_Buff+i) == Scene_ID)					// Scene saved before
//		if(SceneID_Buff == Scene_ID)														// Scene saved before
//		{
//			Scene_Saved = i;
//			#if UART_ON
//			char UART_TempSend[128];
//			sprintf(UART_TempSend,"saved before: location: %d \n", ((i-5)/6+1));
//			uart_CSend(UART_TempSend);
//			#endif
//			break;
//		}
//
//	}

	if(Scene_Saved == 0)// full scenen
	{
		#if UART_ON
				uart_CSend("Scene don't save: \n");
		#endif
	}
	else
	{


		// 1-ON......0-OFF.......>=2-ignore
//		if(*(Flash_Data_Buff+Scene_Saved+2) == 0) relay1_Stt = Relay_Off;
//		if(*(Flash_Data_Buff+Scene_Saved+2) == 1) relay1_Stt = Relay_On;
//
//		if(*(Flash_Data_Buff+Scene_Saved+3) == 0) relay2_Stt = Relay_Off;
//		if(*(Flash_Data_Buff+Scene_Saved+3) == 1) relay2_Stt = Relay_On;
//
//		if(*(Flash_Data_Buff+Scene_Saved+4) == 0) relay3_Stt = Relay_Off;
//		if(*(Flash_Data_Buff+Scene_Saved+4) == 1) relay3_Stt = Relay_On;
//
//		if(*(Flash_Data_Buff+Scene_Saved+5) == 0) relay4_Stt = Relay_Off;
//		if(*(Flash_Data_Buff+Scene_Saved+5) == 1) relay4_Stt = Relay_On;


	#if UART_ON
		char UART_TempSend[128];
		sprintf(UART_TempSend,"Active Scene:0x%x  data: %d, %d, %d, %d\n", relay1_Stt, relay2_Stt, relay3_Stt,(uint8_t) relay4_Stt );
		uart_CSend(UART_TempSend);
	#endif
	}

	Send_Relay_Stt_Message();
}

int RD_Flash_DeleteScene(uint16_t Scene_ID )
{
	uint8_t *Flash_Data_Buff;
	Flash_Data_Buff  = (uint8_t *) (&Sw_Flash_Data_Val);

	uint8_t Scene_Saved =0;
	uint16_t SceneID_Buff=0x0000;

//	for(int i=5; i<= (SCENE_MAX*SIZE_SCENE_DATA) ; i=i+SIZE_SCENE_DATA) 					// check Scene saved
//	{
//		SceneID_Buff = (*(Flash_Data_Buff+i) <<8) | *(Flash_Data_Buff+i+1);
//
//		if(SceneID_Buff == Scene_ID)														// Scene saved before
//		{
//			Scene_Saved = i;
//			#if UART_ON
//			char UART_TempSend[128];
//			sprintf(UART_TempSend,"detect Scene: %d \n", ((i-5)/6+1));
//			uart_CSend(UART_TempSend);
//			#endif
//			break;
//		}
//	}

	if(Scene_Saved == 0)// no scene to delete
	{
		#if UART_ON
		char UART_TempSend[128];
		sprintf(UART_TempSend,"have no Scene: 0x%x to delete\n",Scene_ID);
		uart_CSend(UART_TempSend);
		#endif
		return 0;
	}
	else
	{
		*(Flash_Data_Buff+Scene_Saved )  = 0x00;
		*(Flash_Data_Buff+Scene_Saved+1) = 0x00;
		*(Flash_Data_Buff+Scene_Saved+2) = 0x00;
		*(Flash_Data_Buff+Scene_Saved+3) = 0x00;
		*(Flash_Data_Buff+Scene_Saved+4) = 0x00;
		*(Flash_Data_Buff+Scene_Saved+5) = 0x00;
	   	flash_erase_sector(FLASH_ADDR);
		flash_write_page(FLASH_ADDR, FLASH_DATA_SIZE,Flash_Data_Buff );
	#if UART_ON
		char UART_TempSend[128];
		sprintf(UART_TempSend,"deteled Scene: 0x%x\n", Scene_ID);
		uart_CSend(UART_TempSend);
	#endif
		return 1;
	}


}

void RD_Flash_SavePowerOff(void)
{
//	Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[0]	= relay1_Stt;
//	Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[1]	= relay2_Stt;
//	Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[2]	= relay3_Stt;
//	Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[3]	= relay4_Stt;

	Counter_Flash_Power_Eraser++;
	Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[0] = (uint8_t) (Counter_Flash_Power_Eraser & 0xFF000000);
	Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[1] = (uint8_t) (Counter_Flash_Power_Eraser & 0x00FF0000);
	Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[2] = (uint8_t) (Counter_Flash_Power_Eraser & 0x0000FF00);
	Sw_Flash_Data_Save_Power_Off_Val.Counter_Eraser[3] = (uint8_t) (Counter_Flash_Power_Eraser & 0x000000FF);

	if(Counter_Flash_Power_Eraser >=80000)																		// over eraser Flash this Sector
	{
		Jump_Flash++;																							// move to next Flash sector
		Counter_Flash_Power_Eraser=0;
		Sw_Flash_Data_Save_Power_Off_Val.Factory_Check = Jump_Flash;
		uint8_t *Flash_Data_Buff2;
		Flash_Data_Buff2  = (uint8_t *) (&Sw_Flash_Data_Save_Power_Off_Val);
	   	flash_erase_sector(DATA_POWER_OFF_FLASH_ADDR);															// update to 0x79000
		flash_write_page(DATA_POWER_OFF_FLASH_ADDR, FLASH_DATA_POWER_OFF_SIZE,Flash_Data_Buff2 );
	}
	uint8_t *Flash_Data_Buff;
	Flash_Data_Buff  = (uint8_t *) (&Sw_Flash_Data_Save_Power_Off_Val);
   	flash_erase_sector((DATA_POWER_OFF_FLASH_ADDR + (0x1000*Jump_Flash)));
	flash_write_page((DATA_POWER_OFF_FLASH_ADDR + (0x1000*Jump_Flash)), FLASH_DATA_POWER_OFF_SIZE,Flash_Data_Buff );

	#if UART_ON
		char UART_TempSend[128];
		sprintf(UART_TempSend,"Saves data power Off: %x-%x-%x-%x \n Eraser: %d Add: 0x%x \n",	Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[0], Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[1], Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[2], Sw_Flash_Data_Save_Power_Off_Val.Stt_LastPower[3], Counter_Flash_Power_Eraser, (DATA_POWER_OFF_FLASH_ADDR + (0x1000*Jump_Flash))  );
		uart_CSend(UART_TempSend);
	#endif
}

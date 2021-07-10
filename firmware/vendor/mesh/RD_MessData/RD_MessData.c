/*
 * RD_MessData.c
 *
 *  Created on: Jul 10, 2021
 *      Author: DANG HAO
 */


#include "RD_MessData.h"

extern STT_Relay_Mess STT_Relay_Mess_Config;

//RD_TYPE_MAINDEVICE 				TYPE_MAINDEVICE 	= RD_MAINTYPE_SWITCH;
//RD_TYPE_SWITCH					TYPE_SWITCH			= RD_SWITCHTYPE_TOUCH;
//#if(TYPE_HARDWARE_SWITCH == X4_TOUCH_SWITCH)
//RD_TYPE_TOUCH_SWITCH			TYPE_TOUCH_SWITCH	= RD_TYPE_TOUCHSWITCH4CH;
//#endif
#if(TYPE_HARDWARE_SWITCH == X1_TOUCH_SWITCH)
RD_TYPE_TOUCH_SWITCH			TYPE_TOUCH_SWITCH	= RD_TYPE_TOUCHSWITCH1CH;
#endif
uint16_t RD_GATEWAYADDRESS  = 0x0001;
int RD_Messenger_ProcessCommingProcess_SCENE(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	uart_CSend("co ban tin E2\n");

	uint16_t Header_buff =0x00;
	uint16_t Gw_Add_Buff =0x00;
	uint8_t  ButtonCtr_Buff=0x00;
	Header_buff = ((par[1]<<8) | par[0]);
	Gw_Add_Buff = cb_par->adr_src;

	switch (Header_buff)
	{
		case RD_HEADER_CONTROL_SWITCH4TOUCH:
		{
			ButtonCtr_Buff = par[2];
			switch (ButtonCtr_Buff)
			{
//				 case 0x01:
//				 {
//					 relay1_Stt = par[3];   // 1 on, 0 off
//				 }
//				 break;
//				 case 0x02:
//				 {
//					 relay2_Stt = par[3];   // 1 on, 0 off
//				 }
//				 break;
//				 case 0x03:
//				 {
//					 relay3_Stt = par[3];   // 1 on, 0 off
//				 }
//				 break;
//				 case 0x04:
//				 {
//					 relay4_Stt = par[3];   // 1 on, 0 off
//				 }
//				 break;
		  }

			#if UART_ON
				char UART_TempSend[128];
				sprintf(UART_TempSend,"Control SW: %x-%x-%x-%x \n",relay1_Stt, relay2_Stt, relay3_Stt, relay4_Stt);
				uart_CSend(UART_TempSend);
			#endif
				uint8_t *RD_Messenger_TempSend;
				RD_Messenger_TempSend   = par;
				//RD_Messenger_SendNode2GatewayRsp(RD_Messenger_TempSend,10);
				mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, RD_Messenger_TempSend, 6, Gw_Add_Buff, RD_MAXRESPONESEND);
			#if UART_ON
				uart_CSend("Responed");
			#endif
		}
		break;
		case RD_HEADER_SWITCH4TOUCH_STT:
		{
			Send_Relay_Stt_Message();
		}
			break;
		case RD_HEADER_SW4TOUCH_SAVE_SCENE:
		{
			uint16_t Scene_ID_Buff =0x0000;
			Scene_ID_Buff = par[3]<<8 | par[2];

			uint8_t Stt_Relay_Buff[4]={0};
			Stt_Relay_Buff[0] = par[4];
			Stt_Relay_Buff[1] = par[5];
			Stt_Relay_Buff[2] = par[6];
			Stt_Relay_Buff[3] = par[7];

			uint8_t Check_buff=0;
			Check_buff = RD_Flash_SaveScene(Scene_ID_Buff, Stt_Relay_Buff[0], Stt_Relay_Buff[1], Stt_Relay_Buff[2], Stt_Relay_Buff[3]);  // save Scene to Flash

			if(Check_buff == 0) // full scene
			{
				Save_Scene_Mess_Config.Scene_ID[0] 	= 0x00;
				Save_Scene_Mess_Config.Scene_ID[1] 	= 0x00;
			}
			if(Check_buff == 1) // save Scene success
			{
				Save_Scene_Mess_Config.Scene_ID[0]	= Scene_ID_Buff & 0xff;
				Save_Scene_Mess_Config.Scene_ID[1]	= Scene_ID_Buff >> 8;
			}

			Save_Scene_Mess_Config.Header[0]    = RD_HEADER_SW4TOUCH_SAVE_SCENE & 0xff;
			Save_Scene_Mess_Config.Header[1]	= RD_HEADER_SW4TOUCH_SAVE_SCENE >>8;
			Save_Scene_Mess_Config.Relay_Stt[0] = par[4];
			Save_Scene_Mess_Config.Relay_Stt[1] = par[5];
			Save_Scene_Mess_Config.Relay_Stt[2] = par[6];
			Save_Scene_Mess_Config.Relay_Stt[3] = par[7];

			uint8_t* Mess_Buff;
			Mess_Buff = (uint8_t *) (&Save_Scene_Mess_Config);
			mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, Mess_Buff, 8, Gw_Add_Buff, RD_MAXRESPONESEND);
			RD_SwitchAc4Ch_BlinkSet(5, 100);

		}
			break;
		case RD_HEADER_SW4TOUCH_DELETE_SCENE:
		{
			uint16_t Scene_ID_Buff = 0x0000;
			uint8_t  Check_Delete_Buff=0x00;
			Scene_ID_Buff = par[3]<<8 | par[2];

			Check_Delete_Buff = RD_Flash_DeleteScene(Scene_ID_Buff);//RD_EDIT Note

			Delete_Scene_Mess_Config.Header[0] = RD_HEADER_SW4TOUCH_DELETE_SCENE & 0xff;
			Delete_Scene_Mess_Config.Header[1] = RD_HEADER_SW4TOUCH_DELETE_SCENE >> 8;
			if(Check_Delete_Buff == 1)
			{
				Delete_Scene_Mess_Config.Scene_ID[0] = Scene_ID_Buff & 0xFF;
				Delete_Scene_Mess_Config.Scene_ID[1] = Scene_ID_Buff >> 8 ;
			}
			else																// have no scene to delete
			{
				Delete_Scene_Mess_Config.Scene_ID[0] = 0x00 ;
				Delete_Scene_Mess_Config.Scene_ID[1] = 0x00;
			}
			Delete_Scene_Mess_Config.Future[0]	 = 0x00;
			Delete_Scene_Mess_Config.Future[1]	 = 0x00;
			Delete_Scene_Mess_Config.Future[2]	 = 0x00;
			Delete_Scene_Mess_Config.Future[3]	 = 0x00;

			uint8_t* Mess_Buff;
			Mess_Buff = (uint8_t *) (&Delete_Scene_Mess_Config);
			mesh_tx_cmd2normal_primary(RD_OPCODE_SCENE_RSP, Mess_Buff, 8, Gw_Add_Buff, RD_MAXRESPONESEND);
			RD_SwitchAc4Ch_BlinkSet(5, 100);
		}
			break;
	}

	return 0;
}

int RD_Messenger_ProcessCommingProcess_Type(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par){
	#if UART_ON
	uart_CSend("Co ban tin");
	char UART_TempSend[128];
	sprintf(UART_TempSend,"Messenger type:%d-Content:%x-%x-%x-%x-%x-%x-%x-%x\n",par_len,par[0],par[1],par[2],par[3],par[4],par[5],par[6],par[7]);
	uart_CSend(UART_TempSend);
	uart_CSend("..\n");
	#endif

	uint16_t Gw_Add_Buff =0x00;
	Gw_Add_Buff = cb_par->adr_src;

	uint16_t header_Buff = 0x0000;
	header_Buff = (par[1] << 8 | par[0]);
	switch (header_Buff)
	{
		case(RD_HEADER_RSP_TYPE_DEVICE):
		{

//			vrts_RD_TypeDeviceMessage_Config.Header[0] =  RD_HEADER_RSP_TYPE_DEVICE & 0xff;
//			vrts_RD_TypeDeviceMessage_Config.Header[1] = (RD_HEADER_RSP_TYPE_DEVICE >> 8) & 0xff;
//			vrts_RD_TypeDeviceMessage_Config.MainType 	= TYPE_MAINDEVICE;
//			vrts_RD_TypeDeviceMessage_Config.Feature  	= TYPE_SWITCH;
//			vrts_RD_TypeDeviceMessage_Config.TypeScene	= TYPE_TOUCH_SWITCH;
//			vrts_RD_TypeDeviceMessage_Config.Reserved[0] = 0;
//			vrts_RD_TypeDeviceMessage_Config.Reserved[1] = 0;
//			vrts_RD_TypeDeviceMessage_Config.Reserved[2] = 0;

			uint8_t* Mess_buff;
			Mess_buff = (uint8_t*) (&vrts_RD_TypeDeviceMessage_Config);
			mesh_tx_cmd2normal_primary(RD_OPCODE_TYPE_RSP, Mess_buff, 8, Gw_Add_Buff, RD_MAXRESPONESEND);
		}
		break;
		case(RD_HEADER_SAVE_GW_ADD):
		{
//			RD_GATEWAYADDRESS = Gw_Add_Buff;
//			sprintf(UART_TempSend,"Save GWAdd:%x \n", RD_GATEWAYADDRESS);
//			uart_CSend(UART_TempSend);
//			RD_Flash_SaveGwAdd(RD_GATEWAYADDRESS);
//			vrts_RD_TypeDeviceMessage_Config.Header[0] = RD_HEADER_SAVE_GW_ADD & 0xff;
//			vrts_RD_TypeDeviceMessage_Config.Header[1] = (RD_HEADER_SAVE_GW_ADD >> 8) & 0xff;
//			vrts_RD_TypeDeviceMessage_Config.MainType 	= Sw_Flash_Data_Val.GW_Add[0];			//TYPE_MAINDEVICE;
//			vrts_RD_TypeDeviceMessage_Config.Feature  	= Sw_Flash_Data_Val.GW_Add[1];			//TYPE_SWITCH;
//			vrts_RD_TypeDeviceMessage_Config.TypeScene	= 0x00;									//TYPE_SCENE_SWITCH;
//			vrts_RD_TypeDeviceMessage_Config.Reserved[0] = 0x00;								// Buff_Flash_Read[60];
//			vrts_RD_TypeDeviceMessage_Config.Reserved[1] = 0x00;								//Buff_Flash_Read[61];
//			vrts_RD_TypeDeviceMessage_Config.Reserved[2] = 0x00;
			uint8_t* Mess_buff;
			Mess_buff = (uint8_t*) (&vrts_RD_TypeDeviceMessage_Config);
			mesh_tx_cmd2normal_primary(RD_OPCODE_TYPE_RSP, Mess_buff, 8, Gw_Add_Buff, RD_MAXRESPONESEND);
		}
		break;
	}

/*	if(header_Buff == RD_HEADER_FACTORY_TEST)
	{
		uart_CSend("Factory Test");
		RD_GATEWAYADDRESS = cb_par->adr_src;
		vrts_RD_TypeDeviceMessage_Config.Header[0] = RD_HEADER_FACTORY_TEST & 0xff;
		vrts_RD_TypeDeviceMessage_Config.Header[1] = (RD_HEADER_FACTORY_TEST >> 8) & 0xff;
//		vrts_RD_TypeDeviceMessage_Config.MainType 	= TYPE_MAINDEVICE;
//		vrts_RD_TypeDeviceMessage_Config.Feature  	= TYPE_SWITCH;
//		vrts_RD_TypeDeviceMessage_Config.TypeScene	= TYPE_SCENE_SWITCH;
		vrts_RD_TypeDeviceMessage_Config.Reserved[0] = Buff_Flash_Read[60];
		vrts_RD_TypeDeviceMessage_Config.Reserved[1] = Buff_Flash_Read[61];
		vrts_RD_TypeDeviceMessage_Config.Reserved[2] = 0;
		vrp_TypeDevice_Point2Message = (unsigned char *)(&vrts_RD_TypeDeviceMessage_Config);
		mesh_tx_cmd2normal_primary(RD_OPCODE_TYPE_RSP, vrp_TypeDevice_Point2Message, 8, RD_GATEWAYADDRESS, RD_MAXRESPONESEND);

	}*/

	return 0;
}


void Send_Relay_Stt_Message(void)
{
//	STT_Relay_Mess_Config.Header[0] 	= RD_HEADER_SENSOR_SW4T_STT & 0xFF ;
//	STT_Relay_Mess_Config.Header[1] 	= RD_HEADER_SENSOR_SW4T_STT >>8;
//	STT_Relay_Mess_Config.Relay_Stt[0] 	= relay1_Stt;
//	STT_Relay_Mess_Config.Relay_Stt[1] 	= relay2_Stt;
//	STT_Relay_Mess_Config.Relay_Stt[2] 	= relay3_Stt;
//	STT_Relay_Mess_Config.Relay_Stt[3] 	= relay4_Stt;
//	STT_Relay_Mess_Config.Future[0]		= 0x00;
//	STT_Relay_Mess_Config.Future[1]		= 0x00;

	uint8_t* Mess_Buff;
	Mess_Buff = (uint8_t *) (&STT_Relay_Mess_Config);
	mesh_tx_cmd2normal_primary(SENSOR_STATUS, Mess_Buff, 8, RD_GATEWAYADDRESS, RD_MAXRESPONESEND); //SENSOR_STATUS
}

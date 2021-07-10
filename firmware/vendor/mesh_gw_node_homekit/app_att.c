/********************************************************************************************************
 * @file     app_att.c 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
#include "proj/tl_common.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/service/ble_ll_ota.h"
#include "../common/app_provison.h"
#include "../common/app_proxy.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "../common/app_beacon.h"

#include "homekit_src/homekit_inc.h"
#include "../common/led_cfg.h"
#include "common.h"

#include "ble_UUID.h"
#include "hk_TopUUID.h"

#if(1)
#if 0
typedef struct
{
  /** Minimum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMin;
  /** Maximum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMax;
  /** Number of LL latency connection events (0x0000 - 0x03e8) */
  u16 latency;
  /** Connection Timeout (0x000A - 0x0C80 * 10 ms) */
  u16 timeout;
} gap_periConnectParams_t;
#endif
const u16 clientCharacterCfgUUID = GATT_UUID_CLIENT_CHAR_CFG;

const u16 extReportRefUUID = GATT_UUID_EXT_REPORT_REF;

const u16 reportRefUUID = GATT_UUID_REPORT_REF;

const u16 characterPresentFormatUUID = GATT_UUID_CHAR_PRESENT_FORMAT;

const u16 my_primaryServiceUUID = GATT_UUID_PRIMARY_SERVICE;

static const u16 my_characterUUID = GATT_UUID_CHARACTER;

const u16 my_devServiceUUID = SERVICE_UUID_DEVICE_INFORMATION;

const u16 my_PnPUUID = CHARACTERISTIC_UUID_PNP_ID;

const u16 my_devNameUUID = GATT_UUID_DEVICE_NAME;

//device information
const u16 my_gapServiceUUID = SERVICE_UUID_GENERIC_ACCESS;
// Device Name Characteristic Properties
static u8 my_devNameCharacter = CHAR_PROP_READ | CHAR_PROP_NOTIFY;
// Appearance Characteristic Properties
const u16 my_appearanceUIID = 0x2a01;
const u16 my_periConnParamUUID = 0x2a04;
static u8 my_appearanceCharacter = CHAR_PROP_READ;
// Peripheral Preferred Connection Parameters Characteristic Properties
static u8 my_periConnParamChar = CHAR_PROP_READ;
u16 my_appearance = GAP_APPEARE_UNKNOWN;
gap_periConnectParams_t my_periConnParameters = {20, 40, 0, 1000};


extern u8  ble_devName[];

// Device Name Characteristic Properties
static u8 my_PnPCharacter = CHAR_PROP_READ;


const u8	my_PnPtrs [] = {0x02, 0x8a, 0x24, 0x66, 0x82, 0x01, 0x00};

//////////////////////// Battery /////////////////////////////////////////////////
const u16 my_batServiceUUID       			= SERVICE_UUID_BATTERY;
//static u8 my_batProp 						= CHAR_PROP_READ | CHAR_PROP_NOTIFY;
const u16 my_batCharUUID       				= CHARACTERISTIC_UUID_BATTERY_LEVEL;
u8 		  my_batVal[1] 						= {99};

/////////////////////////////////////////////////////////
const u16 userdesc_UUID		= GATT_UUID_CHAR_USER_DESC;

/////////////////////////////////////////spp/////////////////////////////////////
#if (TELIK_SPP_SERVICE_ENABLE)
u8 TelinkSppServiceUUID[16]	      = TELINK_SPP_UUID_SERVICE;
u8 TelinkSppDataServer2ClientUUID[16]    = TELINK_SPP_DATA_SERVER2CLIENT;
u8 TelinkSppDataClient2ServerUUID[16]    = TELINK_SPP_DATA_CLIENT2SERVER;

// Spp data from Server to Client characteristic variables
static u8 SppDataServer2ClientProp = CHAR_PROP_READ | CHAR_PROP_NOTIFY;
u8  SppDataServer2ClientData[ATT_MTU_SIZE - 3];
static u8 SppDataServer2ClientDataCCC[2] = {0};

// Spp data from Client to Server characteristic variables
//CHAR_PROP_WRITE: Need response from slave, low transmission speed
static u8 SppDataClient2ServerProp = CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP; //CHAR_PROP_WRITE;
u8  SppDataClient2ServerData[ATT_MTU_SIZE - 3];

//SPP data descriptor
const u8 TelinkSPPS2CDescriptor[] = "Telink SPP: Module->Phone";
const u8 TelinkSPPC2SDescriptor[] = "Telink SPP: Phone->Module";

u32 	spp_err = 0;
void	spp_test_read (u8 *p, int n)
{
	static u32 spp_err_st;
	static u32 spp_read = 0;
	u32 seq;
	memcpy (&seq, p, 4);
	if (spp_read != seq)
	{
		spp_err++;
	}
	else
	{
		for (int i=4; i<n; i++)
		{
			if ((u8)(p[0] + i) != p[i])
			{
				spp_err++;
				spp_err_st += BIT(16);
				break;
			}
		}
	}
	spp_read = seq - 1;

	if (0 && spp_err > 1)
	{
#if UART_8266_EN
		gpio_set_input_en(GPIO_URX, 0);
#else
		gpio_set_input_en(GPIO_URXB3, 0);
#endif
		irq_disable ();
		while (1);
	}
}

void module_onReceiveData(rf_packet_att_write_t *p)
{
	u32 n;
	u8 len = p->l2capLen - 3;
	if(len > 0)
	{
#if 0
		static u32 sn = 0;
		memcpy (&n, &p->value, 4);
		if (sn != n)
		{
			sn = 0;
			bls_ll_terminateConnection (0x13);
		}
		else
		{
			sn = n + 1;
		}
#endif
		u32 header;
		header = 0x07a0;		//data received event
		header |= (3 << 16) | (1<<24);
		spp_test_read (&p->value, len);
		blc_hci_send_data(header, &p->opcode, len + 3);		//HCI_FLAG_EVENT_TLK_MODULE
	}
}
#endif
// Include attribute (Battery service)
//static u16 include[3] = {0x0026, 0x0028, SERVICE_UUID_BATTERY};
//const u8 ATT_PERMISSIONS_READ_AUTHOR
u8 att_perm_auth_read = ATT_PERMISSIONS_READ_AUTHOR;
u8 att_perm_auth_write = ATT_PERMISSIONS_WRITE_AUTHOR;
u8 att_perm_auth_rdwd  = ATT_PERMISSIONS_RDWD_AUTHOR;


#define MESH_PROVISON_DATA	{0xce,0x7f}
#define MESH_PROXY_DATA		{0xcf,0x7f}

const u8 my_OtaServiceUUID[16]		= TELINK_OTA_UUID_SERVICE;
const u8 my_OtaUUID[16]		= TELINK_SPP_DATA_OTA;

static u8 my_OtaProp		= CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP;
const u8  my_OtaName[] = {'O', 'T', 'A'};
u8	 	my_OtaData 		= 0x00;
// pb-gatt 
u8 my_pb_gattUUID[2]=SIG_MESH_PROVISION_SERVICE;

const u8 my_pb_gatt_out_UUID[2]= SIG_MESH_PROVSIION_DATA_OUT;
//static u8 my_pb_gatt_out_prop = CHAR_PROP_NOTIFY;
static u8 my_pb_gatt_out_prop = CHAR_PROP_NOTIFY;

const u8 my_pb_gattOutName[]={'P','B','G','A','T','T','-','O','U','T'};
u8 	my_pb_gattOutData[2] =MESH_PROVISON_DATA;

const u8 my_pb_gatt_in_UUID[2]= SIG_MESH_PROVISION_DATA_IN;
static u8 my_pb_gatt_in_prop =  CHAR_PROP_WRITE_WITHOUT_RSP;
const u8 my_pb_gattInName[]={'P','B','G','A','T','T','-','I','N'};
u8 	my_pb_gattInData[2] =MESH_PROVISON_DATA;

u8 my_proxy_gattUUID[2]= SIG_MESH_PROXY_SERVICE;

const u8 my_proxy_out_UUID[2]= SIG_MESH_PROXY_DATA_OUT;
static u8 my_proxy_out_prop = CHAR_PROP_NOTIFY;
const u8 my_proxy_out_Name[]={'P','R','O','X','Y','-','O','U','T'};
u8 my_proxy_outData[2] =MESH_PROXY_DATA;

const u8 my_proxy_in_UUID[2]= SIG_MESH_PROXY_DATA_IN;
static u8 my_proxy_in_prop = CHAR_PROP_WRITE_WITHOUT_RSP;
const u8 my_proxy_in_Name[]={'P','R','O','X','Y','-','I','N'};
u8 my_proxy_inData[2] =MESH_PROXY_DATA;


/*define for homekit att table.*/
u8 protocol_info_broadcast_param=1;
const hap_characteristic_desc_t hap_desc_char_protocol_info_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0,(u8*)&protocol_info_broadcast_param};
const hap_service_desc_t hap_desc_service_protocol_info_A5_R_S = {12, (0x00 + 0), 0, 0};

_attribute_data_retention_ u8 identify = 0xff;
_attribute_data_retention_ u8 identifyCB_w = 0;

#ifdef MFI_ENABLE
const u8 my_pairFeature = 1;
#else
#if SOFTWARE_AUTHENTICATION_ENABLE
const u8 my_pairFeature = 2;	//for software authentication.
#else
const u8 my_pairFeature = 0;
#endif
#endif

_attribute_data_retention_ u8 otabuffer[20] = {0};
_attribute_data_retention_ u8 hap_desc_char_protocol_info_temp_var=0;


/* ------------------------------------
    Type Definitions
   ------------------------------------ */
const u16 my_gapAttUUID = SERVICE_UUID_GENERIC_ATTRIBUTE;
const u16 my_devInfoUUID = SERVICE_UUID_DEVICE_INFORMATION;

//const u16 my_gapServiceUUID     = SERVICE_UUID_GENERIC_ACCESS;      // 0x1800

const u16 primaryServiceUUID    = GATT_UUID_PRIMARY_SERVICE;        // 0x2800   //!< Primary Service
const u16 characterUUID         = GATT_UUID_CHARACTER;              // 0x2803   //!< Characteristic

const u16 CEPUUID               = GATT_UUID_CHAR_EXT_PROPS;         // 0x2900   //!< Characteristic Extended Properties
//const u16 userdesc_UUID         = GATT_UUID_CHAR_USER_DESC;         // 0x2901   //!< Characteristic User Description
const u16 ClientCharConfig_UUID = GATT_UUID_CLIENT_CHAR_CFG;        // 0x2902   //!< Client Characteristic Configuration
const u16 CFGUUID               = GATT_UUID_CHAR_PRESENT_FORMAT;    // 0x2904   //!< Characteristic Present Format
const u16 ValidRangeUUID        = GATT_UUID_VALID_RANGE;            // 0x2906   //!< Valid Range

//const u16 my_devNameUUID        = GATT_UUID_DEVICE_NAME;            // 0x2a00   //!< Report Reference
//const u16 my_appearanceUUID     = GATT_UUID_APPEARANCE;             // 0x2a01   //!< Report Reference
//const u16 my_periConnParamUUID  = GATT_UUID_PERI_CONN_PARAM;    	// 0x2a04
const u16 my_ServiceChangedUUID = GATT_UUID_SERVICE_CHANGE;        	// 0x2a05   //!< Report Reference

//-const u16 CCCnoneProp           = 0x000;                            // 0x0000
const u16 CCCnotifyOnlyProp     = CLIENT_CHAR_CFG_NOTI;             // 0x0001 //!< permit broadcasts of the Characteristic Value
const u16 CCCindicationOnlyProp = CLIENT_CHAR_CFG_IND;              // 0x0002 //!< permit reads of the Characteristic Value

const u8 writeOnlyProp          = CHAR_PROP_WRITE;
const u8 writeNotifyProp        = CHAR_PROP_WRITE | CHAR_PROP_NOTIFY;
const u8 readOnlyProp           = CHAR_PROP_READ;
const u8 readIndicateProp       = CHAR_PROP_READ | CHAR_PROP_INDICATE;
const u8 readWriteProp          = CHAR_PROP_WRITE | CHAR_PROP_READ;
const u8 notifyOnlyProp         = CHAR_PROP_NOTIFY;
const u8 readWriteIndicateProp  = CHAR_PROP_WRITE | CHAR_PROP_READ | CHAR_PROP_INDICATE;
const u8 readWriteNotifyProp    = CHAR_PROP_WRITE | CHAR_PROP_READ | CHAR_PROP_NOTIFY;
const u8 ServiceChangedProp     = CHAR_PROP_READ | CHAR_PROP_INDICATE;  // 0x22;


int identifyWriteCallback(void *pp)
{
    identifyCB_w++;
    _attribute_data_retention_ static u8 identifyCB_w_previous=0;

    extern rf_packet_adv_t	pkt_adv;
    extern u8 blt_pair_ok;
    extern void cfg_led_event(u32 e);
    if(pkt_adv.data[ADV_PKT_PAIR_STATUS_FLAG])
    {
        if(identifyCB_w != identifyCB_w_previous)
        {
            identifyCB_w_previous = identifyCB_w;
            cfg_led_event(LED_EVENT_FLASH_1HZ_3TIMES);
        }
    }
    else
    {
        if(blt_pair_ok)
        {
            if(identifyCB_w != identifyCB_w_previous)
            {
                identifyCB_w_previous = identifyCB_w;
                cfg_led_event(LED_EVENT_FLASH_1HZ_3TIMES);
                extern u32 session_timeout_tick;
                session_timeout_tick = clock_time () | 1;
            }
        }
    }

    return 0;
}

#if USER_DEFINE_SET_CCC_ENABLE
const  u8 my_userdefine_service_UUID[16]= TELINK_USERDEFINE_GATT;
static u8 my_userdefine_prop		= CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP|CHAR_PROP_NOTIFY|CHAR_PROP_INDICATE;
u8	 	  my_userdefine_dat 		= 0x00;
const u8  my_userderdefine[4] = {'U', 'S', 'E','R'};
const u8  my_userdefine_UUID[16]= TELINK_USERDEFINE_UUID;
#endif

const u16  mi_gerneric_service  = SERVICE_UUID_GENERIC_ATTRIBUTE;
const u16 mi_service_change_uuid = 0x2a05;
static u8 mi_service_change_prop = CHAR_PROP_INDICATE;
static u8 mi_service_change_buf[4];
const u8 mi_service_change_str[]="service change";
u8 mi_service_change_ccc[2]=	{0x00,0x00};
u8 mi_service_change_char_perm = ATT_PERMISSIONS_READ;
u8 mi_service_change_buf_perm = ATT_PERMISSIONS_READ;
u8 mi_service_change_ccc_perm = ATT_PERMISSIONS_RDWR;

#if MI_API_ENABLE 
const u16 mi_primary_service_uuid = 0xfe95;
u8 mi_pri_service_perm = ATT_PERMISSIONS_READ_AUTHOR;

const u16 mi_version_uuid = 0x0004;
static u8 mi_version_prop = CHAR_PROP_READ;
static u8 mi_version_buf[20]="0.0.1_0000";
const u8 mi_version_str[]="Version";
u8 mi_version_perm = ATT_PERMISSIONS_RDWD_AUTHOR;

const u16 mi_ctrlp_uuid = 0x0010;
static u8 mi_ctrlp_prop = CHAR_PROP_WRITE_WITHOUT_RSP|CHAR_PROP_NOTIFY;
static u8 mi_ctrlp_buf[4];
const u8 mi_ctrlp_str[]="contrl point";
u8 mi_sec_ctrlp_ccc[2]=	{0x00,0x00};
u8 mi_sec_ctrlp_buf_perm = ATT_PERMISSIONS_RDWD_AUTHOR;
u8 mi_sec_ctrlp_ccc_perm = ATT_PERMISSIONS_RDWD_AUTHOR;


const u16 mi_sec_auth_uuid = 0x0016;
static u8 mi_sec_auth_prop = CHAR_PROP_WRITE_WITHOUT_RSP|CHAR_PROP_NOTIFY;
static u8 mi_sec_auth_buf[20];
const u8 mi_sec_auth_str[]="Security Auth";
u8 mi_sec_auth_ccc[2]=	{0x00,0x00};
u8 mi_sec_auth_buf_perm = ATT_PERMISSIONS_RDWD_AUTHOR;
u8 mi_sec_auth_ccc_perm = ATT_PERMISSIONS_RDWD_AUTHOR;

const u16 mi_ota_ctrl_uuid = 0x0017;
static u8 mi_ota_ctrl_prop = CHAR_PROP_WRITE|CHAR_PROP_NOTIFY;
static u8 mi_ota_ctrl_buf[20];
const u8 mi_ota_ctrl_str[]="Ota ctrl";
u8 mi_ota_ctrl_ccc[2]=	{0x00,0x00};
u8 mi_ota_ctrl_buf_perm = ATT_PERMISSIONS_RDWD_AUTHOR;
u8 mi_ota_ctrl_ccc_perm = ATT_PERMISSIONS_RDWD_AUTHOR;

const u16 mi_ota_data_uuid = 0x0018;
static u8 mi_ota_data_prop = CHAR_PROP_WRITE_WITHOUT_RSP|CHAR_PROP_NOTIFY;
static u8 mi_ota_data_buf[20];
const u8 mi_ota_data_str[]="Ota data";
u8 mi_ota_data_ccc[2]=	{0x00,0x00};
u8 mi_ota_data_buf_perm = ATT_PERMISSIONS_RDWD_AUTHOR;
u8 mi_ota_data_ccc_perm = ATT_PERMISSIONS_RDWD_AUTHOR;
#endif 

#if(AIS_ENABLE)
const u16 ais_pri_service_uuid = 0xfeb3;
const u16 ais_read_uuid = 0xfed4;
const u8 ais_read_prop = CHAR_PROP_READ;
const u16 ais_write_uuid = 0xfed5;
const u8 ais_write_prop = CHAR_PROP_READ|CHAR_PROP_WRITE;
const u16 ais_indicate_uuid = 0xfed6;
const u8 ais_indicate_prop = CHAR_PROP_READ|CHAR_PROP_INDICATE;
const u16 ais_write_without_rsp_uuid = 0xfed7;
const u8 ais_write_without_rsp_prop = CHAR_PROP_READ|CHAR_PROP_WRITE_WITHOUT_RSP;
const u16 ais_notify_uuid = 0xfed8;
const u8 ais_notify_prop = CHAR_PROP_READ|CHAR_PROP_NOTIFY;
const u8 ais_service_desc[]="Alibaba IoT Service";

u8 ais_data_buf[2];
#endif


#define MAX_SERVICE_GAP                 (7)
#define MAX_SERVICE_DEVICE_INFO         (5)
#define MAX_SERVICE_GATT_OTA            (4)
#define MAX_SERVICE_PROVISION           (9)
#define MAX_SERVICE_PROXY               (9)
#define MAX_HOMEKIT_SERVICE				(HOMEKIT_NO)
#define MAX_USER_DEFINE_SET_CCC_ATT_NUM (USER_DEFINE_SET_CCC_ENABLE ? 4 : 0)
#define MAX_MI_ATT_NUM                  (MI_API_ENABLE ? 20 : 0)
#define MAX_SERVICE_CHANGE_ATT_NUM      (5)
#define MAX_AIS_ATT_NUM 	            (AIS_ENABLE ? 12 : 0)
#define MAX_ONLINE_ST_ATT_NUM 	        (ONLINE_STATUS_EN ? 4 : 0)	// ONLINE_STATUS_EN always 0 now.

//---
#define ATT_NUM_START_GAP                   (1)     // line of ATT, start from 0.
#define ATT_NUM_START_DEVICE_INFO           (ATT_NUM_START_GAP + MAX_SERVICE_GAP)
#define ATT_NUM_START_GATT_OTA              (ATT_NUM_START_DEVICE_INFO + MAX_SERVICE_DEVICE_INFO)
#define ATT_NUM_START_PROVISION             (ATT_NUM_START_GATT_OTA + MAX_SERVICE_GATT_OTA)
#define ATT_NUM_START_PROXY                 (ATT_NUM_START_PROVISION + MAX_SERVICE_PROVISION)
#define ATT_NUM_START_HOMEKIT				(ATT_NUM_START_PROXY + MAX_SERVICE_PROXY)
#define ATT_NUM_START_USER_DEFINE_SET_CCC   (ATT_NUM_START_HOMEKIT + MAX_HOMEKIT_SERVICE)
#define ATT_NUM_START_MI_API                (ATT_NUM_START_USER_DEFINE_SET_CCC + MAX_USER_DEFINE_SET_CCC_ATT_NUM)
#define ATT_NUM_START_SERVICE_CHANGE        (ATT_NUM_START_MI_API + MAX_MI_ATT_NUM)
#define ATT_NUM_START_AIS                   (ATT_NUM_START_SERVICE_CHANGE + MAX_SERVICE_CHANGE_ATT_NUM)
#define ATT_NUM_START_ONLINE_ST             (ATT_NUM_START_AIS + MAX_AIS_ATT_NUM)

#define ATTRIBUTE_TOTAL_NUM                 (ATT_NUM_START_ONLINE_ST + MAX_ONLINE_ST_ATT_NUM + HOMEKIT_NO - 1)

/*const */u8 PROVISION_ATT_HANDLE = (ATT_NUM_START_PROVISION + 2);  // slave
/*const */u8 GATT_PROXY_HANDLE = (ATT_NUM_START_PROXY + 2);  // slave
const u8 SERVICE_CHANGE_ATT_HANDLE_SLAVE = (ATT_NUM_START_SERVICE_CHANGE + 2);
const u8 ONLINE_ST_ATT_HANDLE_SLAVE = (ATT_NUM_START_ONLINE_ST + 2);



#define MY_ATTRIBUTE_BASE0           \
    {ATTRIBUTE_TOTAL_NUM,0,0,0,0,0,0}, /* total num of attribute*/   \
	/* 0001 - 0007	gap*/   \
    {MAX_SERVICE_GAP,ATT_PERMISSIONS_READ_AUTHOR,2,2,2,(u8*)(&my_primaryServiceUUID),   (u8*)(&my_gapServiceUUID), 0},\
    {0,ATT_PERMISSIONS_READ_AUTHOR,2,1,1,(u8*)(&my_characterUUID),        (u8*)(&my_devNameCharacter), 0},\
    {0,ATT_PERMISSIONS_READ_AUTHOR,2,MAX_DEV_NAME_LEN,MAX_DEV_NAME_LEN, (u8*)(&my_devNameUUID), (u8*)(&ble_devName), 0},\
    {0,ATT_PERMISSIONS_READ_AUTHOR,2,1,1,(u8*)(&my_characterUUID),        (u8*)(&my_appearanceCharacter), 0},\
    {0,ATT_PERMISSIONS_READ_AUTHOR,2,sizeof (my_appearance),sizeof (my_appearance), (u8*)(&my_appearanceUIID),     (u8*)(&my_appearance), 0},\
    {0,ATT_PERMISSIONS_READ_AUTHOR,2,1,1,(u8*)(&my_characterUUID),        (u8*)(&my_periConnParamChar), 0},\
    {0,ATT_PERMISSIONS_READ_AUTHOR,2,sizeof (my_periConnParameters),sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID),   (u8*)(&my_periConnParameters), 0},\
    /* 0008 - 000c  device Information Service*/   \
    {MAX_SERVICE_DEVICE_INFO,ATT_PERMISSIONS_READ_AUTHOR,2,2,2,(u8*)(&my_primaryServiceUUID),   (u8*)(&my_devServiceUUID), 0},\
    {0,ATT_PERMISSIONS_READ_AUTHOR,2,1,1,(u8*)(&my_characterUUID),        (u8*)(&my_PnPCharacter), 0},\
    {0,ATT_PERMISSIONS_READ_AUTHOR,2,sizeof (my_PnPtrs),sizeof (my_PnPtrs),(u8*)(&my_PnPUUID), (u8*)(my_PnPtrs), 0},\
	\
    {0,ATT_PERMISSIONS_READ_AUTHOR,2,1,1,(u8*)(&my_characterUUID),        (u8*)(&my_fwRevisionCharacter), 0},\
    {0,ATT_PERMISSIONS_READ_AUTHOR,2,FW_REVISION_VALUE_LEN,FW_REVISION_VALUE_LEN,(u8*)(&my_fwRevisionUUID), (u8*)(my_fwRevision_value), 0},\
    /* 000d - 0010  OTA*/   \
    {MAX_SERVICE_GATT_OTA,ATT_PERMISSIONS_READ_AUTHOR, 2,16,16,(u8*)(&my_primaryServiceUUID),     (u8*)(&my_OtaServiceUUID), 0},\
    {0,ATT_PERMISSIONS_READ_AUTHOR, 2, 1,1,(u8*)(&my_characterUUID),      (u8*)(&my_OtaProp), 0}, /*prop*/   \
    {0,ATT_PERMISSIONS_RDWD_AUTHOR,16,sizeof(my_OtaData),sizeof(my_OtaData),(u8*)(&my_OtaUUID),    (&my_OtaData), &otaWrite, &otaRead}, /*value*/   \
    {0,ATT_PERMISSIONS_READ_AUTHOR, 2,sizeof (my_OtaName),sizeof (my_OtaName),(u8*)(&userdesc_UUID), (u8*)(my_OtaName), 0},

#define MY_ATTRIBUTE_PB_GATT_CHAR           \
    {0,ATT_PERMISSIONS_READ_AUTHOR, 2, 1,1,(u8*)(&my_characterUUID),      (u8*)(&my_pb_gatt_out_prop), 0}, /*prop*/   \
    {0,ATT_PERMISSIONS_RDWD_AUTHOR, 2,sizeof(my_pb_gattOutData),sizeof(my_pb_gattOutData),(u8*)(&my_pb_gatt_out_UUID),    (my_pb_gattOutData), 0, 0}, /*value*/   \
    {0,ATT_PERMISSIONS_READ_AUTHOR, 2,sizeof (my_pb_gattOutName),sizeof (my_pb_gattOutName),(u8*)(&userdesc_UUID), (u8*)(my_pb_gattOutName), 0},\
    {0,ATT_PERMISSIONS_RDWD_AUTHOR, 2, sizeof(provision_Out_ccc),sizeof(provision_Out_ccc),(u8*)(&clientCharacterCfgUUID),    (u8*)(provision_Out_ccc), &pb_gatt_provision_out_ccc_cb,0}, /*value*/   \
	\
    {0,ATT_PERMISSIONS_READ_AUTHOR, 2, 1,1,(u8*)(&my_characterUUID),      (u8*)(&my_pb_gatt_in_prop), 0}, /*prop*/   \
    {0,ATT_PERMISSIONS_RDWD_AUTHOR, 2,sizeof(my_pb_gattInData),sizeof(my_pb_gattInData),(u8*)(&my_pb_gatt_in_UUID),  (my_pb_gattInData), &pb_gatt_Write, 0}, /*value*/   \
    {0,ATT_PERMISSIONS_READ_AUTHOR, 2,sizeof (my_pb_gattInName),sizeof (my_pb_gattInName),(u8*)(&userdesc_UUID), (u8*)(my_pb_gattInName), 0},\
    {0,ATT_PERMISSIONS_RDWD_AUTHOR, 2, sizeof(provision_In_ccc),sizeof(provision_In_ccc),(u8*)(&clientCharacterCfgUUID),     (u8*)(provision_In_ccc), 0}, /*value*/

#define MY_ATTRIBUTE_PROXY_GATT_CHAR           \
    {0,ATT_PERMISSIONS_READ_AUTHOR, 2, 1,1,(u8*)(&my_characterUUID),      (u8*)(&my_proxy_out_prop), 0}, /*prop*/   \
    {0,ATT_PERMISSIONS_RDWD_AUTHOR, 2,1,1,(u8*)(&my_proxy_out_UUID),  (my_proxy_outData), 0, 0}, /*value*/   \
    {0,ATT_PERMISSIONS_READ_AUTHOR, 2,sizeof (my_proxy_out_Name),sizeof (my_proxy_out_Name),(u8*)(&userdesc_UUID), (u8*)(my_proxy_out_Name), 0},\
    {0,ATT_PERMISSIONS_RDWD_AUTHOR, 2, sizeof(proxy_Out_ccc),sizeof(proxy_Out_ccc),(u8*)(&clientCharacterCfgUUID),    (u8*)(proxy_Out_ccc), &proxy_out_ccc_cb,0}, /*value*/   \
	\
    {0,ATT_PERMISSIONS_READ_AUTHOR, 2, 1,1,(u8*)(&my_characterUUID),      (u8*)(&my_proxy_in_prop), 0}, /*prop*/   \
    {0,ATT_PERMISSIONS_RDWD_AUTHOR, 2,1,1,(u8*)(&my_proxy_in_UUID),   (my_proxy_inData), &proxy_gatt_Write, 0}, /*value*/   \
    {0,ATT_PERMISSIONS_READ_AUTHOR, 2,sizeof (my_proxy_in_Name),sizeof (my_proxy_in_Name),(u8*)(&userdesc_UUID), (u8*)(my_proxy_in_Name), 0},\
    {0,ATT_PERMISSIONS_RDWD_AUTHOR, 2, sizeof(proxy_In_ccc),sizeof(proxy_In_ccc),(u8*)(&clientCharacterCfgUUID),     (u8*)(proxy_In_ccc), 0}, /*value*/   

#if USER_DEFINE_SET_CCC_ENABLE
#define MY_ATTRIBUTE_USER_DEFINE_SET_CCC           \
	{MAX_USER_DEFINE_SET_CCC_ATT_NUM,ATT_PERMISSIONS_READ_AUTHOR, 2,16,16,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_userdefine_service_UUID), 0},\
	{0,ATT_PERMISSIONS_READ_AUTHOR, 2, 1,1,(u8*)(&my_characterUUID),		(u8*)(&my_userdefine_prop), 0}, /*prop*/   \
	{0,ATT_PERMISSIONS_RDWD_AUTHOR,16,sizeof(my_userdefine_dat),sizeof(my_userdefine_dat),(u8*)(&my_userdefine_UUID), (&my_userdefine_dat), &pb_gatt_provision_out_ccc_cb, 0}, /*value*/   \
	{0,ATT_PERMISSIONS_READ_AUTHOR, 2,sizeof (my_userderdefine),sizeof (my_userderdefine),(u8*)(&userdesc_UUID), (u8*)(my_userderdefine), 0},
#endif


#define MY_ATTRIBUTE_SERVICE_CHANGE                        \
	{MAX_SERVICE_CHANGE_ATT_NUM,ATT_PERMISSIONS_READ_AUTHOR,2,2,2,(u8*)(&my_primaryServiceUUID),	(u8*)(&mi_gerneric_service), 0},\
	{0,ATT_PERMISSIONS_READ, 2, 1,1,(u8*)(&my_characterUUID), 	(u8*)(&mi_service_change_prop), 0}, /*prop*/   \
	{0,ATT_PERMISSIONS_READ, 2,sizeof(mi_service_change_buf),sizeof(mi_service_change_buf),(u8*)(&mi_service_change_uuid), (u8 *)(mi_service_change_buf), 0, 0}, /*value*/   \
	{0,ATT_PERMISSIONS_READ_AUTHOR, 2,sizeof (mi_service_change_str),sizeof (mi_service_change_str),(u8*)(&userdesc_UUID), (u8*)(mi_service_change_str), 0},	\
	{0,ATT_PERMISSIONS_RDWR, 2, sizeof(mi_service_change_ccc),sizeof(mi_service_change_ccc),(u8*)(&clientCharacterCfgUUID),	(u8*)(mi_service_change_ccc), 0}, /*value*/   


const attribute_t my_Attributes[] = {
	MY_ATTRIBUTE_BASE0

	
    /* 0011 - 0019      PB-GATT*/
    {9,ATT_PERMISSIONS_READ_AUTHOR, 2,2,2,(u8*)(&my_primaryServiceUUID),  (u8*)(&my_pb_gattUUID), 0},
    MY_ATTRIBUTE_PB_GATT_CHAR
    
    /* 001a - 0022  PROXY_GATT PART*/
    {9,ATT_PERMISSIONS_READ_AUTHOR, 2,2,2,(u8*)(&my_primaryServiceUUID),  (u8*)(&my_proxy_gattUUID), 0},
    MY_ATTRIBUTE_PROXY_GATT_CHAR

	HOMEKIT_TOP_ATT_TABLE
#if USER_DEFINE_SET_CCC_ENABLE
	// 0023 - 0026	userdefine 
	MY_ATTRIBUTE_USER_DEFINE_SET_CCC
#endif

    MY_ATTRIBUTE_SERVICE_CHANGE
};


u8 homekit_pair_start_handle = 0xff;
void get_homekit_pair_start_handle()
{
	foreach_arr(i, my_Attributes){
		if(!memcmp(pairingServiceUUID, my_Attributes[i].pAttrValue, 16)){
			homekit_pair_start_handle = i;
		}
	}
	return;
}

void my_att_init(u8 mode)
{
	u8 device_name[] = DEV_NAME;
	bls_att_setDeviceName(device_name, sizeof(DEV_NAME));
	get_homekit_pair_start_handle();
	
    bls_att_setAttributeTable ((u8 *)my_Attributes);
    u8 unused_gattUUID[2] = SIG_MESH_ATT_UNUSED;
    if(mode == GATT_PROVISION_MODE){
        u8 pb_gattUUID[2]=SIG_MESH_PROVISION_SERVICE;
        memcpy(my_pb_gattUUID, pb_gattUUID, sizeof(my_pb_gattUUID));
        memcpy(my_proxy_gattUUID, unused_gattUUID, sizeof(my_proxy_gattUUID));
    }else if(mode == GATT_PROXY_MODE){
        u8 proxy_gattUUID[2]= SIG_MESH_PROXY_SERVICE;
        memcpy(my_pb_gattUUID, unused_gattUUID, sizeof(my_pb_gattUUID));
        memcpy(my_proxy_gattUUID, proxy_gattUUID, sizeof(my_proxy_gattUUID));
    }
}

_attribute_ram_code_ u8 is_homekit_pair_handle(u8 handle)
{

	if((handle>= homekit_pair_start_handle) && (handle < homekit_pair_start_handle + my_Attributes[homekit_pair_start_handle].attNum)){
		return 1;
	}
	return 0;
}

u8 is_homekit_att_handle(u8 handle)
{
	if((handle>= ATT_NUM_START_HOMEKIT) && (handle < ATT_NUM_START_HOMEKIT + HOMEKIT_NO)){
		return 1;
	}

	return 0;
}
#endif

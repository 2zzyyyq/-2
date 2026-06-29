#include "ALL_Var.h"
#include <stdint.h>
void App_portCfg(void);
void App_UartCfg(void);
void r_uart0_service(void);
void data_handle(void);
void Model_Net_Now(void);
void Model_Net_Change(void);
void Judge_Nightlight_Mode(void);
unsigned char Queue_Read_Byte(void);
unsigned char get_queue_total_data(void);
void Invalid_Cleanup(uint8_t *i);
static uint8_t mcu_common_uart_data_unpack(uint8_t data);
static uint16_t UART_RX_Count = 0,Frame_length=0;
uint16_t set_position_pulse,set_position_pulse_B;
uint8_t   Net_state;
uint8_t  idx=0,offset_value;
uint8_t  Poweron_Set_Model_step=0,g_MiioRoundFlag=0,g_MiioErrorFlag=0;
uint8_t  _hour,_min;
uint32_t min_data_now,min_start_data_flash,min_stop_data_flash;
volatile uint8_t *queue_in= NULL;
volatile uint8_t *queue_out= NULL;
volatile char uart0_queue_buf[QUEUE_BUF_DEEP] = {0};
uint8_t uart0_rx_buf[180]= {0};//wifi接收串口数据
uint8_t uart0_tx_buf[180]= {0};//发送给wifi串口数据

volatile TIME_SLICE  Time;
PCBA_TEST Pcba_test;
LED Led ;
#define SET_MODEL_OK 11
#define PACK_SPEC_NUM 15
const char *Up_Motor_Properties[5]={"7 4 0 0 ","7 4 0 1 ","7 4 0 2 ","7 4 0 3 ","7 4 0 4 "};//
const char *Up_Motor_Properties_B[5]={"6 4 0 0 ","6 4 0 1 ","6 4 0 2 ","6 4 0 3 ","6 4 0 4 "};//
/**============= Miio Init ========================**/
const  char SentReast [8]         ="restore\r";                                 /* 模块重置 */
const  char enter [2]             ="\r\n";

#ifdef TESTADC		
const  char MCU_TEST[9]   ="MCU TEST\r";
const  char MCU_TEST_CLOSE[11]     ="MCU NOTEST\r";
#endif


const  char ModelCheck[6]         ="model\r";                                   /* 名称配置 */
const  char ModelCharCheck[19]    ="xiaomi.airer.0004\r";                        /* 名称配置 */

const  char MCUversion[17]        ="mcu_version 0008\r";                        /* 上报版本 */
const  char ModelChar[24]         ="model xiaomi.airer.0004\r";                 /* 名称配置 */
const  char ModelPid[26]          ="ble_config set 36240 0008\r";               /* 产品识别号 */
const  char MIIOuartarck[9]       ="echo off\r";                                /* 模块回显功能关 */
/**================================ Mcu Up ==============================**/
const  char MCUnet[4]             ="net\r";					/*询问网络状态*/
const  char DownInstruct[9]       ="get_down\r";                                /* 获取下行指令，间隔150ms*/
const  char getnettime[5]         ="time\r";					/*获取网络时间  间隔30s*/


const  char error5001[26]        ="error \"bytes limit\" -5001\r";              /* 返回错误 */
const  char error5002[20]        ="error \"error\" -5002\r";                    /* 返回错误 */

const  char setresult[7]         ="result ";					/*按不同的下行指令应答*/

const  char properties[18]       ="properties_changed";                          /*上报属性,1s间隔*/  
const  char upevent[13]          ="event_occured";                               /*上报事件,1s间隔*/        


/**=============================== Miio State ========================**/
const  char QueryMcuVersion[27]  ="down MIIO_mcu_version_req\r";                /* 版本查询 */
const  char Arcknone[11]         ="down none\r";                                /* 无命令下发*/
const  char ArckOk[4]            ="ok\r";                                       /* MIIO接收完成 */

const  char offline[9]           ="offline\r";                                  /* wifi连接中或掉线 */
const  char local[7]             ="local\r";                                    /* wifi连上路由器 */
const  char cloud[7]             ="cloud\r";                                    /* wifi连接上米家平台*/
const  char uap[5]               ="uap\r";                                      /* uap模式等待快连*/
const  char unprov[8]            ="unprov\r";                                   /* wifi删除于米家平台*/
const  char updating[10]          ="updating\r";                                /* wifi删除于米家平台*/

const  char UnknownCommand[22]   ="down unknown_command\r";                     /* 无用命令*/
const  char WIFIoffline[30]      ="down MIIO_net_change offline\r";             /* wifi连接中 */
const  char WIFIlocal[28]        ="down MIIO_net_change local\r";               /* wifi连上路由器 */
const  char WIFIcloud[28]        ="down MIIO_net_change cloud\r";               /* wifi连接上米家平台 */
const  char WIFIuap[26]          ="down MIIO_net_change uap\r";                 /* uap模式等待快连 */
const  char WIFIunprov[29]       ="down MIIO_net_change unprov\r";              /* wifi删除于米家平台 */
const  char WIFIupdating[31]     ="down MIIO_net_change updating\r";		/*正在下载更新程序*/

const  char error[7]             ="error\r";
const  char poweroff[15]         ="down power_off\r";

const  char SetProperties[21]    ="down set_properties\r";                       /* 打开功能*/
const  char ActionImplement[13]   ="down action\r";
const  char QueryOneState[21]    ="down get_properties\r";                       /* 查询单个属性状态*/
            
const  char bootloard[16]        ="down update_fw\r";
/**============================== 自动升级 ========================**/
const  char auto_on[20]          ="set_mcu_auto_ota on\r";                       /* 打开自动更新*/
const  char auto_off[21]         ="set_mcu_auto_ota off\r";                      /* 关闭自动更新*/
const  char autoupdate[20]       ="down update_fw auto\r";                       /* 自动更新*/
const  char forceupdate[21]      ="down update_fw force\r";                      /* 强制自动更新*/

const  char autoready[15]        ="result \"ready\"\r";                          /* 准备更新*/
const  char autobusy[14]         ="result \"busy\"\r";                           /* 不合适更新*/ 

void r_uart0_send_bytes(unsigned char *src_p,unsigned char dataNum)
{
	uint8_t i=0;
	for(i=0;i<dataNum;i++)
	{
				while(Uart_GetStatus(M0P_UART0,UartTxe)==FALSE)
					{    
						;
					}
						Uart_SendDataIt(M0P_UART0,*src_p++);         //调用库函数，通过UART0发送一个字母。

		send_busy=1;
	}
}

/*****************************************************************************
Function name:get_queue_total_data
Function description: read data in the queue
Input parameters: none
Return parameter: none
*****************************************************************************/
unsigned char get_queue_total_data(void)
{  
if(queue_in != queue_out)
    return 1;
  else
    return 0;
}


void r_uart0_receive_input(unsigned char value)
{
 if((queue_in > queue_out) && ((size_t)(queue_in - queue_out) >= sizeof(uart0_queue_buf)))
  {
	// the data queue is full
  }
  else if((queue_in < queue_out) && ((queue_out  - queue_in) == 0))
  {
    //Data queue is full
  }
  else
  {
    //Queue is not full
    if(queue_in >= (unsigned char *)(uart0_queue_buf + sizeof(uart0_queue_buf)))
    {
      queue_in = (unsigned char *)(uart0_queue_buf);
    }  
    *queue_in ++ = value;
  }
}

unsigned char Queue_Read_Byte(void)
{
  unsigned char value;
  
  if(queue_out != queue_in)
  {
    //Data is not empty
    if(queue_out >= (unsigned char *)(uart0_queue_buf + sizeof(uart0_queue_buf)))
    {
      //Data has reached the end
      queue_out = (unsigned char *)(uart0_queue_buf);
    }    
    value = *queue_out ++;   
  } 
  return value;
}
void r_uart0_service(void)
{ 
  if(get_queue_total_data() > 0)
  {
    if(mcu_common_uart_data_unpack(Queue_Read_Byte()))//收到一包数据
    {
	        data_handle();//解析数据包
          memset((char *)uart0_rx_buf,0,sizeof(uart0_rx_buf));
	        UART_RX_Count = 0;
			 
    }
  }
 }
void r_uart0_protocol_init(void)
{
  queue_in = (unsigned char *)uart0_queue_buf;
  queue_out = (unsigned char *)uart0_queue_buf;
}
static uint8_t mcu_common_uart_data_unpack(uint8_t data)
{
    uint8_t ret = FALSE;
      if(Time.Uart_Receive_Interval_time>=RECEIVE_DATA_PACKET_INTERVAL_TIME)//间隔20ms判定为下一包数据
     {
      	UART_RX_Count=0;
     }
    Time.Uart_Receive_Interval_time=0;
    uart0_rx_buf[UART_RX_Count]=data;
    if(uart0_rx_buf[UART_RX_Count]==0x0D)//结束符，一帧结束
    {
	    Frame_length=UART_RX_Count;
	    UART_RX_Count=0;
    	ret = TRUE;
    }
    UART_RX_Count++;
    if(UART_RX_Count >= sizeof(uart0_rx_buf)) 
    {
	    UART_RX_Count=0;
  	  Flag.Uart1_Err = 1;
	    ret = TRUE;
    }
    return ret;
}

/*****************************************************************************
处理接收到一帧wifi数据
根据命令执行功能
*****************************************************************************/
void data_handle(void)
{

  if(Flag.Enter_Pcba_test_flag)//自检
	{
		if (strcmp(ArckOk, (char *)uart0_rx_buf) == 0)	
		{
			Pcba_test.wifi_test_flag=1;
		}	
	}
	if(Poweron_Set_Model_step<SET_MODEL_OK)
	{
		//匹配新的mcu 固件协议检测
		if (strcmp(ArckOk, (char *)uart0_rx_buf) == 0) 
		{
			if(Poweron_Set_Model_step==4)
			{
				 Poweron_Set_Model_step+=2;
			}
			else
			{
				Poweron_Set_Model_step++;
			}
		}
		else if (strcmp(error, (char *)uart0_rx_buf) == 0) 
		{
			if(Poweron_Set_Model_step==4)//发送mode查询
			{
				 Poweron_Set_Model_step++;
			}
		}
	}
	else if(Poweron_Set_Model_step>=SET_MODEL_OK)
	{
		if(Flag.Uart1_Err == 1)
		{
			Flag.Uart1_Err = 0;
			memcpy((uint8_t *)uart0_tx_buf, error5001, sizeof(error5001));
			r_uart0_send_bytes((uint8_t *)uart0_tx_buf, sizeof(error5001));
		}
		 if ((strcmp(ArckOk, (char *)uart0_rx_buf) == 0) || (strcmp(Arcknone, (char *)uart0_rx_buf) == 0))   
		{
		//app通信正常，无命令下传
		}
		else
		{
			if ((strcmp(WIFIoffline, (char *)uart0_rx_buf) == 0) 
			 || (strcmp(WIFIuap, (char *)uart0_rx_buf) == 0)
			 || (strcmp(WIFIlocal, (char *)uart0_rx_buf) == 0)
			 || (strcmp(WIFIcloud, (char *)uart0_rx_buf) == 0)
			 || (strcmp(WIFIunprov, (char *)uart0_rx_buf) == 0)
			 || (strcmp(WIFIupdating, (char *)uart0_rx_buf) == 0))
			{
				Model_Net_Change();
			}
			else if((strcmp(offline, (char *)uart0_rx_buf) == 0) 
			|| (strcmp(uap, (char *)uart0_rx_buf) == 0)
			|| (strcmp(local, (char *)uart0_rx_buf) == 0) 
			|| (strcmp(cloud, (char *)uart0_rx_buf) == 0)
			|| (strcmp(unprov, (char *)uart0_rx_buf) == 0)
			|| (strcmp(updating, (char *)uart0_rx_buf) == 0))
			{
				Model_Net_Now();
			}
			else if ((strcmp(error, (char *)uart0_rx_buf) == 0) 
			|| (strcmp(poweroff, (char *)uart0_rx_buf) == 0)                                         
			|| (strcmp(UnknownCommand, (char *)uart0_rx_buf) == 0))
			{      
				if(!g_MiioErrorFlag) 
				{
					g_MiioErrorFlag=1;
					memcpy((uint8_t *)uart0_tx_buf, error5002, sizeof(error5002));
					r_uart0_send_bytes((uint8_t *)uart0_tx_buf, sizeof(error5002));
					if (strcmp(error, (char *)uart0_rx_buf) == 0) 
					{
						r_uart0_protocol_init();//清空缓存
					}
					memset((uint8_t *)&uart0_queue_buf,0,sizeof(uart0_queue_buf));
				}
			}
			else if (strcmp(bootloard, (char *)uart0_rx_buf) == 0) //在线升级
			{
				Flash_Data.Quite_updata=1;
				BootLoardDeal();//升级
			}
			else if ((strncmp(autoupdate, (char *)uart0_rx_buf, 19) == 0) || (strncmp(forceupdate, (char *)uart0_rx_buf, 19) == 0))//自动静默升级
			{
				Flash_Data.Quite_updata=0;
				BootLoardDeal();//自动更新
			}
			else if (strncmp(QueryMcuVersion, (char *)uart0_rx_buf, 18) == 0)//请求版本号	 
			{
				memcpy((uint8_t *)uart0_rx_buf, MCUversion, sizeof(MCUversion));
				r_uart0_send_bytes((uint8_t *)uart0_rx_buf, sizeof(MCUversion));//			
			} 
			else if (strncmp(QueryOneState, (char *)uart0_rx_buf, 18) == 0)	 
			{
				Return_Properties();//app查询属性回应	
			}
			else if (strncmp(SetProperties, (char *)uart0_rx_buf, 18) == 0) 
			{
				Set_Properties();//响应app操作指令及回应
			}
			else if (strncmp(ActionImplement, (char *)uart0_rx_buf, 11) == 0) 
			{
				Action_deal();
			}		
			else if((uart0_rx_buf[4]=='-')&&(uart0_rx_buf[7]=='-'))//返回网络时间例：2015-06-04 16:58:07
			{
				if(uart0_rx_buf[12]==':')
				{
					_hour =	(uart0_rx_buf[11]-0x30);	
					if(uart0_rx_buf[14]==':')
					{
						_min = (uart0_rx_buf[13]-0x30);	
					}
					else if(uart0_rx_buf[15]==':')
					{
						_min =	(uart0_rx_buf[14]-0x30)+(uart0_rx_buf[13]-0x30)*10;
					}
				}
				else if(uart0_rx_buf[13]==':')
				{
					_hour =	(uart0_rx_buf[12]-0x30)+(uart0_rx_buf[11]-0x30)*10;
					
					if(uart0_rx_buf[15]==':')
					{
						_min = (uart0_rx_buf[14]-0x30);	
					}
					else if(uart0_rx_buf[16]==':')
					{
						_min =	(uart0_rx_buf[15]-0x30)+(uart0_rx_buf[14]-0x30)*10;
					}
				}
				Judge_Nightlight_Mode();
				if(Flag.Brightness_time_flag)//夜灯
				{
					if(Flag.Brightness_change==0)
					{
						Device_State_Data.Light_Bright=Flash_Data.Light_Bright_Value;
					}
				}
				else
				{
					Device_State_Data.Light_Bright=Flash_Data.Max_Light_Value;
				}	
			}
		}
	}
	
		#ifdef TESTADC		
		 Poweron_Set_Model_step=6;	
		if (strncmp(MCU_TEST, (char *)uart0_rx_buf, sizeof(MCU_TEST)-1) == 0)
		{
			Flag.mcu_wifi_mode=1;					
		}
		else if(strncmp(MCU_TEST_CLOSE, (char *)uart0_rx_buf, sizeof(MCU_TEST_CLOSE)-1) == 0)
		{
			Flag.mcu_wifi_mode=0;		
		}

		#else  
 
		#endif
		memset((char *)uart0_rx_buf, 0, sizeof (uart0_rx_buf));
}


/***********************************************************************************************************************
* Function Name: Model_Net_Change
* Description  : Miio Net
* Arguments    : None
* Return Value : None
*指示灯工作状态,0：灭 1：闪烁状态：0.1s亮  0.4s灭 2：长亮状态
***********************************************************************************************************************/
void Model_Net_Change(void)
{ 
	if (strcmp(WIFIoffline, (char *)uart0_rx_buf) == 0)//wifi离线或连接中
	{
		if(Net_state!=NET_OFFLINE)
		{
			if((Net_state==NET_UAP)||(Net_state==NET_UNPROV))
			{	
			  M0P_ADTIM_TypeDef *ADTx= M0P_ADTIM6;
				Time.WIFI_enable_config_time=0;
				Led.Light_Led_state=3;
		   if(Device_State_Data.Light_Bright==30)//与正常照明状态亮度相同，恢复正常照明状态改变色温
				{
					colour_temperature_temp=0;//
				}
				else//与正常照明状态亮度不同，恢复正常照明状态改变亮度
				{
					brightness_temp=30;
				}
			
				Light_colour_temperature_gear=0;
				LIGHT_WHITE_VALUE=(uint16_t)((brightness_temp))*Light_colour_temperature_gear*LIGHT_TEM_RATIO;//
				LIGHT_YELLOW_VALUE=(uint16_t)(brightness_temp)*(LIGHT_MAX_STEP-Light_colour_temperature_gear)*LIGHT_TEM_RATIO;//	
			 	LED_LIGHT_ON;
 
			}
			
			Net_state=NET_OFFLINE;
		}
	}
	else if (strcmp(WIFIuap, (char *)uart0_rx_buf) == 0)//转变成快连状态
	{
	if(Net_state!=NET_UAP)
		{
			if(Flag.Reset_model==0)//删除设备
			{
				if(Net_state==NET_CLOUD)
				{
					SetupBzhx(WIFI_RESET_BEEP,Flash_Data.Beep_open);
					
					Flash_Data.Light_open_hour_time=23;
					Flash_Data.Light_open_min_time=0;
					Flash_Data.Light_close_hour_time=6;
					Flash_Data.Light_close_min_time=0;	
					
					Flash_Data.Light_Bright_Enable=0;
					Flag.Brightness_time_flag=0;
					Flash_Data.Light_Bright_Value=20;
					
					Device_State_Data.start_time_hour=Flash_Data.Light_open_hour_time;
					Device_State_Data.start_time_min=Flash_Data.Light_open_min_time;
					Device_State_Data.end_time_hour=Flash_Data.Light_close_hour_time;
					Device_State_Data.end_time_min=Flash_Data.Light_close_min_time;
					
					Device_State_Data.nightlight_switch=Flash_Data.Light_Bright_Enable;		
					Device_State_Data.brightness=Flash_Data.Light_Bright_Value;	
					
					Judge_Nightlight_Mode();
					if(Flag.Brightness_time_flag)//进入夜灯时间
					{
						Device_State_Data.Light_Bright=Flash_Data.Light_Bright_Value;
					}
					else
					{
						Device_State_Data.Light_Bright=Flash_Data.Max_Light_Value;
					}
				}
			}
			Flag.Reset_model=0;
			Net_state=NET_UAP;
		}
	}
	/*else if (strcmp(WIFIlocal, (char *)uart0_tx_buf) == 0)//连上路由器 
	{
		if(Net_state!=NET_LOCAL)
		{
			Net_state=NET_LOCAL;
			Led.Led_Orange_state=LED_ORG_STATE_OFF;
			Led.Led_Blue_state=LED_BLUE_STATE_TWINK;
			Led.Led_Blue_On_time=0;
		}
	}*/			
	/*else if (strcmp(WIFIcloud,(char *)uart0_tx_buf) == 0)//连上平台
	{			
		if(Net_state!=NET_CLOUD)//状态转换成连接成功
		{
			Net_state=NET_CLOUD;
			Led.Led_Orange_state=LED_ORG_STATE_OFF;
			Led.Led_Blue_state=LED_BLUE_STATE_ON;
			Led.Led_Blue_On_time=0;//3min时间变量清0
			Time._150ms_Count=156;//下一个时间片获取网络时间
			Flag.Reset_model=0;
			Led.Light_Led_state=0;
			if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
			{
				LED_LIGHT_OFF;
			}
		}
	}*/
	else if (strcmp(WIFIunprov, (char *)uart0_rx_buf) == 0)//
	{
		if(Net_state!=NET_UNPROV)
		{
			Net_state=NET_UNPROV;	
			/*Led.Light_Led_state=0;	
			Led.Led_Blue_state=LED_BLUE_STATE_OFF;
			Led.Led_Orange_state=LED_ORG_STATE_ON;
			Led.Led_Orange_On_time=0;*/	
		}
	}
}
void Judge_Nightlight_Mode(void)
{
	min_data_now=_hour*60+_min;
	min_start_data_flash = Flash_Data.Light_open_hour_time*60+Flash_Data.Light_open_min_time;
	min_stop_data_flash = Flash_Data.Light_close_hour_time*60+Flash_Data.Light_close_min_time;
	
	if(Flash_Data.Light_Bright_Enable==1)//夜灯模式
	{
		if((_hour<24)&&(_min<60))//时间正确
		{	
			if(min_start_data_flash<min_stop_data_flash)//当天
			{
				if((min_start_data_flash<=min_data_now)&&(min_data_now<min_stop_data_flash))
				{
					Flag.Brightness_time_flag=1;
				}
				else
				{
					Flag.Brightness_time_flag=0;
					Flag.Brightness_change=0;
				}
			}
			else if(min_start_data_flash>min_stop_data_flash)//非当天
			{
				if((min_data_now>=min_stop_data_flash)&&(min_data_now<min_start_data_flash))
				{
					Flag.Brightness_time_flag=0;
					Flag.Brightness_change=0;
				}
				else
				{
					Flag.Brightness_time_flag=1;
				}
			}
			else
			{
				if(min_data_now==min_start_data_flash)
				{
					Flag.Brightness_time_flag=1;
				}
				else
				{
					Flag.Brightness_time_flag=0;
					Flag.Brightness_change=0;
				}
			}
		}
	}
	else
	{
		Flag.Brightness_time_flag=0;
		Flag.Brightness_change=0;
	}
}

void Model_Net_Now(void)
{ 
	if (strcmp(offline, (char *)uart0_rx_buf) == 0)//wifi离线或连接中
	{
		/*if(Net_state!=NET_OFFLINE)
		{
			if(Net_state==NET_UAP)
			{
				Led.Light_Led_state=3;
				Time.WIFI_enable_config_time=0;
				
				Light_colour_temperature_gear=0;
				brightness_temp=30;
				LIGHT_WHITE_VALUE=(float)(brightness_temp)*Light_colour_temperature_gear*0.04;//
				LIGHT_YELLOW_VALUE=(float)(brightness_temp)*(LIGHT_MAX_STEP-Light_colour_temperature_gear)*0.04;//	
				LED_LIGHT_ON;
			}
			Net_state=NET_OFFLINE;
			Led.Led_Orange_state=LED_ORG_STATE_OFF;
			Led.Led_Blue_state=LED_BLUE_STATE_TWINK;
			Led.Led_Blue_On_time=0;
		}*/		
	}
	else if (strcmp(uap, (char *)uart0_rx_buf) == 0)//等待快连
	{	
		if(Net_state!=NET_UAP)
		{
			Net_state=NET_UAP;
			if(Flag.Reset_model==0)
			{
			
			}
			Flag.Reset_model=0;
		}
	}
	/*else if (strcmp(local, (char *)uart0_tx_buf) == 0)//连上路由器 
	{
		if(Net_state!=NET_LOCAL)
		{
			Net_state=NET_LOCAL;
			Led.Led_Blue_On_time=0;
			
			Led.Led_Orange_state=LED_ORG_STATE_OFF;
			Led.Led_Blue_state=LED_BLUE_STATE_TWINK;
		}
	}*/			
	else if (strcmp(cloud,(char *)uart0_rx_buf) == 0)//连上平台
	{			
		if(Net_state!=NET_CLOUD)//状态转换成连接成功
		{
			Net_state=NET_CLOUD;
			Time._150ms_Count=156;//下一个时间片获取网络时间
			Flag.Reset_model=0;
			Led.Light_Led_state=0;
			if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
			{
				LED_LIGHT_OFF;
			}
		}
	}
	else if (strcmp(unprov, (char *)uart0_rx_buf) == 0)//设备关闭配网状态
	{
		if(Net_state!=NET_UNPROV)
		{
			Net_state=NET_UNPROV;
			/*Flag.Reset_model=0;
			Led.Led_Blue_state=LED_BLUE_STATE_OFF;
			Led.Led_Orange_state=LED_ORG_STATE_ON;
			Led.Led_Orange_On_time=0;*/		
		}
	}
}
/*设置指令*/
void Action_deal(void)
{
	uint8_t i=0,offset=0;
	uint8_t Num=7;
	char *UpLoadBuf;
	memcpy((uint8_t *)uart0_tx_buf, setresult, 7);//上报命令字	
	while((12+i)<Frame_length)
	{
		offset=4;
		if((uart0_rx_buf[12+i]==0x33)&&(uart0_rx_buf[14+i]==0x31))
		{
			UpLoadBuf="3 1 0 ";
			memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 6);	
			Num+=6;
			Time.Light_key_space_time=Light_KEY_SPACE_TIME;
 		  Cmd_Light();
		}
		else if((uart0_rx_buf[12+i]==0x37)&&(uart0_rx_buf[14+i]==0x31))//设置A杆上限位
		{
			UpLoadBuf="7 1 0 ";
			memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 6);
			Num+=6;
			if(Ell_Data.Motor_Current_Position<=UP_POSITION_VALUE)
			{
				Device_State_Data.electronic_up_limit=0;
			}
			else
			{	
				Device_State_Data.electronic_up_limit=Device_State_Data.Motor_Position;
			}
			Flash_Data.Electronic_Up_Limit=Device_State_Data.electronic_up_limit;	
			if(Flash_Data.Electronic_Up_Limit==0)
			{
				set_up_limit_value=0;//电子上限位对应霍尔脉冲数
			}
			else
			{
				set_up_limit_value=Flash_Data.Electronic_Up_Limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;
			}
			if(Ell_Data.Motor_Current_Position<set_up_limit_value)
			{
				Device_State_Data.Motor_State_Byte=MOTOR_UP_STOP_STATE;
			}	
		}	
		else if((uart0_rx_buf[12]==0x36)&&(uart0_rx_buf[14]==0x31))//设置B杆上限位
		{
			UpLoadBuf="6 1 0 ";
			memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 6);
			Num+=6;	
			if(Ell_Data.B_Motor_Current_Position<=UP_POSITION_VALUE)
			{
				Device_State_Data.B_electronic_up_limit=0;
			}
			else
			{	
				Device_State_Data.B_electronic_up_limit=Device_State_Data.B_Motor_Position;
			}
			Flash_Data.B_electronic_up_limit=Device_State_Data.B_electronic_up_limit;	
			if(Flash_Data.B_electronic_up_limit==0)
			{
				B_set_up_limit_value=0;//电子上限位对应霍尔脉冲数
			}
			else
			{
				B_set_up_limit_value=Flash_Data.B_electronic_up_limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;
			}
			if(Ell_Data.B_Motor_Current_Position<B_set_up_limit_value)
			{
				Device_State_Data.B_Motor_State_Byte=MOTOR_UP_STOP_STATE;
			}	
		}
		else if((uart0_rx_buf[12]==0x37)&&(uart0_rx_buf[14]==0x32))//设置A杆下限位
		{
			UpLoadBuf="7 2 0 ";
			memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 6);
			Num+=6;
			if(Ell_Data.Motor_Current_Position>=DOWN_POSITION_VALUE)
			{
				Device_State_Data.electronic_down_limit=100;
			}
			else
			{
				Device_State_Data.electronic_down_limit=Device_State_Data.Motor_Position;
			}
			Flash_Data.Electronic_Down_Limit=Device_State_Data.electronic_down_limit;
			set_down_limit_value=Flash_Data.Electronic_Down_Limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;//电子下限位对应霍尔脉冲数		
			Device_State_Data.Motor_State_Byte=MOTOR_DOWN_STOP_STATE;		
		}
		else if((uart0_rx_buf[12]==0x36)&&(uart0_rx_buf[14]==0x32))//设置B杆下限位
		{
			UpLoadBuf="6 2 0 ";
			memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 6);
			Num+=6;
			if(Ell_Data.B_Motor_Current_Position>=DOWN_POSITION_VALUE)
			{
				Device_State_Data.B_electronic_down_limit=100;
			}
			else
			{
				Device_State_Data.B_electronic_down_limit=Device_State_Data.B_Motor_Position;
			}
			Flash_Data.B_electronic_down_limit=Device_State_Data.B_electronic_down_limit;
			B_set_down_limit_value=Flash_Data.B_electronic_down_limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;//电子下限位对应霍尔脉冲数		
			Device_State_Data.B_Motor_State_Byte=MOTOR_DOWN_STOP_STATE;		
		}
		else if((uart0_rx_buf[12]==0x38)&&(uart0_rx_buf[14]==0x31))//清码
		{
			UpLoadBuf="8 1 0 ";
			memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 6);
			Num+=6;
			// 整个结构体全部清 0
			//memset(&Capture_Command, 0, sizeof(Capture_Command));
			SetupBzhx(LONG_BEEP, 1);
			match_data_clear();  
	 
		 }else if ((uart0_rx_buf[12]==0x33)&&(uart0_rx_buf[14]==0x32))
		{
				UpLoadBuf="3 2 0\r";
				memcpy((uint8_t *)uart0_tx_buf + 7, UpLoadBuf, 6);
				Num+=6;
				if(uart0_rx_buf[16]==0x35)//夜灯开关
				{
					if(uart0_rx_buf[18]==0x66)//关
					{
						Flag.Brightness_time_flag=0;
						Device_State_Data.nightlight_switch=LIGHT_OFF;	
						Num=12;
					}
					else if(uart0_rx_buf[18]==0x74)
					{
						Flag.Brightness_time_flag=1;
						Device_State_Data.nightlight_switch=LIGHT_ON;
						Num=11;
					}	
				}
				if(uart0_rx_buf[12+Num]==0x36)//夜灯亮度
				{
					if((uart0_rx_buf[15+Num]==0x0d)||(uart0_rx_buf[15+Num]==0x20))
					{
						Device_State_Data.brightness=(uart0_rx_buf[14+Num]-0x30);	
					}
					else if((uart0_rx_buf[16+Num]==0x0d)||(uart0_rx_buf[16+Num]==0x20))
					{
						Device_State_Data.brightness=(uart0_rx_buf[14+Num]-0x30)*10+(uart0_rx_buf[15+Num]-0x30);
					}
					else if((uart0_rx_buf[17+Num]==0x0d)||(uart0_rx_buf[17+Num]==0x20))
					{
						Device_State_Data.brightness=(uart0_rx_buf[14+Num]-0x30)*100+(uart0_rx_buf[15+Num]-0x30)*10
											+uart0_rx_buf[16+Num]-0x30;
					}
					Flash_Data.Light_Bright_Value=Device_State_Data.brightness;			
				}
				Judge_Nightlight_Mode();
				if(Flag.Brightness_time_flag)//进入夜灯时间
				{
					Device_State_Data.Light_Bright=Flash_Data.Light_Bright_Value;
				}
				else
				{
					Device_State_Data.Light_Bright=Flash_Data.Max_Light_Value;
				}
				SetupBzhx(LONG_BEEP,Flash_Data.Beep_open);
			}
			{
			break;
			}
  if(Num>179)
	{
	
		uart0_tx_buf[Num-1] = 0x0d;
		r_uart0_send_bytes((uint8_t *)uart0_tx_buf, Num);	
   	return ;
	
	}

		i+=offset;
	}
	uart0_tx_buf[Num-1] = 0x0d;
	r_uart0_send_bytes((uint8_t *)uart0_tx_buf, Num);		
}

/***********************************************************************************************************************
* Function Name: XMModelSet
* Description  : Miio Init
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void PowerOn_ModelSet(void)
{	
	switch (Poweron_Set_Model_step)
	{
	//多发几次，
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		 memcpy((uint8_t *)uart0_tx_buf, ModelChar,sizeof(ModelChar) < sizeof(uart0_tx_buf) ? sizeof(ModelChar) : sizeof(uart0_tx_buf));
		 r_uart0_send_bytes((uint8_t *)uart0_tx_buf, sizeof(ModelChar)); //上报名称	
		 break;
	case 5:
		 memcpy((uint8_t *)uart0_tx_buf, ModelCheck, sizeof(ModelChar));
		 r_uart0_send_bytes((uint8_t *)uart0_tx_buf, sizeof(ModelChar)); //查询model	
		 break;		 
	case 6:
		 memcpy((uint8_t *)uart0_tx_buf, MCUversion, sizeof(MCUversion));
		 r_uart0_send_bytes((uint8_t *)uart0_tx_buf, sizeof(MCUversion));//上报版本
		 break;
	case 7:	
		 memcpy((uint8_t *)uart0_tx_buf, MIIOuartarck, sizeof(MIIOuartarck));//
		 r_uart0_send_bytes((uint8_t *)uart0_tx_buf, sizeof(MIIOuartarck));			 		
		 break;
	case 8:	
		 memcpy((uint8_t *)uart0_tx_buf, ModelPid, sizeof(ModelPid)); //产品识别号
		 r_uart0_send_bytes((uint8_t *)uart0_tx_buf, sizeof(ModelPid));		  
        	 break;
	case 9:
		 memcpy((uint8_t *)uart0_tx_buf, MCUversion, sizeof(MCUversion));
        	 r_uart0_send_bytes((uint8_t *)uart0_tx_buf, sizeof(MCUversion));//上报版本			 	
		 break;	
	case 10:
		 memcpy((uint8_t *)uart0_tx_buf, auto_on, sizeof(auto_on));//设置模组自动升级程序
       		 r_uart0_send_bytes((uint8_t *)uart0_tx_buf, sizeof(auto_on));
		 break;	
	default: break;
	}
}
/* 变量初始化 */
void variable_init(void)
{		
	  BUZ_OFF;
    BuzVcc_OFF; 
    current_state=0;
    Holl_Circle_num=0;
    Motor_Circle_num=0;
    Capture_Command.Capture_Pulse_Wide_Sequence=0;
	  Device_State_Data.Control_open=0;
    //Function_select.AD_sum=0;   
    memset((uint8_t *)&Time,0,sizeof(Time));
    Time.Light_key_space_time=Light_KEY_SPACE_TIME;//防止上电第一次开灯，带切换色温功能
    memset((uint8_t *)&Synchronous_Count_value,0,sizeof(Synchronous_Count_value));
    memset((uint8_t *)&Led,0,sizeof(Led));
    memset((uint8_t *)&Up_State_Data,0xff,sizeof(Up_State_Data));//   
    Flag.mcu_wifi_mode=0;
  // Function_select.Function=FUNCTION_L;
    Net_state=NET_OFFLINE;  
	  Device_State_Data.lightmode='4';
	  Device_State_Data.Control_num=Flash_Data.Remote_Control_Num;
    Device_State_Data.set_position=10;
    Device_State_Data.B_set_position=10;
    Device_State_Data.set_position_all=10;
}

void _150ms_work(void)
{	
	uint16_t  _data;
	if(Poweron_Set_Model_step>=SET_MODEL_OK)
	{
		if(g_MiioRoundFlag==1)//
		{
			g_MiioRoundFlag=0;
			memcpy((uint8_t *)uart0_tx_buf, DownInstruct, sizeof(DownInstruct));//获取下行指令
			r_uart0_send_bytes((uint8_t *)uart0_tx_buf, sizeof(DownInstruct));
		}
		else
		{
			Time._150ms_Count++;
			if(Time._150ms_Count%3==0)//更新一次位置信息
			{
				if((Ell_Data.Motor_Current_Position>=(DOWN_POSITION_VALUE))||(Flag.A_Down_limit))
				{
					Device_State_Data.Motor_Position=100;
				}
				else
				{
					if(Ell_Data.Motor_Current_Position<UP_POSITION_VALUE)
					{
						Device_State_Data.Motor_Position=0;
					}
					else
					{
						_data=(Ell_Data.Motor_Current_Position-UP_POSITION_VALUE)*1000.0/STROKE_TOTAL;//百分比先放大10倍
						//四舍五入
						if(_data%10>=5)
						{
							Device_State_Data.Motor_Position=_data/10+1;
						} 
						else
						{
							Device_State_Data.Motor_Position=_data/10;
						}
					}
				}
				if((Ell_Data.B_Motor_Current_Position>=(DOWN_POSITION_VALUE))||(Flag.B_Down_limit))
				{
					Device_State_Data.B_Motor_Position=100;
				}
				else
				{
					if(Ell_Data.B_Motor_Current_Position<UP_POSITION_VALUE)
					{
						Device_State_Data.B_Motor_Position=0;
					}
					else
					{
						_data=(Ell_Data.B_Motor_Current_Position-UP_POSITION_VALUE)*1000.0/STROKE_TOTAL;//百分比先放大10倍
						//四舍五入
						if(_data%10>=5)
						{
							Device_State_Data.B_Motor_Position=_data/10+1;
						} 
						else
						{
							Device_State_Data.B_Motor_Position=_data/10;
						}
					}
				}
			}
			if(Time._150ms_Count%25==0)//5s，读取一次模块联网状态
			{				
				memcpy((uint8_t *)uart0_tx_buf, MCUnet, sizeof(MCUnet));
				r_uart0_send_bytes((uint8_t *)uart0_tx_buf, sizeof(MCUnet)); 
				g_MiioRoundFlag=1;	
			}
			else if(Time._150ms_Count>=157)//30s,获取一次网络时间
			{
				Time._150ms_Count=0;
				if(Net_state==NET_CLOUD)
				{			
					memcpy((uint8_t *)uart0_tx_buf, getnettime, sizeof(getnettime));
					r_uart0_send_bytes((uint8_t *)uart0_tx_buf, sizeof(getnettime));  
					g_MiioRoundFlag=1;	
				}
			}
			else
			{	
				if(properties_up())//上报属性
				{
					g_MiioRoundFlag=1;
				}
				else
				{
					memcpy((uint8_t *)uart0_tx_buf, DownInstruct, sizeof(DownInstruct));//获取下行指令
					r_uart0_send_bytes((uint8_t *)uart0_tx_buf, sizeof(DownInstruct));
				}	
			}
		}
	}
	else
	{
		Time._150ms_Count++;
		if(Time._150ms_Count>=3)//更新一次位置信息
		{
			Time._150ms_Count=0;
			if((Ell_Data.Motor_Current_Position>=(DOWN_POSITION_VALUE))||(Flag.A_Down_limit))
			{
				Device_State_Data.Motor_Position=100;
			}
			else
			{
				if(Ell_Data.Motor_Current_Position<UP_POSITION_VALUE)
				{
					Device_State_Data.Motor_Position=0;
				}
				else
				{
					_data=(Ell_Data.Motor_Current_Position-UP_POSITION_VALUE)*1000.0/STROKE_TOTAL;//百分比先放大10倍
					//四舍五入
					if(_data%10>=5)
					{
						Device_State_Data.Motor_Position=_data/10+1;
					} 
					else
					{
						Device_State_Data.Motor_Position=_data/10;
					}
				}
			}
			if((Ell_Data.B_Motor_Current_Position>=(DOWN_POSITION_VALUE))||(Flag.B_Down_limit))
			{
				Device_State_Data.B_Motor_Position=100;
			}
			else
			{
				if(Ell_Data.B_Motor_Current_Position<UP_POSITION_VALUE)
				{
					Device_State_Data.B_Motor_Position=0;
				}
				else
				{
					_data=(Ell_Data.B_Motor_Current_Position-UP_POSITION_VALUE)*1000.0/STROKE_TOTAL;//百分比先放大10倍
					//四舍五入
					if(_data%10>=5)
					{
						Device_State_Data.B_Motor_Position=_data/10+1;
					} 
					else
					{
						Device_State_Data.B_Motor_Position=_data/10;
					}
				}
			}
		}	
	}
}

void BootLoardDeal(void)
{ 
 	MOTOR_DN_IO(0);//停止电机
	MOTOR_UP_IO(0);
	B_MOTOR_DN_IO(0);
	B_MOTOR_UP_IO(0);
	BuzVcc_OFF;
	BUZ_OFF;
	Flash_Data.Device_updata=2;//保存升级状态
	Flash_Data.Device_light_state=Device_State_Data.Light_State_Byte;//保存照明状态
	Flash_Data.Light_pwm_value=Device_State_Data.Light_Bright;//保存亮度百分比
	
	Flash_Data.Device_motor_state=Device_State_Data.Motor_State_Byte;//保存电机状态
	Flash_Data.B_Device_motor_state=Device_State_Data.B_Motor_State_Byte;//保存B电机状态
 
	 __disable_irq();

	LED_LIGHT_OFF;
	FDL_Block0_Save_Times=FDL_ONCE_MAX_WRITE_NUM-1;	
	Flash_WriteStruct(FDL_Block0+FDL_Block0_Save_Times*sizeof(Flash_Data),(uint8_t*)&Flash_Data,sizeof(Flash_Data));//写入一次	
	FDL_Block1_Save_Times=FDL_BLOCK1_ONCE_MAX_WRITE_NUM-1;
	Flash_WriteStruct(FDL_Block1+FDL_Block1_Save_Times*sizeof(Ell_Data),(uint8_t*)&Ell_Data,sizeof(Ell_Data));//块1写入一次	
  NVIC_SystemReset();
	while (1) 
	{
		
	}
}
uint8_t properties_up(void)//设备状态上报
{
	uint8_t _data[4];
	uint8_t ret = FALSE;
	char *UpLoad_Buf;
	uint8_t BufNum;
	uint8_t Property_num=0;
	if(Net_state!=NET_CLOUD)//
	{
		//return ret;
	}
	BufNum=18;
	memcpy((uint8_t *)uart0_tx_buf, properties, 18);
	//A,B故障属性
	if((Device_State_Data.Err_Byte_all&0x0f)!=(Up_State_Data.Err_Byte_all&0x0f))//
	{
		UpLoad_Buf = " 2 1 0";
		memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 6);
		BufNum += 6;

		Up_State_Data.Err_Byte_all=Device_State_Data.Err_Byte_all;
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}
	if(Device_State_Data.convergent_all!=Up_State_Data.convergent_all)
	{
		if(Device_State_Data.convergent_all==CONVERGENT_OFF)
		{
			UpLoad_Buf=" 2 3 false";//
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 10);
			BufNum += 10;		
		}
		else if(Device_State_Data.convergent_all==CONVERGENT_ON)
		{
			UpLoad_Buf=" 2 3 true";
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 9);
			BufNum += 9;			
		}	
		Up_State_Data.convergent_all=Device_State_Data.convergent_all;
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}	
	}

	//A故障属性
	if((Device_State_Data.Err_Byte&0x0f)!=(Up_State_Data.Err_Byte&0x0f))//
	{
		UpLoad_Buf = " 7 1 0";
		memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 6);
		BufNum += 6;
		if((Err.data&0x01)||(Err.data&0x08))//有遇阻
		{
			uart0_tx_buf[BufNum-1]=0x31;
		}
		else if(Err.data&0x02)//有超重
		{
			uart0_tx_buf[BufNum-1]=0x32;
		}
		else if(Err.data&0x04)//有电机堵转
		{
			uart0_tx_buf[BufNum-1]=0x33;
		}
		else
		{
			uart0_tx_buf[BufNum-1]=0x30;
		}
		Up_State_Data.Err_Byte=Device_State_Data.Err_Byte;
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}
	//B故障属性
	if((Device_State_Data.B_Err_Byte&0x0f)!=(Up_State_Data.B_Err_Byte&0x0f))//
	{
		UpLoad_Buf = " 6 1 0";
		memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 6);
		BufNum += 6;
		if((Err_B.data&0x01)||(Err_B.data&0x08))//有遇阻
		{
			uart0_tx_buf[BufNum-1]=0x31;
		}
		else if(Err_B.data&0x02)//有超重
		{
			uart0_tx_buf[BufNum-1]=0x32;
		}
		else if(Err_B.data&0x04)//有电机堵转
		{
			uart0_tx_buf[BufNum-1]=0x33;
		}
		else
		{
			uart0_tx_buf[BufNum-1]=0x30;
		}
		Up_State_Data.B_Err_Byte=Device_State_Data.B_Err_Byte;
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}
	//A晾杆状态
	if(Device_State_Data.Motor_State_Byte!=Up_State_Data.Motor_State_Byte)//
	{
		UpLoad_Buf = " 7 4 0";	
		memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 6);
		BufNum += 6;		
		uart0_tx_buf[BufNum - 1] = Device_State_Data.Motor_State_Byte+0x30;		
		
		Up_State_Data.Motor_State_Byte=Device_State_Data.Motor_State_Byte;	
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}
	//B晾杆状态
	if(Device_State_Data.B_Motor_State_Byte!=Up_State_Data.B_Motor_State_Byte)//
	{
		UpLoad_Buf = " 6 4 0";;	
		memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 6);
		BufNum += 6;		
		uart0_tx_buf[BufNum - 1] = Device_State_Data.B_Motor_State_Byte+0x30;		
		
		Up_State_Data.B_Motor_State_Byte=Device_State_Data.B_Motor_State_Byte;	
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}
	//A晾杆位置	
	if(Device_State_Data.Motor_Position!=Up_State_Data.Motor_Position)//
	{
		UpLoad_Buf = " 7 11 0";
		if(Device_State_Data.Motor_Position<10)
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 7;
			uart0_tx_buf[BufNum - 1] =Device_State_Data.Motor_Position+0x30;
		}
		else if(Device_State_Data.Motor_Position<100)
		{	
			_data[0]=Device_State_Data.Motor_Position/10;
			_data[1]=Device_State_Data.Motor_Position%10;
			
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 8;
			uart0_tx_buf[BufNum - 2] =_data[0]+0x30;
			uart0_tx_buf[BufNum - 1] =_data[1]+0x30;
		}
		else
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 9;
			uart0_tx_buf[BufNum - 3] =0x31;
			uart0_tx_buf[BufNum - 2] =0x30;
			uart0_tx_buf[BufNum - 1] =0x30;
		}		
		Up_State_Data.Motor_Position=Device_State_Data.Motor_Position;
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}
	//B晾杆位置	
	if(Device_State_Data.B_Motor_Position!=Up_State_Data.B_Motor_Position)//
	{
		UpLoad_Buf = " 6 11 0";
		if(Device_State_Data.B_Motor_Position<10)
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 7;
			uart0_tx_buf[BufNum - 1] =Device_State_Data.B_Motor_Position+0x30;
		}
		else if(Device_State_Data.B_Motor_Position<100)
		{	
			_data[0]=Device_State_Data.B_Motor_Position/10;
			_data[1]=Device_State_Data.B_Motor_Position%10;
			
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 8;
			uart0_tx_buf[BufNum - 2] =_data[0]+0x30;
			uart0_tx_buf[BufNum - 1] =_data[1]+0x30;
		}
		else
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 9;
			uart0_tx_buf[BufNum - 3] =0x31;
			uart0_tx_buf[BufNum - 2] =0x30;
			uart0_tx_buf[BufNum - 1] =0x30;
		}		
		Up_State_Data.B_Motor_Position=Device_State_Data.B_Motor_Position;
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}	
	}
	//设定A运行位置上报
	if(Device_State_Data.set_position!=Up_State_Data.set_position)//
	{
		UpLoad_Buf = " 7 12 0";
		if(Device_State_Data.set_position<10)
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 7;
			uart0_tx_buf[BufNum - 1] =Device_State_Data.set_position+0x30;
		}
		else if(Device_State_Data.set_position<100)
		{	
			_data[0]=Device_State_Data.set_position/10;
			_data[1]=Device_State_Data.set_position%10;
			
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 8;
			uart0_tx_buf[BufNum - 2] =_data[0]+0x30;
			uart0_tx_buf[BufNum - 1] =_data[1]+0x30;
		}
		else
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 8);
			BufNum += 9;
			uart0_tx_buf[BufNum - 3] =0x31;
			uart0_tx_buf[BufNum - 2] =0x30;
			uart0_tx_buf[BufNum - 1] =0x30;
		}	
	
		Up_State_Data.set_position=Device_State_Data.set_position;
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}			
	}
	//自动化设定运行B位置上报
	if(Device_State_Data.B_set_position!=Up_State_Data.B_set_position)//
	{
		UpLoad_Buf = " 6 12 0";
		if(Device_State_Data.B_set_position<10)
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 7;
			uart0_tx_buf[BufNum - 1] =Device_State_Data.B_set_position+0x30;
		}
		else if(Device_State_Data.B_set_position<100)
		{	
			_data[0]=Device_State_Data.B_set_position/10;
			_data[1]=Device_State_Data.B_set_position%10;
			
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 8;
			uart0_tx_buf[BufNum - 2] =_data[0]+0x30;
			uart0_tx_buf[BufNum - 1] =_data[1]+0x30;
		}
		else
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 9;
			uart0_tx_buf[BufNum - 3] =0x31;
			uart0_tx_buf[BufNum - 2] =0x30;
			uart0_tx_buf[BufNum - 1] =0x30;
		}	
	
		Up_State_Data.B_set_position=Device_State_Data.B_set_position;
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}			
	}
	//A杆电子上限位
	if(Device_State_Data.electronic_up_limit!=Up_State_Data.electronic_up_limit)
	{
		UpLoad_Buf = " 7 13 0";;
		if(Device_State_Data.electronic_up_limit<UP_L)
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 7;
			uart0_tx_buf[BufNum - 1] =Device_State_Data.electronic_up_limit+0x30;
		}
		else if(Device_State_Data.electronic_up_limit<100)
		{	
			_data[0]=Device_State_Data.electronic_up_limit/10;
			_data[1]=Device_State_Data.electronic_up_limit%10;
			
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 8;
			uart0_tx_buf[BufNum - 2] =_data[0]+0x30;
			uart0_tx_buf[BufNum - 1] =_data[1]+0x30;
		}
		else
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 9;
			uart0_tx_buf[BufNum - 3] =0x31;
			uart0_tx_buf[BufNum - 2] =0x30;
			uart0_tx_buf[BufNum - 1] =0x30;
		}
		Up_State_Data.electronic_up_limit=Device_State_Data.electronic_up_limit;
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}
	//B杆电子上限位
	if(Device_State_Data.B_electronic_up_limit!=Up_State_Data.B_electronic_up_limit)
	{
		UpLoad_Buf = " 6 13 0";
		if(Device_State_Data.B_electronic_up_limit<10)
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 7;
			uart0_tx_buf[BufNum - 1] =Device_State_Data.B_electronic_up_limit+0x30;
		}
		else if(Device_State_Data.B_electronic_up_limit<100)
		{	
			_data[0]=Device_State_Data.B_electronic_up_limit/10;
			_data[1]=Device_State_Data.B_electronic_up_limit%10;
			
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 8;
			uart0_tx_buf[BufNum - 2] =_data[0]+0x30;
			uart0_tx_buf[BufNum - 1] =_data[1]+0x30;
		}
		else
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 9;
			uart0_tx_buf[BufNum - 3] =0x31;
			uart0_tx_buf[BufNum - 2] =0x30;
			uart0_tx_buf[BufNum - 1] =0x30;
		}
		Up_State_Data.B_electronic_up_limit=Device_State_Data.B_electronic_up_limit;
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}
	//A杆电子下限位
	if(Device_State_Data.electronic_down_limit!=Up_State_Data.electronic_down_limit)
	{
		UpLoad_Buf = " 7 14 0";
		if(Device_State_Data.electronic_down_limit<10)
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 7;
			uart0_tx_buf[BufNum - 1] =Device_State_Data.electronic_down_limit+0x30;
		}
		else if(Device_State_Data.electronic_down_limit<100)
		{	
			_data[0]=Device_State_Data.electronic_down_limit/10;
			_data[1]=Device_State_Data.electronic_down_limit%10;
			
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 8;
			uart0_tx_buf[BufNum - 2] =_data[0]+0x30;
			uart0_tx_buf[BufNum - 1] =_data[1]+0x30;
		}
		else
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 9;
			uart0_tx_buf[BufNum - 3] =0x31;
			uart0_tx_buf[BufNum - 2] =0x30;
			uart0_tx_buf[BufNum - 1] =0x30;
		}
		Up_State_Data.electronic_down_limit=Device_State_Data.electronic_down_limit;
		ret = TRUE;	
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}
	//B杆电子下限位
	if(Device_State_Data.B_electronic_down_limit!=Up_State_Data.B_electronic_down_limit)
	{
		UpLoad_Buf = " 6 14 0";
		if(Device_State_Data.B_electronic_down_limit<10)
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 7;
			uart0_tx_buf[BufNum - 1] =Device_State_Data.B_electronic_down_limit+0x30;
		}
		else if(Device_State_Data.B_electronic_down_limit<100)
		{	
			_data[0]=Device_State_Data.B_electronic_down_limit/10;
			_data[1]=Device_State_Data.B_electronic_down_limit%10;
			
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 8;
			uart0_tx_buf[BufNum - 2] =_data[0]+0x30;
			uart0_tx_buf[BufNum - 1] =_data[1]+0x30;
		}
		else
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 9;
			uart0_tx_buf[BufNum - 3] =0x31;
			uart0_tx_buf[BufNum - 2] =0x30;
			uart0_tx_buf[BufNum - 1] =0x30;
		}
		Up_State_Data.B_electronic_down_limit=Device_State_Data.B_electronic_down_limit;
		ret = TRUE;	
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}
	//A杆收合开关状态
	if(Device_State_Data.convergent!=Up_State_Data.convergent)
	{
		if(Device_State_Data.convergent==CONVERGENT_OFF)
		{
			UpLoad_Buf=" 7 15 false";//
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 11);
			BufNum += 11;		
		}
		else if(Device_State_Data.convergent==CONVERGENT_ON)
		{
			UpLoad_Buf=" 7 15 true";
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 10);
			BufNum += 10;			
		}	
		Up_State_Data.convergent=Device_State_Data.convergent;
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}
	//B杆收合开关状态
	if(Device_State_Data.B_convergent!=Up_State_Data.B_convergent)
	{
		if(Device_State_Data.B_convergent==CONVERGENT_OFF)
		{
			UpLoad_Buf=" 6 15 false";//
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 11);
			BufNum += 11;		
		}
		else if(Device_State_Data.B_convergent==CONVERGENT_ON)
		{
			UpLoad_Buf=" 6 15 true";
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 10);
			BufNum += 10;			
		}	
		Up_State_Data.B_convergent=Device_State_Data.B_convergent;
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}
	//当前A速度
	if(Device_State_Data.current_speed!=Up_State_Data.current_speed)
	{
		UpLoad_Buf =  " 7 16 0";
		if(Device_State_Data.current_speed<10)
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 9;
			uart0_tx_buf[BufNum - 3] =0x30;
			uart0_tx_buf[BufNum - 2] ='.';
			uart0_tx_buf[BufNum - 1] =Device_State_Data.current_speed+0x30;
		}
		else if(Device_State_Data.current_speed<100)
		{	
			_data[0]=Device_State_Data.current_speed/10;
			_data[1]=Device_State_Data.current_speed%10;
			
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 9;
			uart0_tx_buf[BufNum - 3] =_data[0]+0x30;
			uart0_tx_buf[BufNum - 2] ='.';
			uart0_tx_buf[BufNum - 1] =_data[1]+0x30;
		}
		else
		{
			_data[0]=Device_State_Data.current_speed/100;
			_data[1]=Device_State_Data.current_speed%100/10;
			_data[2]=Device_State_Data.current_speed%10;
			
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 10;
			uart0_tx_buf[BufNum - 4] =_data[0]+0x30;
			uart0_tx_buf[BufNum - 3] =_data[1]+0x30;
			uart0_tx_buf[BufNum - 2] ='.';
			uart0_tx_buf[BufNum - 1] =_data[2]+0x30;
		}
		Up_State_Data.current_speed=Device_State_Data.current_speed;
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}
	//当前B速度
	if(Device_State_Data.B_current_speed!=Up_State_Data.B_current_speed)
	{
		UpLoad_Buf =  " 6 16 0";
		if(Device_State_Data.current_speed<10)
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 9;
			uart0_tx_buf[BufNum - 3] =0x30;
			uart0_tx_buf[BufNum - 2] ='.';
			uart0_tx_buf[BufNum - 1] =Device_State_Data.B_current_speed+0x30;
		}
		else if(Device_State_Data.B_current_speed<100)
		{	
			_data[0]=Device_State_Data.B_current_speed/10;
			_data[1]=Device_State_Data.B_current_speed%10;
			
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 9;
			uart0_tx_buf[BufNum - 3] =_data[0]+0x30;
			uart0_tx_buf[BufNum - 2] ='.';
			uart0_tx_buf[BufNum - 1] =_data[1]+0x30;
		}
		else
		{
			_data[0]=Device_State_Data.B_current_speed/100;
			_data[1]=Device_State_Data.B_current_speed%100/10;
			_data[2]=Device_State_Data.B_current_speed%10;
			
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 7);
			BufNum += 10;
			uart0_tx_buf[BufNum - 4] =_data[0]+0x30;
			uart0_tx_buf[BufNum - 3] =_data[1]+0x30;
			uart0_tx_buf[BufNum - 2] ='.';
			uart0_tx_buf[BufNum - 1] =_data[2]+0x30;
		}
		Up_State_Data.B_current_speed=Device_State_Data.B_current_speed;
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}


	//照明状态
	if(Device_State_Data.Light_State_Byte!=Up_State_Data.Light_State_Byte)//
	{
		if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
		{
			UpLoad_Buf=" 3 1 false";//
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 10);
			BufNum += 10;		
		}
		else if(Device_State_Data.Light_State_Byte==LIGHT_ON)
		{
			UpLoad_Buf=" 3 1 true";//
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 9);
			BufNum += 9;			
		}	
		Up_State_Data.Light_State_Byte=Device_State_Data.Light_State_Byte;
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}
		//情景模式
	if(Device_State_Data.lightmode!=Up_State_Data.lightmode)//
	{
	
	  	UpLoad_Buf=" 3 4 0";
			 memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 6);
			BufNum += 6;	
		
  	  uart0_tx_buf[BufNum - 1] = (Device_State_Data.lightmode);

			Up_State_Data.lightmode=Device_State_Data.lightmode;
			ret = TRUE;
			Property_num++;
			if(Property_num==PACK_SPEC_NUM)
			{
				goto send_data;
			}
	}
	//照明亮度
	if(Device_State_Data.Light_Bright!=Up_State_Data.Light_Bright)//
	{
		UpLoad_Buf =" 3 2 0";
		if(Device_State_Data.Light_Bright<10)
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 6);
			BufNum += 6;
			uart0_tx_buf[BufNum - 1] =Device_State_Data.Light_Bright+0x30;
		}
		else if(Device_State_Data.Light_Bright<100)
		{	
			_data[0]=Device_State_Data.Light_Bright/10;
			_data[1]=Device_State_Data.Light_Bright%10;
			
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 6);
			BufNum += 7;
			uart0_tx_buf[BufNum - 2] =_data[0]+0x30;
			uart0_tx_buf[BufNum - 1] =_data[1]+0x30;
		}
		else
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 6);
			BufNum += 8;
			uart0_tx_buf[BufNum - 3] =0x31;
			uart0_tx_buf[BufNum - 2] =0x30;
			uart0_tx_buf[BufNum - 1] =0x30;
		}	
		Up_State_Data.Light_Bright=Device_State_Data.Light_Bright;
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}
	//照明色温
	if(Device_State_Data.Light_colour_temperature!=Up_State_Data.Light_colour_temperature)//
	{
		UpLoad_Buf = " 3 3 0";
		if(Device_State_Data.Light_colour_temperature<10)
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 6);
			BufNum += 6;
			uart0_tx_buf[BufNum - 1] =Device_State_Data.Light_colour_temperature+0x30;
		}
		else if(Device_State_Data.Light_colour_temperature<100)
		{	
			_data[0]=Device_State_Data.Light_colour_temperature/10;
			_data[1]=Device_State_Data.Light_colour_temperature%10;
			
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 6);
			BufNum += 7;
			uart0_tx_buf[BufNum - 2] =_data[0]+0x30;
			uart0_tx_buf[BufNum - 1] =_data[1]+0x30;
		}
		else if(Device_State_Data.Light_colour_temperature<1000)
		{
			_data[0]=Device_State_Data.Light_colour_temperature/100;
			_data[1]=Device_State_Data.Light_colour_temperature%100/10;
			_data[2]=Device_State_Data.Light_colour_temperature%10;
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 6);
			BufNum += 8;
			uart0_tx_buf[BufNum - 3] =_data[0]+0x30;
			uart0_tx_buf[BufNum - 2] =_data[1]+0x30;
			uart0_tx_buf[BufNum - 1] =_data[2]+0x30;
		}	
		else if(Device_State_Data.Light_colour_temperature<10000)
		{
			_data[0]=Device_State_Data.Light_colour_temperature/1000;
			_data[1]=Device_State_Data.Light_colour_temperature%1000/100;	
			_data[2]=Device_State_Data.Light_colour_temperature%100/10;	
			_data[3]=Device_State_Data.Light_colour_temperature%10;
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 6);
			BufNum += 9;
			uart0_tx_buf[BufNum - 4] =0x30+_data[0];
			uart0_tx_buf[BufNum - 3] =0x30+_data[1];
			uart0_tx_buf[BufNum - 2] =0x30+_data[2];
			uart0_tx_buf[BufNum - 1] =0x30+_data[3];
		}
		Up_State_Data.Light_colour_temperature=Device_State_Data.Light_colour_temperature;
		ret = TRUE;
	}

	//夜灯开关
	if(Device_State_Data.nightlight_switch!=Up_State_Data.nightlight_switch)//
	{
		if(Device_State_Data.nightlight_switch==LIGHT_OFF)
		{
			UpLoad_Buf=" 3 5 false";//
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 10);
			BufNum += 10;		
		}
		else if(Device_State_Data.nightlight_switch==LIGHT_ON)
		{
			UpLoad_Buf=" 3 5 true";//
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 9);
			BufNum += 9;			
		}	
		Up_State_Data.nightlight_switch=Device_State_Data.nightlight_switch;
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}	
	//夜灯亮度
	if(Device_State_Data.brightness!=Up_State_Data.brightness)//
	{
		UpLoad_Buf =" 3 6 0";
		if(Device_State_Data.brightness<10)
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 6);
			BufNum += 6;
			uart0_tx_buf[BufNum - 1] =Device_State_Data.brightness+0x30;
		}
		else if(Device_State_Data.brightness<100)
		{	
			_data[0]=Device_State_Data.brightness/10;
			_data[1]=Device_State_Data.brightness%10;
			
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 6);
			BufNum += 7;
			uart0_tx_buf[BufNum - 2] =_data[0]+0x30;
			uart0_tx_buf[BufNum - 1] =_data[1]+0x30;
		}
		else
		{
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 6);
			BufNum += 8;
			uart0_tx_buf[BufNum - 3] =0x31;
			uart0_tx_buf[BufNum - 2] =0x30;
			uart0_tx_buf[BufNum - 1] =0x30;
		}	
		Up_State_Data.brightness=Device_State_Data.brightness;
		ret = TRUE;
		Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}
	
		if((Device_State_Data.start_time_hour!=Up_State_Data.start_time_hour)||
	   (Device_State_Data.start_time_min!=Up_State_Data.start_time_min)||
	   (Device_State_Data.end_time_hour!=Up_State_Data.end_time_hour)||	
	   (Device_State_Data.end_time_min!=Up_State_Data.end_time_min))
	{
		UpLoad_Buf = " 3 7 \"00:00:00-00:00:00\"";
		memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 26);
		BufNum += 24;
		uart0_tx_buf[BufNum-18]=Device_State_Data.start_time_hour/10+0x30;
		uart0_tx_buf[BufNum-17]=Device_State_Data.start_time_hour%10+0x30;
		
		uart0_tx_buf[BufNum-15]=Device_State_Data.start_time_min/10+0x30;
		uart0_tx_buf[BufNum-14]=Device_State_Data.start_time_min%10+0x30;
		
		uart0_tx_buf[BufNum-9]=Device_State_Data.end_time_hour/10+0x30;
		uart0_tx_buf[BufNum-8]=Device_State_Data.end_time_hour%10+0x30;
		
		uart0_tx_buf[BufNum-6]=Device_State_Data.end_time_min/10+0x30;
		uart0_tx_buf[BufNum-5]=Device_State_Data.end_time_min%10+0x30;				
                
		Up_State_Data.start_time_hour=Device_State_Data.start_time_hour;
		Up_State_Data.start_time_min=Device_State_Data.start_time_min;
		Up_State_Data.end_time_hour=Device_State_Data.end_time_hour;	
		Up_State_Data.end_time_min= Device_State_Data.end_time_min;
		
		ret = TRUE;
	  Property_num++;
	  if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}
	

	//提示音开关
	if(Device_State_Data.Beep_Enable!=Up_State_Data.Beep_Enable)//
	{	
		if(Device_State_Data.Beep_Enable==BEEP_OFF)
		{
			UpLoad_Buf=" 5 1 false";//
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 10);
			BufNum += 10;		
		}
		else if(Device_State_Data.Beep_Enable==BEEP_ON)
		{
			UpLoad_Buf=" 5 1 true";//
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 9);
			BufNum += 9;			
		}
		Up_State_Data.Beep_Enable=Device_State_Data.Beep_Enable;
		ret = TRUE;		
		 Property_num++;
		if(Property_num==PACK_SPEC_NUM)
		{
			goto send_data;
		}
	}
	if(Device_State_Data.Control_open!=Up_State_Data.Control_open)//
	{
 
		if(Device_State_Data.Control_open==0)
		{
			UpLoad_Buf=" 8 1 false";//
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 10);
			BufNum += 10;		
		}
		else if(Device_State_Data.Control_open==1)
		{
			UpLoad_Buf=" 8 1 true";//
			memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoad_Buf, 9);
			BufNum += 9;			
		}
		Up_State_Data.Control_open=Device_State_Data.Control_open;
		ret = TRUE;		
	}
	if(Device_State_Data.Control_num!=Up_State_Data.Control_num)
	{
				char UpLoadBuf1[90];
				memset(UpLoadBuf1, 0, sizeof(UpLoadBuf1));
				switch(Flash_Data.Remote_Control_Num)	
				{
				case 0: 
					snprintf(UpLoadBuf1,
					sizeof(UpLoadBuf1)-1,
					" 8 3 \"%02X:%02X:%02X:%02X\"",
					(0xFF),
					(0xFF),
					(0xFF),
					(0xFF));
				break; 
					
				case 1: 
				snprintf(UpLoadBuf1,
						 sizeof(UpLoadBuf1)-1,
						 " 8 3 \"%02X:%02X:%02X:%02X\"",
						 (unsigned int)((Flash_Data.Remote_Control_Id[0] >> 24) & 0xFF),
						 (unsigned int)((Flash_Data.Remote_Control_Id[0] >> 16) & 0xFF),
						 (unsigned int)((Flash_Data.Remote_Control_Id[0] >> 8) & 0xFF),
						 (unsigned int)(Flash_Data.Remote_Control_Id[0] & 0xFF));
				break; 

				case 2:
				snprintf(UpLoadBuf1,
						 sizeof(UpLoadBuf1)-1,
						 " 8 3 \"%02X:%02X:%02X:%02X,%02X:%02X:%02X:%02X\"",
						 (unsigned int)((Flash_Data.Remote_Control_Id[0] >> 24) & 0xFF),
						 (unsigned int)((Flash_Data.Remote_Control_Id[0] >> 16) & 0xFF),
						 (unsigned int)((Flash_Data.Remote_Control_Id[0] >> 8) & 0xFF),
						 (unsigned int)(Flash_Data.Remote_Control_Id[0] & 0xFF),
						 (unsigned int)((Flash_Data.Remote_Control_Id[1] >> 24) & 0xFF),
						 (unsigned int)((Flash_Data.Remote_Control_Id[1] >> 16) & 0xFF),
						 (unsigned int)((Flash_Data.Remote_Control_Id[1] >> 8) & 0xFF),
						 (unsigned int)(Flash_Data.Remote_Control_Id[1] & 0xFF));
				break;

				case 3: 
				snprintf(UpLoadBuf1,
						 sizeof(UpLoadBuf1)-1,
						 " 8 3 \"%02X:%02X:%02X:%02X,%02X:%02X:%02X:%02X,%02X:%02X:%02X:%02X\"",
						 (unsigned int)((Flash_Data.Remote_Control_Id[0] >> 24) & 0xFF),
						 (unsigned int)((Flash_Data.Remote_Control_Id[0] >> 16) & 0xFF),
						 (unsigned int)((Flash_Data.Remote_Control_Id[0] >> 8) & 0xFF),
						 (unsigned int)(Flash_Data.Remote_Control_Id[0] & 0xFF),
						 (unsigned int)((Flash_Data.Remote_Control_Id[1] >> 24) & 0xFF),
						 (unsigned int)((Flash_Data.Remote_Control_Id[1] >> 16) & 0xFF),
						 (unsigned int)((Flash_Data.Remote_Control_Id[1] >> 8) & 0xFF),
						 (unsigned int)(Flash_Data.Remote_Control_Id[1] & 0xFF),
						 (unsigned int)((Flash_Data.Remote_Control_Id[2] >> 24) & 0xFF),
						 (unsigned int)((Flash_Data.Remote_Control_Id[2] >> 16) & 0xFF),
						 (unsigned int)((Flash_Data.Remote_Control_Id[2] >> 8) & 0xFF),
						 (unsigned int)(Flash_Data.Remote_Control_Id[2] & 0xFF));
				break;

				default:
				snprintf(UpLoadBuf1,
					sizeof(UpLoadBuf1)-1,
					" 8 3 \"%02X:%02X:%02X:%02X\"",
					(0xFF),
					(0xFF),
					(0xFF),
					(0xFF));
				break;
				}
					Up_State_Data.Control_num=Device_State_Data.Control_num;
					memcpy((uint8_t *)uart0_tx_buf + BufNum, UpLoadBuf1, strlen(UpLoadBuf1));
					BufNum += strlen(UpLoadBuf1);
					ret = TRUE;							
					Property_num++;
					if(Property_num==PACK_SPEC_NUM)
					{
						 goto send_data;
					}	
	}
 
send_data:
	if(ret==TRUE)
	{
		uart0_tx_buf[BufNum] = 0x0d;
		r_uart0_send_bytes((uint8_t *)uart0_tx_buf, BufNum + 1); 
	}
	return ret;
}


void ModuleResetReq(void)//重置模块
{  	
	Time.WIFI_enable_config_time=0;
	Flag.Reset_model=1;
	//出产默认参数
	Flash_Data.Max_Light_Value=100;
	Flash_Data.Light_CT_value=5700;
	Device_State_Data.Light_colour_temperature=5700;
	 colour_temperature_temp=(Flash_Data.Light_CT_value-2700)/5;
	if((Flash_Data.Electronic_Down_Limit<100)&&(Device_State_Data.Motor_State_Byte==MOTOR_DOWN_STOP_STATE))
	{
		Device_State_Data.Motor_State_Byte=MOTOR_SUSPEND_STATE;	
	}
	Flash_Data.Electronic_Down_Limit=100;
		
	if((Flash_Data.B_electronic_down_limit<100)&&(Device_State_Data.B_Motor_State_Byte==MOTOR_DOWN_STOP_STATE))
	{
		Device_State_Data.B_Motor_State_Byte=MOTOR_SUSPEND_STATE;	
	}
	Flash_Data.B_electronic_down_limit=100;	
	
	if((Flash_Data.Electronic_Up_Limit>UP_L)&&(Device_State_Data.Motor_State_Byte==MOTOR_UP_STOP_STATE))
	{
		Device_State_Data.Motor_State_Byte=MOTOR_SUSPEND_STATE;	
	}	
	Flash_Data.Electronic_Up_Limit=UP_L;
	
	if((Flash_Data.B_electronic_up_limit>UP_L)&&(Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STOP_STATE))
	{
		Device_State_Data.B_Motor_State_Byte=MOTOR_SUSPEND_STATE;	
	}	
  Flash_Data.B_electronic_up_limit=UP_L;
	
	Flash_Data.Light_open_hour_time=23;
	Flash_Data.Light_open_min_time=0;
	Flash_Data.Light_close_hour_time=6;
	Flash_Data.Light_close_min_time=0;	
	
	Flash_Data.Light_Bright_Enable=0;
	Flag.Brightness_time_flag=0;
	Flash_Data.Light_Bright_Value=20;
	Flash_Data.Beep_open=1;
	
	set_down_limit_value=Flash_Data.Electronic_Down_Limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;//电子下限位对应霍尔脉冲数
	B_set_down_limit_value=Flash_Data.B_electronic_down_limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;
	if(Flash_Data.Electronic_Up_Limit==0)
	{
		set_up_limit_value=Flash_Data.Electronic_Up_Limit*ONE_PERCENT_PULSE;//电子上限位对应霍尔脉冲数	
	}
	else
	{
		set_up_limit_value=Flash_Data.Electronic_Up_Limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;
	}	
	if(Flash_Data.B_electronic_up_limit==0)
	{
		B_set_up_limit_value=Flash_Data.B_electronic_up_limit*ONE_PERCENT_PULSE;//电子上限位对应霍尔脉冲数	
	}
	else
	{
		B_set_up_limit_value=Flash_Data.B_electronic_up_limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;
	}
 
	save_data();//恢复默认值
	Device_State_Data.Light_Bright=Flash_Data.Max_Light_Value;
	
	Device_State_Data.electronic_down_limit=Flash_Data.Electronic_Down_Limit;
	Device_State_Data.B_electronic_down_limit=Flash_Data.B_electronic_down_limit;
	Device_State_Data.electronic_up_limit=Flash_Data.Electronic_Up_Limit;
	Device_State_Data.B_electronic_up_limit=Flash_Data.B_electronic_up_limit;
		
	
	Device_State_Data.start_time_hour=Flash_Data.Light_open_hour_time;
	Device_State_Data.start_time_min=Flash_Data.Light_open_min_time;
	Device_State_Data.end_time_hour=Flash_Data.Light_close_hour_time;
	Device_State_Data.end_time_min=Flash_Data.Light_close_min_time;
	
	Device_State_Data.nightlight_switch=Flash_Data.Light_Bright_Enable;
	Device_State_Data.brightness=Flash_Data.Light_Bright_Value;
	
	Device_State_Data.Beep_Enable=Flash_Data.Beep_open;	
	
 	properties_up();//上报恢复值
 	properties_up();//上报恢复值
 	properties_up();//上报恢复值
	memcpy((uint8_t *)uart0_tx_buf, SentReast, sizeof(SentReast));
	r_uart0_send_bytes((uint8_t *)uart0_tx_buf, sizeof(SentReast));                
}


void Return_Properties(void)//回应app查询属性
{
	uint8_t _data[4];
	uint8_t Num=7;
	char *UpLoadBuf;
	uint8_t value;
	memcpy((uint8_t *)uart0_tx_buf, setresult, 7);//上报命令字	
	for(idx=0;idx<Frame_length-21;idx=idx+offset_value)
  {
		if(uart0_rx_buf[20+idx] == 0x32)
		{
      offset_value=4;
		if((uart0_rx_buf[22+idx]==0x31)&&(uart0_rx_buf[23+idx]==0x32))////ppid>10运行到指定位置
			{

			UpLoadBuf ="2 12 0 15 ";
			memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 9);
			Num += 10;
		//	uart0_tx_buf[Num - 3] =_data[0]+0x30;
			//uart0_tx_buf[Num - 2] =_data[1]+0x30;
			uart0_tx_buf[Num - 1]=0x20;
				
			}else 		if((uart0_rx_buf[22+idx]==0x32)&&(uart0_rx_buf[23+idx]==0x30))////ppid>10运行到指定位置
			{

				UpLoadBuf ="2 20 0 0 ";
				memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 9);
				Num += 9;
				uart0_tx_buf[Num - 1]=0x20;
			
			}
      else 	if (uart0_rx_buf[22+idx] == 0x31)//查询A、B杆故障
			{
				UpLoadBuf ="2 1 0 0 ";
				memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 8);
				Num += 8;
				uart0_tx_buf[Num - 1]=0x20;
			}
			else if (uart0_rx_buf[22+idx] == 0x33)//查询双杆收合开关
			{
				if(Device_State_Data.convergent_all==CONVERGENT_OFF)
				{
					UpLoadBuf="2 3 0 false ";//
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 12);
					Num += 12;		
				}
				else if(Device_State_Data.convergent_all==CONVERGENT_ON)
				{
					UpLoadBuf="2 3 0 true ";
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 11);
					Num += 11;			
				}
			}
			else
			{
				break;
			}
		}
		else if(uart0_rx_buf[20+idx] == 0x35)
		{
			offset_value=4;
			if(uart0_rx_buf[22+idx] == 0x31)//查询提示音灯状态
			{
				if (Device_State_Data.Beep_Enable==BEEP_ON)
				{
					UpLoadBuf = "5 1 0 true ";
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 11);
					Num += 11;		
				}
				else 
				{
					UpLoadBuf = "5 1 0 false ";
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 12);
					Num += 12;								
				}	
			}
			else
			{
				break;
			}
		}
		else if (uart0_rx_buf[20+idx] == 0x33)//LIGHT
		{	
			offset_value=4;
			if(uart0_rx_buf[22+idx] == 0x31)//查询照明灯状态
			{
				if (Device_State_Data.Light_State_Byte==LIGHT_ON)
				{
					UpLoadBuf = "3 1 0 true ";
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 11);
					Num += 11;		
				}
				else 
				{
					UpLoadBuf = "3 1 0 false ";
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 12);
					Num += 12;								
				}	
			}
			else if(uart0_rx_buf[22+idx] == 0x32)//查询当前亮度
			{
				UpLoadBuf = "3 2 0 0 ";
				memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 8);
				if(Device_State_Data.Light_Bright<10)
				{
					Num += 8;
					uart0_tx_buf[Num - 2] =Device_State_Data.Light_Bright+0x30;
					uart0_tx_buf[Num - 1]=0x20;
				}
				else if(Device_State_Data.Light_Bright<100)
				{	
					_data[0]=Device_State_Data.Light_Bright/10;
					_data[1]=Device_State_Data.Light_Bright%10;	
					Num += 9;
					uart0_tx_buf[Num - 3] =_data[0]+0x30;
					uart0_tx_buf[Num - 2] =_data[1]+0x30;
					uart0_tx_buf[Num - 1]=0x20;
				}
				else
				{
					Num += 10;
					uart0_tx_buf[Num - 4] =0x31;
					uart0_tx_buf[Num - 3] =0x30;
					uart0_tx_buf[Num - 2] =0x30;
					uart0_tx_buf[Num - 1]=0x20;
				}
			}					
			else if(uart0_rx_buf[22+idx] == 0x35)//查询照明夜灯开关状态
			{
				if (Device_State_Data.nightlight_switch==LIGHT_ON)
				{
					UpLoadBuf = "3 5 0 true ";
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 11);
					Num += 11;		
				}
				else 
				{
					UpLoadBuf = "3 5 0 false ";
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 12);
					Num += 12;								
				}	
			}	
			else if(uart0_rx_buf[22+idx] == 0x36)//查询当前亮度
			{
				UpLoadBuf = "3 6 0 0 ";
				memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 8);
				if(Device_State_Data.brightness<10)
				{
					Num += 8;
					uart0_tx_buf[Num - 2] =Device_State_Data.brightness+0x30;
					uart0_tx_buf[Num - 1]=0x20;
				}
				else if(Device_State_Data.brightness<100)
				{	
					_data[0]=Device_State_Data.brightness/10;
					_data[1]=Device_State_Data.brightness%10;	
					Num += 9;
					uart0_tx_buf[Num - 3] =_data[0]+0x30;
					uart0_tx_buf[Num - 2] =_data[1]+0x30;
					uart0_tx_buf[Num - 1]=0x20;
				}
				else
				{
					Num += 10;
					uart0_tx_buf[Num - 4] =0x31;
					uart0_tx_buf[Num - 3] =0x30;
					uart0_tx_buf[Num - 2] =0x30;
					uart0_tx_buf[Num - 1]=0x20;
				}
			}
    	else if(uart0_rx_buf[22+idx] == 0x33)//查询当前色温
			{
				UpLoadBuf = "3 3 0 0 ";
				memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 8);
				if(Device_State_Data.Light_colour_temperature<10)
				{

					Num += 8;
					uart0_tx_buf[Num - 2] =Device_State_Data.Light_colour_temperature+0x30;
					uart0_tx_buf[Num - 1] =0x20;
				}
				else if(Device_State_Data.Light_colour_temperature<100)
				{	
					_data[0]=Device_State_Data.Light_colour_temperature/10;
					_data[1]=Device_State_Data.Light_colour_temperature%10;	

					Num += 9;
					uart0_tx_buf[Num - 3] =_data[0]+0x30;
					uart0_tx_buf[Num - 2] =_data[1]+0x30;
					uart0_tx_buf[Num - 1] =0x20;
				}
				else if(Device_State_Data.Light_colour_temperature<1000)
				{
					_data[0]=Device_State_Data.Light_colour_temperature/100;
					_data[1]=Device_State_Data.Light_colour_temperature%100/10;
					_data[2]=Device_State_Data.Light_colour_temperature%10;

					Num += 10;
					uart0_tx_buf[Num - 4] =_data[0]+0x30;
					uart0_tx_buf[Num - 3] =_data[1]+0x30;
					uart0_tx_buf[Num - 2] =_data[2]+0x30;
					uart0_tx_buf[Num - 1] =0x20;
				}	
				else if(Device_State_Data.Light_colour_temperature<10000)
				{
					_data[0]=Device_State_Data.Light_colour_temperature/1000;
					_data[1]=Device_State_Data.Light_colour_temperature%1000/100;	
					_data[2]=Device_State_Data.Light_colour_temperature%100/10;	
					_data[3]=Device_State_Data.Light_colour_temperature%10;
					Num += 11;
					uart0_tx_buf[Num - 5] =_data[0]+0x30;
					uart0_tx_buf[Num - 4] =_data[1]+0x30;
					uart0_tx_buf[Num - 3] =_data[2]+0x30;
					uart0_tx_buf[Num - 2] =_data[3]+0x30;
					uart0_tx_buf[Num - 1] =0x20;
				}
			}
	 else if(uart0_rx_buf[22+idx] == 0x34)//查询当前模式
			{
				switch(Device_State_Data.lightmode)
				{
						case '0':  	UpLoadBuf = "3 4 0 0 ";   break;
						case '1': 	UpLoadBuf = "3 4 0 1 ";   break;
						case '2': 	UpLoadBuf = "3 4 0 2 ";   break;
						case '3':   UpLoadBuf = "3 4 0 3 ";   break;
						case '4':  	UpLoadBuf = "3 4 0 4 ";   break;
					  default:    UpLoadBuf = "3 4 0 4 ";   break;
				}

				memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 8);
				Num += 8;
			
			}
		 else if(uart0_rx_buf[22+idx] == 0x37)//查询定时时间
			{
				UpLoadBuf = "3 7 0 \"00:00:00-00:00:00\" ";
				memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 26);
				Num += 26;
				uart0_tx_buf[Num-19]=Device_State_Data.start_time_hour/10+0x30;
				uart0_tx_buf[Num-18]=Device_State_Data.start_time_hour%10+0x30;
				
				uart0_tx_buf[Num-16]=Device_State_Data.start_time_min/10+0x30;
				uart0_tx_buf[Num-15]=Device_State_Data.start_time_min%10+0x30;
				
				uart0_tx_buf[Num-10]=Device_State_Data.end_time_hour/10+0x30;
				uart0_tx_buf[Num-9]=Device_State_Data.end_time_hour%10+0x30;
				
				uart0_tx_buf[Num-7]=Device_State_Data.end_time_min/10+0x30;
				uart0_tx_buf[Num-6]=Device_State_Data.end_time_min%10+0x30;				
				uart0_tx_buf[Num - 1] =0x20;
			}			
			else
			{
				break;
			}	
				
		}
		else if (uart0_rx_buf[20+idx] == 0x37)
		{
			if((uart0_rx_buf[23+idx]<0x30)||(uart0_rx_buf[23+idx]>0x39))//ppid<10
			{
				offset_value=4;
				if (uart0_rx_buf[22+idx] == 0x31)//查询A杆故障
				{
					UpLoadBuf ="7 1 0 0 ";
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 8);
					Num += 8;
					if(Err.data&0x01)//有遇阻
					{
						uart0_tx_buf[Num-2]=0x31;
					}
					else if(Err.data&0x02)//有超重
					{
						uart0_tx_buf[Num-2]=0x32;
					}
					else if(Err.data&0x04)//有电机堵转
					{
						uart0_tx_buf[Num-2]=0x33;
					}
					else
					{
						uart0_tx_buf[Num-2]=0x30;
					}
					uart0_tx_buf[Num - 1]=0x20;
				}
				else if (uart0_rx_buf[22+idx] == 0x34)//A晾杆状态
				{
					UpLoadBuf = (char *)Up_Motor_Properties[Device_State_Data.Motor_State_Byte];
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 8);
					Num += 8;
					uart0_tx_buf[Num-2]=Device_State_Data.Motor_State_Byte+0x30;
					uart0_tx_buf[Num - 1]=0x20;
				}
				else 
				{
					break;
				}
			}
			else
			{
				offset_value=5;
				if ((uart0_rx_buf[22+idx]==0x31)&&(uart0_rx_buf[23+idx]==0x31))//A晾杆当前位置
				{
					UpLoadBuf ="7 11 0 0 ";
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 9);
					value=Device_State_Data.Motor_Position;
					if(value<10)
					{
						Num += 9;
						uart0_tx_buf[Num - 2] =value+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else if(value<100)
					{	
						_data[0]=value/10;
						_data[1]=value%10;
						
						Num += 10;
						uart0_tx_buf[Num - 3] =_data[0]+0x30;
						uart0_tx_buf[Num - 2] =_data[1]+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else
					{
						Num += 11;
						uart0_tx_buf[Num - 4] =0x31;
						uart0_tx_buf[Num - 3] =0x30;
						uart0_tx_buf[Num - 2] =0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
				}				
				else if ((uart0_rx_buf[22+idx]==0x31)&&(uart0_rx_buf[23+idx]==0x32))//查询A晾杆设定位置
				{
					UpLoadBuf ="7 12 0 0 ";
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 9);	
					value=Device_State_Data.set_position;
					if(value<10)
					{
						Num += 9;
						uart0_tx_buf[Num - 2] =value+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else if(value<100)
					{	
						_data[0]=value/10;
						_data[1]=value%10;
						
						Num += 10;
						uart0_tx_buf[Num - 3] =_data[0]+0x30;
						uart0_tx_buf[Num - 2] =_data[1]+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else
					{
						Num += 11;
						uart0_tx_buf[Num - 4] =0x31;
						uart0_tx_buf[Num - 3] =0x30;
						uart0_tx_buf[Num - 2] =0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
				}	
				else if((uart0_rx_buf[22+idx]==0x31)&&(uart0_rx_buf[23+idx]==0x33))//A杆上限位
				{
					UpLoadBuf ="7 13 0 0 ";
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 9);	
					value=Device_State_Data.electronic_up_limit;
					if(value<10)
					{
						Num += 9;
						uart0_tx_buf[Num - 2] =value+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else if(value<100)
					{
						_data[0]=value/10;
						_data[1]=value%10;	
						Num += 10;
						uart0_tx_buf[Num - 3] =_data[0]+0x30;
						
						uart0_tx_buf[Num - 2] =_data[1]+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else
					{
						Num += 11;
						uart0_tx_buf[Num - 4] =0x31;
						uart0_tx_buf[Num - 3] =0x30;
						uart0_tx_buf[Num - 2] =0x30;
						uart0_tx_buf[Num - 1]=0x20;				
					}	
				}
				else if((uart0_rx_buf[22+idx]==0x31)&&(uart0_rx_buf[23+idx]==0x34))//A杆下限位
				{
					UpLoadBuf ="7 14 0 0 ";
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 9);	
					value=Device_State_Data.electronic_down_limit;
					if(value<10)
					{
						Num += 9;
						uart0_tx_buf[Num - 2] =value+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else if(value<100)
					{
						_data[0]=value/10;
						_data[1]=value%10;	
						Num += 10;
						uart0_tx_buf[Num - 3] =_data[0]+0x30;
						uart0_tx_buf[Num - 2] =_data[1]+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else
					{
						Num += 11;
						uart0_tx_buf[Num - 4] =0x31;
						uart0_tx_buf[Num - 3] =0x30;
						uart0_tx_buf[Num - 2] =0x30;
						uart0_tx_buf[Num - 1]=0x20;				
					}	
				}
				else if((uart0_rx_buf[22+idx]==0x31)&&(uart0_rx_buf[23+idx]==0x35))//A杆收合状态
				{
					if (Device_State_Data.convergent==CONVERGENT_ON)
					{
						UpLoadBuf = "7 15 0 true ";
						memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 12);
						Num += 12;		
					}
					else 
					{
						UpLoadBuf ="7 15 0 false ";
						memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 13);
						Num += 13;								
					}
				}
				else if((uart0_rx_buf[22+idx]==0x31)&&(uart0_rx_buf[23+idx]==0x36))
				{
					UpLoadBuf = "7 16 0 0 ";
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 9);
					value=Device_State_Data.current_speed;
					if(value<10)
					{
						Num += 11;
						uart0_tx_buf[Num - 4] =0x30;
						uart0_tx_buf[Num - 3] ='.';						
						uart0_tx_buf[Num - 2] =value+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else if(value<100)
					{	
						_data[0]=value/10;
						_data[1]=value%10;
						
						Num += 11;
						uart0_tx_buf[Num - 4] =_data[0]+0x30;
						uart0_tx_buf[Num - 3] ='.';
						uart0_tx_buf[Num - 2] =_data[1]+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else
					{
						_data[0]=value/100;
						_data[1]=value%100/10;
						_data[2]=value%10;
						Num += 12;
						uart0_tx_buf[Num - 5] =_data[0]+0x31;
						uart0_tx_buf[Num - 4] =_data[1]+0x30;
						uart0_tx_buf[Num - 3] ='.';
						uart0_tx_buf[Num - 2] =_data[2]+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}				
				}

				else
				{
					break;
				}
			}
		}
		else if (uart0_rx_buf[20+idx] == 0x36)
		{
			if((uart0_rx_buf[23+idx]<0x30)||(uart0_rx_buf[23+idx]>0x39))//ppid<10
			{
				offset_value=4;
				if (uart0_rx_buf[22+idx] == 0x31)//查询B杆故障
				{
					UpLoadBuf ="6 1 0 0 ";
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 8);
					Num += 8;
					if(Err_B.data&0x01)//有遇阻
					{
						uart0_tx_buf[Num-2]=0x31;
					}
					else if(Err_B.data&0x02)//有超重
					{
						uart0_tx_buf[Num-2]=0x32;
					}
					else if(Err_B.data&0x04)//有电机堵转
					{
						uart0_tx_buf[Num-2]=0x33;
					}
					else
					{
						uart0_tx_buf[Num-2]=0x30;
					}
					uart0_tx_buf[Num - 1]=0x20;
				}
				else if (uart0_rx_buf[22+idx] == 0x34)//b晾杆状态
				{
					UpLoadBuf = (char *)Up_Motor_Properties_B[Device_State_Data.B_Motor_State_Byte];
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 8);
					Num += 8;
					uart0_tx_buf[Num-2]=Device_State_Data.B_Motor_State_Byte+0x30;
					uart0_tx_buf[Num - 1]=0x20;
				}
				else
				{
					break;
				}
			}
			else
			{
				offset_value=5;
				if((uart0_rx_buf[22+idx]==0x31)&&(uart0_rx_buf[23+idx]==0x31))//b晾杆当前位置
				{
					UpLoadBuf ="6 11 0 0 ";
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 9);
					value=Device_State_Data.B_Motor_Position;
					if(value<10)
					{
						Num += 9;
						uart0_tx_buf[Num - 2] =value+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else if(value<100)
					{	
						_data[0]=value/10;
						_data[1]=value%10;
						
						Num += 10;
						uart0_tx_buf[Num - 3] =_data[0]+0x30;
						uart0_tx_buf[Num - 2] =_data[1]+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else
					{
						Num += 11;
						uart0_tx_buf[Num - 4] =0x31;
						uart0_tx_buf[Num - 3] =0x30;
						uart0_tx_buf[Num - 2] =0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
				}				
				else if((uart0_rx_buf[22+idx]==0x31)&&(uart0_rx_buf[23+idx]==0x32))//查询B晾杆设定位置
				{
					UpLoadBuf ="6 12 0 0 ";
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 9);	
					value=Device_State_Data.B_set_position;
					if(value<10)
					{
						Num += 9;
						uart0_tx_buf[Num - 2] =value+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else if(value<100)
					{	
						_data[0]=value/10;
						_data[1]=value%10;
						
						Num += 10;
						uart0_tx_buf[Num - 3] =_data[0]+0x30;
						uart0_tx_buf[Num - 2] =_data[1]+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else
					{
						Num += 11;
						uart0_tx_buf[Num - 4] =0x31;
						uart0_tx_buf[Num - 3] =0x30;
						uart0_tx_buf[Num - 2] =0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
				}	
				else if((uart0_rx_buf[22+idx]==0x31)&&(uart0_rx_buf[23+idx]==0x33))//B杆上限位
				{
					UpLoadBuf ="6 13 0 0 ";
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 9);	
					value=Device_State_Data.B_electronic_up_limit;
					if(value<10)
					{
						Num += 9;
						uart0_tx_buf[Num - 2] =value+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else if(value<100)
					{
						_data[0]=value/10;
						_data[1]=value%10;	
						Num += 10;
						uart0_tx_buf[Num - 3] =_data[0]+0x30;
						uart0_tx_buf[Num - 2] =_data[1]+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else
					{
						Num += 11;
						uart0_tx_buf[Num - 4] =0x31;
						uart0_tx_buf[Num - 3] =0x30;
						uart0_tx_buf[Num - 2] =0x30;
						uart0_tx_buf[Num - 1] =0x20;				
					}	
				}
				else if((uart0_rx_buf[22+idx]==0x31)&&(uart0_rx_buf[23+idx]==0x34))//B杆下限位
				{
					UpLoadBuf ="6 14 0 0 ";
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 9);	
					value=Device_State_Data.B_electronic_down_limit;
					if(value<10)
					{
						Num += 9;
						uart0_tx_buf[Num - 2] =value+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else if(value<100)
					{
						_data[0]=value/10;
						_data[1]=value%10;	
						Num += 10;
						uart0_tx_buf[Num - 3] =_data[0]+0x30;
						uart0_tx_buf[Num - 2] =_data[1]+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else
					{
						Num += 11;
						uart0_tx_buf[Num - 4] =0x31;
						uart0_tx_buf[Num - 3] =0x30;
						uart0_tx_buf[Num - 2] =0x30;
						uart0_tx_buf[Num - 1]=0x20;				
					}	
				}
				else if((uart0_rx_buf[22+idx]==0x31)&&(uart0_rx_buf[23+idx]==0x35))//B杆收合状态
				{
					if (Device_State_Data.B_convergent==CONVERGENT_ON)
					{
						UpLoadBuf = "6 15 0 true ";
						memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 13);
						Num += 12;		
					}
					else 
					{
						UpLoadBuf ="6 15 0 false ";
						memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 14);
						Num += 13;								
					}
				}
				else if((uart0_rx_buf[22+idx]==0x31)&&(uart0_rx_buf[23+idx]==0x36))//
				{
					UpLoadBuf = "6 16 0 0 ";
					memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 9);
					value=Device_State_Data.B_current_speed;
					if(value<10)
					{
						Num += 11;
						uart0_tx_buf[Num - 4] =0x30;
						uart0_tx_buf[Num - 3] ='.';						
						uart0_tx_buf[Num - 2] =value+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else if(value<100)
					{	
						_data[0]=value/10;
						_data[1]=value%10;	
						Num += 11;
						uart0_tx_buf[Num - 4] =_data[0]+0x30;
						uart0_tx_buf[Num - 3] ='.';
						uart0_tx_buf[Num - 2] =_data[1]+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}
					else
					{
						_data[0]=value/100;
						_data[1]=value%100/10;
						_data[2]=value%10;
						Num += 12;
						uart0_tx_buf[Num - 5] =_data[0]+0x31;
						uart0_tx_buf[Num - 4] =_data[1]+0x30;
						uart0_tx_buf[Num - 3] ='.';
						uart0_tx_buf[Num - 2] =_data[2]+0x30;
						uart0_tx_buf[Num - 1]=0x20;
					}				
				}

				else
				{
					break;
				}
			}
		}/*遥控器设置*/
		else if (uart0_rx_buf[20+idx] == 0x38) //
		{
			    offset_value=4;//识别的指令+4
				  if((uart0_rx_buf[22+idx]==0x31) ) 
					{
							if (Device_State_Data.Control_open==BEEP_ON)
							{
								UpLoadBuf = "8 1 0 true ";
								memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 11);
								Num += 11;		
							}
							else 
							{
								UpLoadBuf = "8 1 0 false ";
								memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf, 12);
								Num += 12;								
							}	
					}
	  else	 if((uart0_rx_buf[22+idx]==0x33)) 
					{
				  char UpLoadBuf1[90];
					memset(UpLoadBuf1, 0, sizeof(UpLoadBuf1));
					switch(Flash_Data.Remote_Control_Num)	
					{
						case 0: 
							
								snprintf(UpLoadBuf1,
								sizeof(UpLoadBuf1)-1,
								"8 3 0 \"%02X:%02X:%02X:%02X \" ",
								(0xFF),
								(0xFF),
								(0xFF),
								(0xFF));
					  
						break; 
					case 1: 
							snprintf(UpLoadBuf1,
									 sizeof(UpLoadBuf1)-1,
									 "8 3 0 \"%02X:%02X:%02X:%02X \" ",
									 (unsigned int)((Flash_Data.Remote_Control_Id[0] >> 24) & 0xFF),
									 (unsigned int)((Flash_Data.Remote_Control_Id[0] >> 16) & 0xFF),
									 (unsigned int)((Flash_Data.Remote_Control_Id[0] >> 8) & 0xFF),
									 (unsigned int)(Flash_Data.Remote_Control_Id[0] & 0xFF));
							break; 

					case 2:
							snprintf(UpLoadBuf1,
									 sizeof(UpLoadBuf1)-1,
									 "8 3 0 \"%02X:%02X:%02X:%02X,%02X:%02X:%02X:%02X\" ",
									 (unsigned int)((Flash_Data.Remote_Control_Id[0] >> 24) & 0xFF),
									 (unsigned int)((Flash_Data.Remote_Control_Id[0] >> 16) & 0xFF),
									 (unsigned int)((Flash_Data.Remote_Control_Id[0] >> 8) & 0xFF),
									 (unsigned int)(Flash_Data.Remote_Control_Id[0] & 0xFF),
									 (unsigned int)((Flash_Data.Remote_Control_Id[1] >> 24) & 0xFF),
									 (unsigned int)((Flash_Data.Remote_Control_Id[1] >> 16) & 0xFF),
									 (unsigned int)((Flash_Data.Remote_Control_Id[1] >> 8) & 0xFF),
									 (unsigned int)(Flash_Data.Remote_Control_Id[1] & 0xFF));
							break;

					case 3: 
							snprintf(UpLoadBuf1,
									 sizeof(UpLoadBuf1)-1,
									 "8 3 0 \"%02X:%02X:%02X:%02X,%02X:%02X:%02X:%02X,%02X:%02X:%02X:%02X\" ",
									 (unsigned int)((Flash_Data.Remote_Control_Id[0] >> 24) & 0xFF),
									 (unsigned int)((Flash_Data.Remote_Control_Id[0] >> 16) & 0xFF),
									 (unsigned int)((Flash_Data.Remote_Control_Id[0] >> 8) & 0xFF),
									 (unsigned int)(Flash_Data.Remote_Control_Id[0] & 0xFF),
									 (unsigned int)((Flash_Data.Remote_Control_Id[1] >> 24) & 0xFF),
									 (unsigned int)((Flash_Data.Remote_Control_Id[1] >> 16) & 0xFF),
									 (unsigned int)((Flash_Data.Remote_Control_Id[1] >> 8) & 0xFF),
									 (unsigned int)(Flash_Data.Remote_Control_Id[1] & 0xFF),
									 (unsigned int)((Flash_Data.Remote_Control_Id[2] >> 24) & 0xFF),
									 (unsigned int)((Flash_Data.Remote_Control_Id[2] >> 16) & 0xFF),
									 (unsigned int)((Flash_Data.Remote_Control_Id[2] >> 8) & 0xFF),
									 (unsigned int)(Flash_Data.Remote_Control_Id[2] & 0xFF));
							break;
 
						default:
						snprintf(UpLoadBuf1,
								sizeof(UpLoadBuf1)-1,
								"8 3 0 \"%02X:%02X:%02X:%02X \" ",
								(0xFF),
								(0xFF),
								(0xFF),
								(0xFF));
						break;
					}

						memcpy((uint8_t *)uart0_tx_buf + Num, UpLoadBuf1, strlen(UpLoadBuf1));
						Num += strlen(UpLoadBuf1);	
					}
 	
		   }
		if(Num>179)//防止数据太长
		{
      Num=179;
			uart0_tx_buf[Num-1] = 0x0d;//结束符	
			r_uart0_send_bytes((uint8_t *)uart0_tx_buf, Num);
			Num=0;
			memset(uart0_tx_buf, 0, sizeof(uart0_tx_buf));
			return ;
		}
 
	 }
	uart0_tx_buf[Num-1] = 0x0d;//结束符	
	r_uart0_send_bytes((uint8_t *)uart0_tx_buf, Num);
	Num=0;
	g_MiioRoundFlag = 1;
	Flag.time150ms_flag=0;
	Time.time150ms=150;
	
}
void Set_Properties (void)//回应app控制指令
{
	uint8_t i=0,offset=0;
	uint8_t Num=7;
	char *UpLoadBuf;	
	memcpy((uint8_t *)uart0_tx_buf, setresult, 7);
	while((20+i)<Frame_length)
	{
		if(uart0_rx_buf[20+i] == 0x32)//siid  2
		{
			if(uart0_rx_buf[22+i] == 0x32)//电机控制
			{
				UpLoadBuf = "2 2 0 ";
				memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 6);
				Num+=6;
				offset=6;
				switch (uart0_rx_buf[24+i])
				{
					case 0x30:
					{
						Cmd_Motor_Stop();
						Cmd_Motor_Stop_B();
					}break;		 
					case 0x31:
					{
						Flag.B_No_judge_electronic_limit=0;
						Flag.No_judge_electronic_limit=0;
						if(Up_State_Data.Err_Byte&0x01)
						{
							Up_State_Data.Motor_State_Byte=MOTOR_UP_STATE;//app会点亮图标，如果此时是遇阻，需要上报停止
							Err.err_bit.Encounter_Obstacle=0;
							Time.Encounter_Obstacle_beepnum=0;
							Up_State_Data.Err_Byte&=0xfe;//重新上报app遇阻
						}
						else
						{
							if(Device_State_Data.Motor_State_Byte!=MOTOR_UP_STATE)
							{
								Cmd_Motor_Up();
							}
						}
						if(Up_State_Data.B_Err_Byte&0x01)
						{
							Up_State_Data.B_Motor_State_Byte=MOTOR_UP_STATE;//app会点亮图标，如果此时是遇阻，需要上报停止
							Err_B.err_bit.Encounter_Obstacle=0;
							Time.B_Encounter_Obstacle_beepnum=0;
							Up_State_Data.B_Err_Byte&=0xfe;//重新上报app遇阻
						}
						else
						{
							if(Device_State_Data.B_Motor_State_Byte!=MOTOR_UP_STATE)
							{
								Cmd_Motor_Up_B();
							}
						}
					}break;		
					case 0x32:
					{
						Flag.B_No_judge_electronic_limit=0;
						Flag.No_judge_electronic_limit=0;
						if(Up_State_Data.Err_Byte&0x01)
						{
							Up_State_Data.Motor_State_Byte=MOTOR_DOWN_STATE;//app会点亮图标，如果此时是遇阻，需要上报停止
							Err.err_bit.Encounter_Obstacle=0;
							Time.Encounter_Obstacle_beepnum=0;
							Up_State_Data.Err_Byte&=0xfe;//重新上报app遇阻
						}
						else
						{
							if(Device_State_Data.Motor_State_Byte!=MOTOR_DOWN_STATE)
							{
								Cmd_Motor_Down();
							}
						}
						if(Up_State_Data.B_Err_Byte&0x01)
						{
							Up_State_Data.B_Motor_State_Byte=MOTOR_DOWN_STATE;//app会点亮图标，如果此时是遇阻，需要上报停止
							Err_B.err_bit.Encounter_Obstacle=0;
							Time.B_Encounter_Obstacle_beepnum=0;
							Up_State_Data.B_Err_Byte&=0xfe;//重新上报app遇阻
						}
						else
						{
							if(Device_State_Data.B_Motor_State_Byte!=MOTOR_DOWN_STATE)
							{
								Cmd_Motor_Down_B();
							}
						}
					}break;	
					case 0x33:
					{
						Flag.No_judge_electronic_limit=1;
						Cmd_Motor_Stop();
						Flag.B_No_judge_electronic_limit=1;
						Cmd_Motor_Stop_B();	
					}break;	
					case 0x34:
					{
						if(Ell_Data.Motor_Current_Position>UP_ELE_VALUE)//低于电子上限位最小值
						{
							Flag.No_judge_electronic_limit=1;
							Cmd_Motor_Up();	
						}
						if(Ell_Data.B_Motor_Current_Position>UP_ELE_VALUE)//低于电子上限位最小值
						{
							Flag.B_No_judge_electronic_limit=1;
							Cmd_Motor_Up_B();	
						}
	
					}break;
					case 0x35:
					{
						Flag.No_judge_electronic_limit=1;
						Cmd_Motor_Down();
						Flag.B_No_judge_electronic_limit=1;
						Cmd_Motor_Down_B();
					}break;	
					
					
					
					default:  break;			 											 
				}
			}
			else if(uart0_rx_buf[22+i]==0x33)//双杆收合
			{
				UpLoadBuf = "2 3 0 ";
				memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 6);
				Num+=6;		
				if (uart0_rx_buf[24+i] == 0x74)//打开
				{
					offset=9;
					if (Device_State_Data.convergent_all == CONVERGENT_OFF)
					{
						Device_State_Data.convergent_all=CONVERGENT_ON;
						if(Device_State_Data.convergent==CONVERGENT_OFF)
						{
							Cmd_Motor_Up_Longpress();
						}
						if(Device_State_Data.B_convergent==CONVERGENT_OFF)
						{
							Cmd_Motor_Up_Longpress_B();
						}
					}		
				}
				else if(uart0_rx_buf[24+i] == 0x66)//关闭
				{
					offset=10;
					if (Device_State_Data.convergent_all == CONVERGENT_ON)
					{
						Device_State_Data.convergent_all=CONVERGENT_OFF;
						Cmd_Motor_Stop();
						Cmd_Motor_Stop_B();
					}					
				}
			}
	   	else if((uart0_rx_buf[22+i]==0x31)&&(uart0_rx_buf[23+i]==0x32))////ppid>10运行到指定位置
			{
				UpLoadBuf = "2 12 0 ";
				memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 7);
				Num+=7;
				if((uart0_rx_buf[26+i]==0x0d)||(uart0_rx_buf[26+i]==0x20))
				{
					Device_State_Data.set_position_all=(uart0_rx_buf[25+i]-0x30);	
					offset=7;					
				}
				else if((uart0_rx_buf[27+i]==0x0d)||(uart0_rx_buf[27+i]==0x20))
				{
					Device_State_Data.set_position_all=(uart0_rx_buf[25+i]-0x30)*10+(uart0_rx_buf[26+i]-0x30);
					offset=8;
				}
				else if((uart0_rx_buf[28+i]==0x0d)||(uart0_rx_buf[28+i]==0x20))
				{
					Device_State_Data.set_position_all=(uart0_rx_buf[25+i]-0x30)*100+(uart0_rx_buf[26+i]-0x30)*10
								    +uart0_rx_buf[27+i]-0x30;
					offset=9;
				}
				Device_State_Data.set_position=Device_State_Data.set_position_all;
				Device_State_Data.B_set_position=Device_State_Data.set_position_all;
				Run_to_position();	
				Run_to_position_B();					
			}
			
			else
			{
				break;
			}
		}
		else if(uart0_rx_buf[20+i] == 0x35)//开关蜂鸣器
		{
			if(uart0_rx_buf[22+i]==0x31)
			{
				UpLoadBuf = "5 1 0 ";
				memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 6);
				Num+=6;
				if ((uart0_rx_buf[24+i] == 0x31) || (uart0_rx_buf[24+i] == 0x74))//打开
				{
					if(uart0_rx_buf[24+i] == 0x31)
					{
						offset=6;
					}
					else
					{
						offset=9;
					}
					Flash_Data.Beep_open = BEEP_ON;
					Device_State_Data.Beep_Enable=Flash_Data.Beep_open;
				}
				else if ((uart0_rx_buf[24+i] == 0x30) || (uart0_rx_buf[24+i] == 0x66))//关闭
				{
					if(uart0_rx_buf[24+i] == 0x30)
					{
						offset=6;
					}
					else
					{
						offset=10;
					}
					Flash_Data.Beep_open = BEEP_OFF;
					Device_State_Data.Beep_Enable=Flash_Data.Beep_open;		
				}
			}
			else
			{
			 Invalid_Cleanup(&offset);
				break;
			}
		}
		else if (uart0_rx_buf[20+i] == 0x33)//Light
		{	
			if(uart0_rx_buf[22+i] == 0x31)//照明控制
			{
				UpLoadBuf = "3 1 0 ";
				memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 6);
				Num+=6;
				if ((uart0_rx_buf[24+i] == 0x31) || (uart0_rx_buf[24+i] == 0x74))//打开
				{
					if(uart0_rx_buf[24+i] == 0x31)
					{
						offset=6;
					}
					else
					{
						offset=9;
					}
					if (Device_State_Data.Light_State_Byte == LIGHT_OFF)
					{
						Time.Light_key_space_time=Light_KEY_SPACE_TIME;
						Cmd_Light();

					}
				}
				else if ((uart0_rx_buf[24+i] == 0x30) || (uart0_rx_buf[24+i] == 0x66))//关闭
				{
					if(uart0_rx_buf[24+i] == 0x30)
					{
						offset=6;
					}
					else
					{
						offset=10;
					}
					if (Device_State_Data.Light_State_Byte == LIGHT_ON) 
					{
						Time.Light_key_space_time=Light_KEY_SPACE_TIME;
						Cmd_Light();
					}
				} 			
			}
			else if( uart0_rx_buf[22+i] == 0x32  && 
				(uart0_rx_buf[25+i+3] == 0x33 || uart0_rx_buf[25+i+4] == 0x33 || uart0_rx_buf[25+i+5] == 0x33)&&Frame_length<50 )//情景模式并且需要避免夜灯设置冲突
			{
					// --------------- 1. 解析亮度 ---------------
					UpLoadBuf = "3 2 0 ";
					memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 6);
					Num+=6;
          Device_State_Data.lightmode='4';
					if((uart0_rx_buf[25+i]==0x0d)||(uart0_rx_buf[25+i]==0x20))
					{
							Device_State_Data.Light_Bright=(uart0_rx_buf[24+i]-0x30);
							offset=6;
					}
					else if((uart0_rx_buf[26+i]==0x0d)||(uart0_rx_buf[26+i]==0x20))
					{
							Device_State_Data.Light_Bright=(uart0_rx_buf[24+i]-0x30)*10+(uart0_rx_buf[25+i]-0x30);
							offset=7;
					}
					else if((uart0_rx_buf[27+i]==0x0d)||(uart0_rx_buf[27+i]==0x20))
					{
							Device_State_Data.Light_Bright=(uart0_rx_buf[24+i]-0x30)*100+(uart0_rx_buf[25+i]-0x30)*10
															+uart0_rx_buf[26+i]-0x30;
							offset=8;
					}
					set_light_value=(Device_State_Data.Light_Bright*0.97+3)*LIGHT_VALUE;
				
					// --------------- 2. 自动跳转解析色温 ---------------
					int temp_i = i + offset;

					UpLoadBuf = "3 3 0 ";
					memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 6);
					Num+=6;

					if((uart0_rx_buf[25+temp_i]==0x0d)||(uart0_rx_buf[25+temp_i]==0x20))
					{
							Device_State_Data.Light_colour_temperature=(uart0_rx_buf[24+temp_i]-0x30);
							offset+=6;
					}
					else if((uart0_rx_buf[26+temp_i]==0x0d)||(uart0_rx_buf[26+temp_i]==0x20))
					{
							Device_State_Data.Light_colour_temperature=(uart0_rx_buf[24+temp_i]-0x30)*10+(uart0_rx_buf[25+temp_i]-0x30);
							offset+=7;
					}
					else if((uart0_rx_buf[27+temp_i]==0x0d)||(uart0_rx_buf[27+temp_i]==0x20))
					{
							Device_State_Data.Light_colour_temperature=(uart0_rx_buf[24+temp_i]-0x30)*100+(uart0_rx_buf[25+temp_i]-0x30)*10
															+uart0_rx_buf[26+temp_i]-0x30;
							offset+=8;
					}
					else if((uart0_rx_buf[28+temp_i]==0x0d)||(uart0_rx_buf[28+temp_i]==0x20))
					{
							Device_State_Data.Light_colour_temperature=(uart0_rx_buf[24+temp_i]-0x30)*1000+(uart0_rx_buf[25+temp_i]-0x30)*100
															+(uart0_rx_buf[26+temp_i]-0x30)*10+uart0_rx_buf[27+temp_i]-0x30;
							offset+=9;
					}else
					{
					
						  offset+=6;
					
					}
					Flash_Data.Light_CT_value=Device_State_Data.Light_colour_temperature;
					
					// --------------- 3. 蜂鸣器 ---------------
					if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
					{
							Device_State_Data.Light_State_Byte=LIGHT_ON;
							SetupBzhx(LIGHT_ON_BEEP,Flash_Data.Beep_open);
					}
					else
					{
							SetupBzhx(SHORT_BEEP,Flash_Data.Beep_open);
					}
 
					 set_light_value=(Device_State_Data.Light_Bright*0.97+3)*LIGHT_VALUE;
					if(Flash_Data.Max_Light_Value==Device_State_Data.Light_Bright )
					{
					
					}else
					{
						Flash_Data.Max_Light_Value=Device_State_Data.Light_Bright;
						colour_temperature_temp=(Flash_Data.Light_CT_value-2700)/5;
					}
					// ============== 关键：执行完直接跳过 ==============
					i += offset;
					continue;
		 }
		  else if(uart0_rx_buf[22+i] == 0x34)//情景模式
			{
				
					UpLoadBuf = "3 4 0 ";
					memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 6);
					Num+=6;	
					offset=6;
				 switch(uart0_rx_buf[24+i])//情景模式
				 {
					 
							case '0':  
								
							Device_State_Data.Light_Bright=100;
						  Device_State_Data.Light_colour_temperature=5700;
									Flash_Data.Light_CT_value=Device_State_Data.Light_colour_temperature;
					
								// --------------- 3. 蜂鸣器 ---------------
								if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
								{
										Device_State_Data.Light_State_Byte=LIGHT_ON;
										SetupBzhx(LIGHT_ON_BEEP,Flash_Data.Beep_open);
								}
								else
								{
										SetupBzhx(SHORT_BEEP,Flash_Data.Beep_open);
								}
			 
								 set_light_value=(Device_State_Data.Light_Bright*0.97+3)*LIGHT_VALUE;
								if(Flash_Data.Max_Light_Value==Device_State_Data.Light_Bright )
								{
								
								}else
								{
									Flash_Data.Max_Light_Value=Device_State_Data.Light_Bright;
									colour_temperature_temp=(Flash_Data.Light_CT_value-2700)/5;
								}
							
							break;
							case '1':  
								
							Device_State_Data.Light_Bright=70;
						  Device_State_Data.Light_colour_temperature=4000;
									Flash_Data.Light_CT_value=Device_State_Data.Light_colour_temperature;
					
								// --------------- 3. 蜂鸣器 ---------------
								if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
								{
										Device_State_Data.Light_State_Byte=LIGHT_ON;
										SetupBzhx(LIGHT_ON_BEEP,Flash_Data.Beep_open);
								}
								else
								{
										SetupBzhx(SHORT_BEEP,Flash_Data.Beep_open);
								}
			 
								 set_light_value=(Device_State_Data.Light_Bright*0.97+3)*LIGHT_VALUE;
								if(Flash_Data.Max_Light_Value==Device_State_Data.Light_Bright )
								{
								
								}else
								{
									Flash_Data.Max_Light_Value=Device_State_Data.Light_Bright;
									colour_temperature_temp=(Flash_Data.Light_CT_value-2700)/5;
								}
								
							break;
							case '2':  
						 		Device_State_Data.Light_Bright=30;
						    Device_State_Data.Light_colour_temperature=2700;
									Flash_Data.Light_CT_value=Device_State_Data.Light_colour_temperature;
					
								// --------------- 3. 蜂鸣器 ---------------
								if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
								{
										Device_State_Data.Light_State_Byte=LIGHT_ON;
										SetupBzhx(LIGHT_ON_BEEP,Flash_Data.Beep_open);
								}
								else
								{
										SetupBzhx(SHORT_BEEP,Flash_Data.Beep_open);
								}
			 
								 set_light_value=(Device_State_Data.Light_Bright*0.97+3)*LIGHT_VALUE;
								if(Flash_Data.Max_Light_Value==Device_State_Data.Light_Bright )
								{
								
								}else
								{
									Flash_Data.Max_Light_Value=Device_State_Data.Light_Bright;
									colour_temperature_temp=(Flash_Data.Light_CT_value-2700)/5;
								}
							
							break;
							case '3': 
								Device_State_Data.Light_Bright=20;
						    Device_State_Data.Light_colour_temperature=2700;
									Flash_Data.Light_CT_value=Device_State_Data.Light_colour_temperature;
					
								// --------------- 3. 蜂鸣器 ---------------
								if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
								{
										Device_State_Data.Light_State_Byte=LIGHT_ON;
										SetupBzhx(LIGHT_ON_BEEP,Flash_Data.Beep_open);
								}
								else
								{
										SetupBzhx(SHORT_BEEP,Flash_Data.Beep_open);
								}
			 
								 set_light_value=(Device_State_Data.Light_Bright*0.97+3)*LIGHT_VALUE;
								if(Flash_Data.Max_Light_Value==Device_State_Data.Light_Bright )
								{
								
								}else
								{
									Flash_Data.Max_Light_Value=Device_State_Data.Light_Bright;
									colour_temperature_temp=(Flash_Data.Light_CT_value-2700)/5;
								}
							
							break;
							case '4': 
//							set_light_value=(Device_State_Data.Light_Bright*0.97+3)*LIGHT_VALUE;
//							Flash_Data.Max_Light_Value=Device_State_Data.Light_Bright;
//							colour_temperature_temp=(Flash_Data.Light_CT_value-2700)/5;	
             
							break;
							default:  uart0_rx_buf[24+i]='4';   break ;
 
				 }
			   Device_State_Data.lightmode=uart0_rx_buf[24+i] ;
 
			}
			else if(uart0_rx_buf[22+i] == 0x32)//照明亮度
			{
				UpLoadBuf = "3 2 0 ";
				memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 6);
				Num+=6;	
				if((uart0_rx_buf[25+i]==0x0d)||(uart0_rx_buf[25+i]==0x20))
				{
					Device_State_Data.Light_Bright=(uart0_rx_buf[24+i]-0x30);	
					offset=6;
				}
				else if((uart0_rx_buf[26+i]==0x0d)||(uart0_rx_buf[26+i]==0x20))
				{
					Device_State_Data.Light_Bright=(uart0_rx_buf[24+i]-0x30)*10+(uart0_rx_buf[25+i]-0x30);
					offset=7;
				}
				else if((uart0_rx_buf[27+i]==0x0d)||(uart0_rx_buf[27+i]==0x20))
				{
					Device_State_Data.Light_Bright=(uart0_rx_buf[24+i]-0x30)*100+(uart0_rx_buf[25+i]-0x30)*10
								    +uart0_rx_buf[26+i]-0x30;
					offset=8;
				}
				if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
				{
					Device_State_Data.Light_State_Byte=LIGHT_ON;
					colour_temperature_temp=(Flash_Data.Light_CT_value-2700)/5;//开灯时色温不渐变，直接是最终状态
					SetupBzhx(LIGHT_ON_BEEP,Flash_Data.Beep_open);
				}
				else
				{
					//SetupBzhx(LONG_BEEP,Flash_Data.Beep_open);
				}
						//在夜灯开启后,照明滑条不改变日灯记忆亮度
				if((Flash_Data.Light_Bright_Enable==1)&&(Flag.Brightness_time_flag))
				{
					Flag.Brightness_change=1;//夜灯模式下照明滑条动作
				}
				else
				{
					Flash_Data.Max_Light_Value=Device_State_Data.Light_Bright;
				}

			}
	  	else if(uart0_rx_buf[22+i] == 0x33)//灯光色温
			{
				UpLoadBuf = "3 3 0 ";
				memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 6);
				Num+=6;		
		
				if((uart0_rx_buf[25+i]==0x0d)||(uart0_rx_buf[25+i]==0x20))
				{
					Device_State_Data.Light_colour_temperature=(uart0_rx_buf[24+i]-0x30);	
					offset=6;
				}
				else if((uart0_rx_buf[26+i]==0x0d)||(uart0_rx_buf[26+i]==0x20))
				{
					Device_State_Data.Light_colour_temperature=(uart0_rx_buf[24+i]-0x30)*10+(uart0_rx_buf[25+i]-0x30);
					offset=7;
				}
				else if((uart0_rx_buf[27+i]==0x0d)||(uart0_rx_buf[27+i]==0x20))
				{
					Device_State_Data.Light_colour_temperature=(uart0_rx_buf[24+i]-0x30)*100+(uart0_rx_buf[25+i]-0x30)*10
								    +uart0_rx_buf[26+i]-0x30;
					offset=8;
				}
				else if((uart0_rx_buf[28+i]==0x0d)||(uart0_rx_buf[28+i]==0x20))
				{
					Device_State_Data.Light_colour_temperature=(uart0_rx_buf[24+i]-0x30)*1000+(uart0_rx_buf[25+i]-0x30)*100
								    +(uart0_rx_buf[26+i]-0x30)*10+uart0_rx_buf[27+i]-0x30;
					offset=9;					
				}
				if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
				{
					Device_State_Data.Light_State_Byte=LIGHT_ON;
					SetupBzhx(LIGHT_ON_BEEP,Flash_Data.Beep_open);

				}
				else
				{
					//SetupBzhx(LONG_BEEP,Flash_Data.Beep_open);
				}	
				Flash_Data.Light_CT_value=Device_State_Data.Light_colour_temperature;
				
			}
			else if(uart0_rx_buf[22+i] == 0x35)//夜灯开关
			{
				UpLoadBuf = "3 5 0 ";
				memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 6);
				Num+=6;		
				offset=6;
				if ((uart0_rx_buf[24+i] == 0x31) || (uart0_rx_buf[24+i] == 0x74))//打开
				{
					if(uart0_rx_buf[24+i] == 0x31)
					{
						offset=6;
					}
					else
					{
						offset=9;
					}
					Device_State_Data.nightlight_switch=LIGHT_ON;
					Flash_Data.Light_Bright_Enable=Device_State_Data.nightlight_switch;
				}
				else if ((uart0_rx_buf[24+i] == 0x30) || (uart0_rx_buf[24+i] == 0x66))//关闭
				{
					if(uart0_rx_buf[24+i] == 0x30)
					{
						offset=6;
					}
					else
					{
						offset=10;
					}
					Device_State_Data.nightlight_switch=LIGHT_OFF;
					Flash_Data.Light_Bright_Enable=Device_State_Data.nightlight_switch;
				}	
					Judge_Nightlight_Mode();
				if(Flag.Brightness_time_flag)//进入夜灯时间
				{
					Device_State_Data.Light_Bright=Flash_Data.Light_Bright_Value;
				}
				else
				{
					Device_State_Data.Light_Bright=Flash_Data.Max_Light_Value;
				}
				
			}
			else if(uart0_rx_buf[22+i] == 0x36)//夜灯亮度
			{
				UpLoadBuf = "3 6 0 ";
				memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 6);
				Num+=6;		
				if((uart0_rx_buf[25+i]==0x0d)||(uart0_rx_buf[25+i]==0x20))
				{
					Device_State_Data.brightness=(uart0_rx_buf[24+i]-0x30);	
					offset=6;
				}
				else if((uart0_rx_buf[26+i]==0x0d)||(uart0_rx_buf[26+i]==0x20))
				{
					Device_State_Data.brightness=(uart0_rx_buf[24+i]-0x30)*10+(uart0_rx_buf[25+i]-0x30);
					offset=7;
				}
				else if((uart0_rx_buf[27+i]==0x0d)||(uart0_rx_buf[27+i]==0x20))
				{
					Device_State_Data.brightness=(uart0_rx_buf[24+i]-0x30)*100+(uart0_rx_buf[25+i]-0x30)*10
								    +uart0_rx_buf[26+i]-0x30;
					offset=8;
				}
				
				  Flash_Data.Light_Bright_Value=Device_State_Data.brightness;	
					Judge_Nightlight_Mode();
		    if(Flag.Brightness_time_flag)//进入夜灯时间
				{
					Device_State_Data.Light_Bright=Flash_Data.Light_Bright_Value;
				}
				else
				{
					Device_State_Data.Light_Bright=Flash_Data.Max_Light_Value;
				}	
			}
 
			else if(uart0_rx_buf[22+i] == 0x37)//夜灯起始时间,第24个字节开始"00:00:00-00:00:00"
			{
				UpLoadBuf = "3 7 0 ";
				memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 6);
				Num+=6;	
				offset=6;
				Device_State_Data.start_time_hour=(uart0_rx_buf[25+i]-0x30)*10+(uart0_rx_buf[26+i]-0x30);
				Device_State_Data.start_time_min=(uart0_rx_buf[28+i]-0x30)*10+(uart0_rx_buf[29+i]-0x30);
				
				Device_State_Data.end_time_hour=(uart0_rx_buf[34+i]-0x30)*10+(uart0_rx_buf[35+i]-0x30);
				Device_State_Data.end_time_min=(uart0_rx_buf[37+i]-0x30)*10+(uart0_rx_buf[38+i]-0x30);
				
				Flash_Data.Light_open_hour_time=Device_State_Data.start_time_hour;
				Flash_Data.Light_open_min_time=Device_State_Data.start_time_min;
				Flash_Data.Light_close_hour_time=Device_State_Data.end_time_hour;
				Flash_Data.Light_close_min_time=Device_State_Data.end_time_min;
				
				Judge_Nightlight_Mode();
				
				if(Flag.Brightness_time_flag)//进入夜灯时间
				{
					Device_State_Data.Light_Bright=Flash_Data.Light_Bright_Value;
				}
				else
				{
					Device_State_Data.Light_Bright=Flash_Data.Max_Light_Value;
				}	
			}
			else
			{
				Invalid_Cleanup(&offset);
				break;
			}
		}
		else if (uart0_rx_buf[20+i] == 0x37)//Airer 1
		{
			if((uart0_rx_buf[23+i]<0x30)||(uart0_rx_buf[23+i]>0x39))//ppid<10
			{
				if(uart0_rx_buf[22+i] == 0x32)//电机控制   //Property  0x35-0x35
				{
					UpLoadBuf = "7 2 0 ";
					memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 6);
					Num+=6;
					offset=6;
					switch (uart0_rx_buf[24+i])
					{
						case 0x30:
						{
							Cmd_Motor_Stop();
						}break;		 
						case 0x31:
						{
							if(Up_State_Data.Err_Byte&0x01)
							{
								Up_State_Data.Motor_State_Byte=MOTOR_UP_STATE;//app会点亮图标，如果此时是遇阻，需要上报停止
								Err.err_bit.Encounter_Obstacle=0;
								Time.Encounter_Obstacle_beepnum=0;
								Up_State_Data.Err_Byte&=0xfe;//重新上报app遇阻
							}
							else
							{
								Flag.No_judge_electronic_limit=0;
								if(Device_State_Data.Motor_State_Byte!=MOTOR_UP_STATE)
								{
									Cmd_Motor_Up();
								}
							}
						}break;		
						case 0x32:
						{
							if(Up_State_Data.Err_Byte&0x01)
							{
								Up_State_Data.Motor_State_Byte=MOTOR_DOWN_STATE;//app会点亮图标，如果此时是遇阻，需要上报停止
								Err.err_bit.Encounter_Obstacle=0;
								Time.Encounter_Obstacle_beepnum=0;
								Up_State_Data.Err_Byte&=0xfe;//重新上报app遇阻
							}
							else
							{
								Flag.No_judge_electronic_limit=0;
								if(Device_State_Data.Motor_State_Byte!=MOTOR_DOWN_STATE)
								{
									Cmd_Motor_Down();
								}
							}
						}break;
							
						case 0x33:
						{
							Flag.No_judge_electronic_limit=1;
							Cmd_Motor_Stop();

						}break;
						
						case 0x34:
						{
							if(Ell_Data.Motor_Current_Position>UP_ELE_VALUE)//低于电子上限位最小值
							{
								Flag.No_judge_electronic_limit=1;
								Cmd_Motor_Up();	
							}
						}break;
						case 0x35:
						{
							Flag.No_judge_electronic_limit=1;
							Cmd_Motor_Down();
							
						}break;					
						default:  break;	 	 											 
					}
				}
				else
				{
				 Invalid_Cleanup(&offset);
					break;
				}
			}
			else
			{
				if((uart0_rx_buf[22+i]==0x31)&&(uart0_rx_buf[23+i]==0x32))//运行到指定位置
				{
					
					UpLoadBuf = "7 12 0 ";
					memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 7);
					Num+=7;
					if((uart0_rx_buf[26+i]==0x0d)||(uart0_rx_buf[26+i]==0x20))
					{
						Device_State_Data.set_position=(uart0_rx_buf[25+i]-0x30);	
						offset=7;					
					}
					else if((uart0_rx_buf[27+i]==0x0d)||(uart0_rx_buf[27+i]==0x20))
					{
						Device_State_Data.set_position=(uart0_rx_buf[25+i]-0x30)*10+(uart0_rx_buf[26+i]-0x30);
						offset=8;
					}
					else if((uart0_rx_buf[28+i]==0x0d)||(uart0_rx_buf[28+i]==0x20))
					{
						Device_State_Data.set_position=(uart0_rx_buf[25+i]-0x30)*100+(uart0_rx_buf[26+i]-0x30)*10
									    +uart0_rx_buf[27+i]-0x30;
						offset=9;
					}

					Run_to_position();					
				}
				else if((uart0_rx_buf[22+i]==0x31)&&(uart0_rx_buf[23+i]==0x33))//设置上限位
				{
					UpLoadBuf = "7 13 0 ";
					memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 7);
					Num+=7;		
					if((uart0_rx_buf[26+i]==0x0d)||(uart0_rx_buf[26+i]==0x20))
					{
						Device_State_Data.electronic_up_limit=(uart0_rx_buf[25]-0x30);
						offset=7;
					}
					else if((uart0_rx_buf[27+i]==0x0d)||(uart0_rx_buf[27+i]==0x20))
					{
						Device_State_Data.electronic_up_limit=(uart0_rx_buf[25+i]-0x30)*10+(uart0_rx_buf[26+i]-0x30);
						offset=8;
					}
					else if((uart0_rx_buf[28+i]==0x0d)||(uart0_rx_buf[28+i]==0x20))
					{
						Device_State_Data.electronic_up_limit=(uart0_rx_buf[25+i]-0x30)*100+(uart0_rx_buf[26+i]-0x30)*10
								    +uart0_rx_buf[27+i]-0x30;	
						offset=9;
					}
					Flash_Data.Electronic_Up_Limit=Device_State_Data.electronic_up_limit;	
					if(Flash_Data.Electronic_Up_Limit==0)
					{
						set_up_limit_value=0;//电子上限位对应霍尔脉冲数
					}
					else
					{
						set_up_limit_value=Flash_Data.Electronic_Up_Limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;
					}
					if(Ell_Data.Motor_Current_Position<set_up_limit_value)
					{
						Device_State_Data.Motor_State_Byte=MOTOR_UP_STOP_STATE;
					}
					Time.Up_Limit_Set_time =0;
				}
				else if((uart0_rx_buf[22+i]==0x31)&&(uart0_rx_buf[23+i]==0x34))//设置下限位
				{
					UpLoadBuf = "7 14 0 ";
					memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 7);
					Num+=7;		
					if((uart0_rx_buf[26+i]==0x0d)||(uart0_rx_buf[26+i]==0x20))
					{
						Device_State_Data.electronic_down_limit=(uart0_rx_buf[25]-0x30);
						offset=7;
					}
					else if((uart0_rx_buf[27+i]==0x0d)||(uart0_rx_buf[27+i]==0x20))
					{
						Device_State_Data.electronic_down_limit=(uart0_rx_buf[25+i]-0x30)*10+(uart0_rx_buf[26+i]-0x30);
						offset=8;
					}
					else if((uart0_rx_buf[28+i]==0x0d)||(uart0_rx_buf[28+i]==0x20))
					{
						Device_State_Data.electronic_down_limit=(uart0_rx_buf[25+i]-0x30)*100+(uart0_rx_buf[26+i]-0x30)*10
								    +uart0_rx_buf[27+i]-0x30;	
						offset=9;
					}
					Flash_Data.Electronic_Down_Limit=Device_State_Data.electronic_down_limit;
					set_down_limit_value=Flash_Data.Electronic_Down_Limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;
					Device_State_Data.Motor_State_Byte=MOTOR_DOWN_STOP_STATE;
				}
				else if((uart0_rx_buf[22+i]==0x31)&&(uart0_rx_buf[23+i]==0x35))//A杆收合
				{
					UpLoadBuf = "7 15 0 ";
					memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 7);
					Num+=7;		
					if (uart0_rx_buf[25+i] == 0x74)//打开
					{
						offset=10;
						if (Device_State_Data.convergent == CONVERGENT_OFF)
						{
							Cmd_Motor_Up_Longpress();
						}
					}
					else if(uart0_rx_buf[25+i] == 0x66)
					{
						offset=11;
						if (Device_State_Data.convergent == CONVERGENT_ON)
						{
							Cmd_Motor_Stop();
						}						
					}	
				}
				else
				{
				
					break;
				}
			}
		}
		else if (uart0_rx_buf[20+i] == 0x36)//Airer 2
		{
			if((uart0_rx_buf[23+i]<0x30)||(uart0_rx_buf[23+i]>0x39))//ppid<10
			{
				if(uart0_rx_buf[22+i] == 0x32)//电机控制
				{
					UpLoadBuf = "6 2 0 ";
					memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 6);
					Num+=6;
					offset=6;
					switch (uart0_rx_buf[24+i])
					{
						case 0x30:
						{
							Cmd_Motor_Stop_B();
						}break;		 
						case 0x31:
						{
							if(Up_State_Data.B_Err_Byte&0x01)
							{
								Up_State_Data.B_Motor_State_Byte=MOTOR_UP_STATE;//app会点亮图标，如果此时是遇阻，需要上报停止
								Err_B.err_bit.Encounter_Obstacle=0;
								Time.B_Encounter_Obstacle_beepnum=0;
								Up_State_Data.B_Err_Byte&=0xfe;//重新上报app遇阻
							}
							else
							{
								Flag.B_No_judge_electronic_limit=0;
								if(Device_State_Data.B_Motor_State_Byte!=MOTOR_UP_STATE)
								{
									Cmd_Motor_Up_B();
								}
							}
						}break;		
						case 0x32:
						{
							if(Up_State_Data.B_Err_Byte&0x01)
							{
								Up_State_Data.B_Motor_State_Byte=MOTOR_DOWN_STATE;//app会点亮图标，如果此时是遇阻，需要上报停止
								Err_B.err_bit.Encounter_Obstacle=0;
								Time.B_Encounter_Obstacle_beepnum=0;
								Up_State_Data.B_Err_Byte&=0xfe;//重新上报app遇阻
							}
							else
							{
								Flag.B_No_judge_electronic_limit=0;
								if(Device_State_Data.B_Motor_State_Byte!=MOTOR_DOWN_STATE)
								{
									Cmd_Motor_Down_B();
								}
							}
						}break;
							
						case 0x33:
						{
							Flag.B_No_judge_electronic_limit=1;
							Cmd_Motor_Stop_B();
						}break;
						
						case 0x34:
						{
							if(Ell_Data.B_Motor_Current_Position>UP_ELE_VALUE)//低于电子上限位最小值
							{
								Flag.B_No_judge_electronic_limit=1;
								Cmd_Motor_Up_B();	
							}
						}break;
						case 0x35:
						{
							Flag.B_No_judge_electronic_limit=1;
							Cmd_Motor_Down_B();	
						}break;					
						default:  break;	 	 											 
					}
				}
				else
				{
				  Invalid_Cleanup(&offset);
					break;
				}
			}
			else
			{
				if((uart0_rx_buf[22+i]==0x31)&&(uart0_rx_buf[23+i]==0x32))////ppid>10运行到指定位置
				{
					UpLoadBuf = "6 12 0 ";
					memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 7);
					Num+=7;
					if((uart0_rx_buf[26+i]==0x0d)||(uart0_rx_buf[26+i]==0x20))
					{
						Device_State_Data.B_set_position=(uart0_rx_buf[25+i]-0x30);	
						offset=7;					
					}
					else if((uart0_rx_buf[27+i]==0x0d)||(uart0_rx_buf[27+i]==0x20))
					{
						Device_State_Data.B_set_position=(uart0_rx_buf[25+i]-0x30)*10+(uart0_rx_buf[26+i]-0x30);
						offset=8;
					}
					else if((uart0_rx_buf[28+i]==0x0d)||(uart0_rx_buf[28+i]==0x20))
					{
						Device_State_Data.B_set_position=(uart0_rx_buf[25+i]-0x30)*100+(uart0_rx_buf[26+i]-0x30)*10
									    +uart0_rx_buf[27+i]-0x30;
						offset=9;
					}
					Run_to_position_B();	
          Time.Up_Limit_Set_time =0;					
				}
				else if((uart0_rx_buf[22+i]==0x31)&&(uart0_rx_buf[23+i]==0x33))//设置上限位
				{
					UpLoadBuf = "6 13 0 ";
					memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 7);
					Num+=7;		
					if((uart0_rx_buf[26+i]==0x0d)||(uart0_rx_buf[26+i]==0x20))
					{
						Device_State_Data.B_electronic_up_limit=(uart0_rx_buf[25]-0x30);
						offset=7;
					}
					else if((uart0_rx_buf[27+i]==0x0d)||(uart0_rx_buf[27+i]==0x20))
					{
						Device_State_Data.B_electronic_up_limit=(uart0_rx_buf[25+i]-0x30)*10+(uart0_rx_buf[26+i]-0x30);
						offset=8;
					}
					else if((uart0_rx_buf[28+i]==0x0d)||(uart0_rx_buf[28+i]==0x20))
					{
						Device_State_Data.B_electronic_up_limit=(uart0_rx_buf[25+i]-0x30)*100+(uart0_rx_buf[26+i]-0x30)*10
								    +uart0_rx_buf[27+i]-0x30;	
						offset=9;
					}
					Flash_Data.B_electronic_up_limit=Device_State_Data.B_electronic_up_limit;	
					if(Flash_Data.B_electronic_up_limit==0)
					{
						B_set_up_limit_value=0;//电子上限位对应霍尔脉冲数
					}
					else
					{
						B_set_up_limit_value=Flash_Data.B_electronic_up_limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;
					}
					if(Ell_Data.B_Motor_Current_Position<B_set_up_limit_value)
					{
						Device_State_Data.B_Motor_State_Byte=MOTOR_UP_STOP_STATE;
					}
						
				}
				else if((uart0_rx_buf[22+i]==0x31)&&(uart0_rx_buf[23+i]==0x34))//设置下限位
				{
					UpLoadBuf = "6 14 0 ";
					memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 7);
					Num+=7;		
					if((uart0_rx_buf[26+i]==0x0d)||(uart0_rx_buf[26+i]==0x20))
					{
						Device_State_Data.B_electronic_down_limit=(uart0_rx_buf[25]-0x30);
						offset=7;
					}
					else if((uart0_rx_buf[27+i]==0x0d)||(uart0_rx_buf[27+i]==0x20))
					{
						Device_State_Data.B_electronic_down_limit=(uart0_rx_buf[25+i]-0x30)*10+(uart0_rx_buf[26+i]-0x30);
						offset=8;
					}
					else if((uart0_rx_buf[28+i]==0x0d)||(uart0_rx_buf[28+i]==0x20))
					{
						Device_State_Data.B_electronic_down_limit=(uart0_rx_buf[25+i]-0x30)*100+(uart0_rx_buf[26+i]-0x30)*10
								    +uart0_rx_buf[27+i]-0x30;	
						offset=9;
					}
					Flash_Data.B_electronic_down_limit=Device_State_Data.B_electronic_down_limit;
					B_set_down_limit_value=Flash_Data.B_electronic_down_limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;
					Device_State_Data.B_Motor_State_Byte=MOTOR_DOWN_STOP_STATE;
				}
				else if((uart0_rx_buf[22+i]==0x31)&&(uart0_rx_buf[23+i]==0x35))//B杆收合
				{
					UpLoadBuf = "6 15 0 ";
					memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 7);
					Num+=7;		
					if (uart0_rx_buf[25+i] == 0x74)//打开
					{
						offset=10;
						if (Device_State_Data.B_convergent == CONVERGENT_OFF)
						{
							Cmd_Motor_Up_Longpress_B();
						}
					}
					else if(uart0_rx_buf[25+i] == 0x66)
					{
						offset=11;
						if (Device_State_Data.B_convergent == CONVERGENT_ON)
						{
							Cmd_Motor_Stop_B();
						}						
					}	
				}

				else
				{
					Invalid_Cleanup(&offset);
					break;
				}
			}
		}
		/*遥控器设置*/
		else if (uart0_rx_buf[20+i] == 0x38) //Remote Control Management
		{
		 	if((uart0_rx_buf[23+i]<0x31))//开启遥控器添码使能
			{
					UpLoadBuf = "8 1 0 ";
					memcpy((uint8_t *)uart0_tx_buf+Num, UpLoadBuf, 6);
					Num+=6;
					offset=6;
					if( !memcmp(uart0_rx_buf + 24, "false", 5) )
					{
		         Device_State_Data.Control_open=0;
				
					} else
					{
			      Flag.Add_Match_Code_flag=1;
						Device_State_Data.Control_open=1;
					}
			//	 SetupBzhx(ADD_MATCH_CODE_BEEP,Flash_Data.Beep_open);
			   Flash_Data.Control_open=Device_State_Data.Control_open;
			}
 
		}
		else
		{
			Invalid_Cleanup(&offset);
		  break;
		}
		if(Num>170)
		{
			Num=0;
			r_uart0_send_bytes((uint8_t *)uart0_tx_buf, sizeof(error5001));	
			memset(uart0_tx_buf, 0, sizeof(uart0_tx_buf));
			return ;
		}
		i+=offset;
	}
		uart0_tx_buf[Num-1] = 0x0d;
		r_uart0_send_bytes((uint8_t *)uart0_tx_buf, Num);	
		memset(uart0_tx_buf, 0, sizeof(uart0_tx_buf));
		g_MiioRoundFlag = 1;
		Flag.time150ms_flag=0;
		Time.time150ms=150;
		//上报属性
		properties_up();
	
}

void Run_to_position(void)
{	
	if(Device_State_Data.set_position!=Device_State_Data.Motor_Position)//运行目标位置不等于当前位置
	{
		if(Device_State_Data.set_position==0)
		{
			set_position_pulse=0;
			if(Device_State_Data.set_position<Device_State_Data.electronic_up_limit)
			{
				set_position_pulse=Device_State_Data.electronic_up_limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;	
				if(Device_State_Data.Motor_Position==Device_State_Data.electronic_up_limit)
				{
					set_position_pulse=Ell_Data.Motor_Current_Position;
				}
				Device_State_Data.set_position=Device_State_Data.electronic_up_limit;//设置位置变更为上限位
				Up_State_Data.set_position=0XFF;//重新上报
				Up_State_Data.Motor_Position=0xff;
			}		
		}
		else
		{
			set_position_pulse=Device_State_Data.set_position*ONE_PERCENT_PULSE+UP_POSITION_VALUE;
			if(Device_State_Data.set_position>Device_State_Data.electronic_down_limit)
			{
				set_position_pulse=Device_State_Data.electronic_down_limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;	
				if(Device_State_Data.Motor_Position==Device_State_Data.electronic_down_limit)
				{
					set_position_pulse=Ell_Data.Motor_Current_Position;
				}
				Device_State_Data.set_position=Device_State_Data.electronic_down_limit;//设置位置变更为下限位
				Up_State_Data.set_position=0XFF;//
				Up_State_Data.Motor_Position=0xff;
			}
			else if(Device_State_Data.set_position<Device_State_Data.electronic_up_limit)
			{
				set_position_pulse=Device_State_Data.electronic_up_limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;	
				if(Device_State_Data.Motor_Position==Device_State_Data.electronic_up_limit)
				{
					set_position_pulse=Ell_Data.Motor_Current_Position;
				}	
				Device_State_Data.set_position=Device_State_Data.electronic_up_limit;
				Up_State_Data.set_position=0XFF;
				Up_State_Data.Motor_Position=0xff;
			}		
		}
		if(set_position_pulse<Ell_Data.Motor_Current_Position)//需要上升
		{
			if(Err.err_bit.Encounter_Obstacle==1)//遇阻状态重新报遇阻提示音
			{
				Err.err_bit.Encounter_Obstacle=0;
				Time.Encounter_Obstacle_beepnum=0;
				Up_State_Data.Err_Byte&=0xfe;//重新上报遇阻
				
				Device_State_Data.set_position=Device_State_Data.Motor_Position;
				Up_State_Data.set_position=0XFF;
				Up_State_Data.Motor_Position=0xff;
			}
			else
			{
				Flag.Run_to_set_position_flag=1;
				if((Device_State_Data.Motor_State_Byte==MOTOR_UP_STOP_STATE)
			      	||(Device_State_Data.Motor_State_Byte==MOTOR_SUSPEND_STATE)
				||(Device_State_Data.Motor_State_Byte==MOTOR_DOWN_STOP_STATE)||(Device_State_Data.Motor_State_Byte==MOTOR_UPLIM_STATE))//当前是暂停状态
				{
			
						Device_State_Data.Motor_State_Byte=MOTOR_UP_STATE;
						SetupBzhx(MOTOR_UP_BEEP,Flash_Data.Beep_open);
						Flag.Motor_run_now=0;
				}
				else if(Device_State_Data.Motor_State_Byte==MOTOR_DOWN_STATE)//当前是下降状态，换向
				{
					Device_State_Data.Motor_State_Byte=MOTOR_UP_STATE;
					Flag.Motor_run_now=0;
					//SetupBzhx(MOTOR_UP_BEEP);//换向完再响蜂鸣器
					Flag.Motor_down_to_up=1;
				}
				else if(Device_State_Data.Motor_State_Byte==MOTOR_UP_STATE)//当前是上升
				{
					SetupBzhx(MOTOR_UP_BEEP,Flash_Data.Beep_open);
				}
			}
		}
		else if(set_position_pulse>Ell_Data.Motor_Current_Position)//需要下降
		{
			if(Err.err_bit.Encounter_Obstacle==1)//遇阻状态重新报遇阻提示音
			{
				Err.err_bit.Encounter_Obstacle=0;
				Time.Encounter_Obstacle_beepnum=0;
				Up_State_Data.Err_Byte&=0xfe;//重新上报遇阻
				
				Device_State_Data.set_position=Device_State_Data.Motor_Position;//设置位置不成功，
				Up_State_Data.set_position=0XFF;//重新上报
				Up_State_Data.Motor_Position=0xff;//
			}
			else
			{
				Flag.Run_to_set_position_flag=1;
				if((Device_State_Data.Motor_State_Byte==MOTOR_UP_STOP_STATE)
			      	||(Device_State_Data.Motor_State_Byte==MOTOR_SUSPEND_STATE)
				||(Device_State_Data.Motor_State_Byte==MOTOR_DOWN_STOP_STATE)||(Device_State_Data.Motor_State_Byte==MOTOR_UPLIM_STATE))//当前是暂停状态
				{
					Device_State_Data.Motor_State_Byte=MOTOR_DOWN_STATE;
					SetupBzhx(MOTOR_DOWN_BEEP,Flash_Data.Beep_open);
					Flag.Motor_run_now=0;
				}
				else if(Device_State_Data.Motor_State_Byte==MOTOR_UP_STATE)//当前是上升状态，先停止
				{
					Device_State_Data.Motor_State_Byte=MOTOR_DOWN_STATE;
					//SetupBzhx(MOTOR_DOWN_BEEP);//换向完再响蜂鸣器	
					Flag.Motor_up_to_down=1;
					Flag.Motor_run_now=0;
				}
				else if(Device_State_Data.Motor_State_Byte==MOTOR_DOWN_STATE)//当前是下降
				{
					SetupBzhx(MOTOR_DOWN_BEEP,Flash_Data.Beep_open);
				}
			}
		}
	}
	else//暂停
	{
		if((Device_State_Data.Motor_State_Byte==MOTOR_UP_STOP_STATE)
	      	||(Device_State_Data.Motor_State_Byte==MOTOR_SUSPEND_STATE)
		||(Device_State_Data.Motor_State_Byte==MOTOR_DOWN_STOP_STATE)||(Device_State_Data.Motor_State_Byte==MOTOR_UPLIM_STATE))
		{

		}
		else
		{
			SetupBzhx(MOTOR_STOP_BEEP,Flash_Data.Beep_open);
			Device_State_Data.Motor_State_Byte=MOTOR_SUSPEND_STATE;
			Flag.Motor_run_now=0;		
		}
	}
}

void Run_to_position_B(void)
{	
	if(Device_State_Data.B_set_position!=Device_State_Data.B_Motor_Position)//运行目标位置不等于当前位置
	{
		if(Device_State_Data.B_set_position==0)
		{
			set_position_pulse_B=0;
			if(Device_State_Data.B_set_position<Device_State_Data.B_electronic_up_limit)
			{
				set_position_pulse_B=Device_State_Data.B_electronic_up_limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;	
				if(Device_State_Data.B_Motor_Position==Device_State_Data.B_electronic_up_limit)
				{
					set_position_pulse_B=Ell_Data.B_Motor_Current_Position;
				}
				Device_State_Data.B_set_position=Device_State_Data.B_electronic_up_limit;//设置位置变更为上限位
				Up_State_Data.B_set_position=0XFF;//重新上报
				Up_State_Data.B_Motor_Position=0xff;
			}		
		}
		else
		{
			set_position_pulse_B=Device_State_Data.B_set_position*ONE_PERCENT_PULSE+UP_POSITION_VALUE;
			if(Device_State_Data.B_set_position>Device_State_Data.B_electronic_down_limit)
			{
				set_position_pulse_B=Device_State_Data.B_electronic_down_limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;	
				if(Device_State_Data.B_Motor_Position==Device_State_Data.B_electronic_down_limit)
				{
					set_position_pulse_B=Ell_Data.B_Motor_Current_Position;
				}
				Device_State_Data.B_set_position=Device_State_Data.B_electronic_down_limit;//设置位置变更为下限位
				Up_State_Data.B_set_position=0XFF;//
				Up_State_Data.B_Motor_Position=0xff;
			}
			else if(Device_State_Data.B_set_position<Device_State_Data.B_electronic_up_limit)
			{
				set_position_pulse_B=Device_State_Data.B_electronic_up_limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;	
				if(Device_State_Data.B_Motor_Position==Device_State_Data.B_electronic_up_limit)
				{
					set_position_pulse_B=Ell_Data.B_Motor_Current_Position;
				}	
				Device_State_Data.B_set_position=Device_State_Data.B_electronic_up_limit;
				Up_State_Data.B_set_position=0XFF;
				Up_State_Data.B_Motor_Position=0xff;
			}		
		}
		if(set_position_pulse_B<Ell_Data.B_Motor_Current_Position)//需要上升
		{
			if(Err_B.err_bit.Encounter_Obstacle==1)//遇阻状态重新报遇阻提示音
			{
				Err_B.err_bit.Encounter_Obstacle=0;
				Time.B_Encounter_Obstacle_beepnum=0;
				Up_State_Data.B_Err_Byte&=0xfe;//重新上报遇阻
				
				Device_State_Data.B_set_position=Device_State_Data.B_Motor_Position;
				Up_State_Data.B_set_position=0XFF;
				Up_State_Data.B_Motor_Position=0xff;
			}
			else
			{
				Flag.B_Run_to_set_position_flag=1;
				if((Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STOP_STATE)
			      	||(Device_State_Data.B_Motor_State_Byte==MOTOR_SUSPEND_STATE)
				||(Device_State_Data.B_Motor_State_Byte==MOTOR_DOWN_STOP_STATE)	||(Device_State_Data.B_Motor_State_Byte==MOTOR_UPLIM_STATE))//当前是暂停状态
				{
					SetupBzhx(MOTOR_UP_BEEP,Flash_Data.Beep_open);
					Device_State_Data.B_Motor_State_Byte=MOTOR_UP_STATE;
					Flag.B_Motor_run_now=0;
				}
				else if(Device_State_Data.B_Motor_State_Byte==MOTOR_DOWN_STATE)//当前是下降状态，换向
				{
					Device_State_Data.B_Motor_State_Byte=MOTOR_UP_STATE;
					Flag.B_Motor_run_now=0;
					//SetupBzhx(MOTOR_UP_BEEP);//换向完再响蜂鸣器
					Flag.B_Motor_down_to_up=1;
				}
				else if(Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STATE)//当前是上升
				{
					SetupBzhx(MOTOR_UP_BEEP,Flash_Data.Beep_open);
				}
			}
		}
		else if(set_position_pulse_B>Ell_Data.B_Motor_Current_Position)//需要下降
		{
			if(Err_B.err_bit.Encounter_Obstacle==1)//遇阻状态重新报遇阻提示音
			{
				Err_B.err_bit.Encounter_Obstacle=0;
				Time.B_Encounter_Obstacle_beepnum=0;
				Up_State_Data.B_Err_Byte&=0xfe;//重新上报遇阻
				
				Device_State_Data.B_set_position=Device_State_Data.B_Motor_Position;//设置位置不成功，
				Up_State_Data.B_set_position=0XFF;//重新上报
				Up_State_Data.B_Motor_Position=0xff;//
			}
			else
			{
				Flag.B_Run_to_set_position_flag=1;
				if((Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STOP_STATE)
			      	||(Device_State_Data.B_Motor_State_Byte==MOTOR_SUSPEND_STATE)
				||(Device_State_Data.B_Motor_State_Byte==MOTOR_DOWN_STOP_STATE)	||(Device_State_Data.B_Motor_State_Byte==MOTOR_UPLIM_STATE))//当前是暂停状态
				{
					Device_State_Data.B_Motor_State_Byte=MOTOR_DOWN_STATE;
					SetupBzhx(MOTOR_DOWN_BEEP,Flash_Data.Beep_open);
					Flag.B_Motor_run_now=0;
				}
				else if(Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STATE)//当前是上升状态，先停止
				{
					Device_State_Data.B_Motor_State_Byte=MOTOR_DOWN_STATE;
					//SetupBzhx(MOTOR_DOWN_BEEP);//换向完再响蜂鸣器	
					Flag.B_Motor_up_to_down=1;
					Flag.B_Motor_run_now=0;
				}
				else if(Device_State_Data.B_Motor_State_Byte==MOTOR_DOWN_STATE)//当前是下降
				{
					SetupBzhx(MOTOR_DOWN_BEEP,Flash_Data.Beep_open);
				}
			}
		}
	}
	else//暂停
	{
		if((Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STOP_STATE)
	      	||(Device_State_Data.B_Motor_State_Byte==MOTOR_SUSPEND_STATE)
		||(Device_State_Data.B_Motor_State_Byte==MOTOR_DOWN_STOP_STATE)	||(Device_State_Data.B_Motor_State_Byte==MOTOR_UPLIM_STATE))
		{

		}
		else
		{
			SetupBzhx(MOTOR_STOP_BEEP,Flash_Data.Beep_open);
			Device_State_Data.B_Motor_State_Byte=MOTOR_SUSPEND_STATE;
			Flag.B_Motor_run_now=0;		
		}
	}
}
void UpMCUversion(void)
{
	 memcpy((uint8_t *)uart0_tx_buf, MCUversion, sizeof(MCUversion));
	 r_uart0_send_bytes((uint8_t *)uart0_tx_buf, sizeof(MCUversion));//上报版本	
}
//Invalid_Cleanup(&offset);
void Invalid_Cleanup(uint8_t *i)
{
		//*i=Frame_length;
		//r_uart0_send_bytes((uint8_t *)uart0_tx_buf, sizeof(error5002));	
		//memset(uart0_tx_buf, 0, sizeof(uart0_tx_buf));
}




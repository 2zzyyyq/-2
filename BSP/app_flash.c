#include "ALL_Var.h"
#include "adc.h"

void match_data_del(void);
uint16_t set_light_value;//开灯时灯实时亮度
uint8_t Read_FLASH_Data(uint32_t Addr);
uint16_t Read_FLASH_MUIData(uint32_t Addr, uint8_t *pBuf, uint16_t LEN);//读取多字节
uint16_t  Synchronous_Count_value[3];//同步计数值
volatile uint8_t  FDL_Block0_Save_Times,FDL_Block1_Save_Times;//flash_data存储次数，
void read_data(void);
void _220AC_Power_down_save(void);//掉电保存
void limit_protect_B(void);//
void limit_protect(void);//
void alarm_signal_test_and_beep(void);
void alarm_signal_test_and_beep_B(void);
//硬件自检
void PCBA_Self_Inspection(void);
uint8_t Flash_WriteStruct(uint32_t u32StartAddr, void *pData, uint16_t size);
uint16_t ADC_sample_max,ADC_sample_min,ADC_sample_max_yz,ADC_sample_min_yz;
uint16_t ADC_sample_max_B,ADC_sample_min_B,ADC_sample_max_yz_B,ADC_sample_min_yz_B;
uint16_t ADC_read_buf[AD_BUFF_DIP],ADC_read_buf_B[AD_BUFF_DIP];


uint8_t ADC_sample_num,min_serial_num,max_serial_num;//
uint8_t ADC_sample_num_B,min_serial_num_B,max_serial_num_B;//
uint8_t Self_Inspection_step;//自检步骤

uint16_t Holl_pulse_temp,Holl_test_speed_temp_B;
uint8_t ADC_sample_num_yz,min_serial_num_yz,max_serial_num_yz;//
uint8_t ADC_sample_num_yz_B,min_serial_num_yz_B,max_serial_num_yz_B;//

uint8_t conform_motor_obs_num,conform_motor_obs_num2;//
uint8_t conform_motor_obs_num_B,conform_motor_obs_num2_B;//

//uint8_t UP_state_obs,UP_state_obsB;

uint8_t Flag_TOP_ods;//区分一级限位 和二级之间的关系
uint8_t Flag_TOP_odsB;
 
const uint8_t MOTOR_OBS_CURRENT_ADVALUE[3]={45*4,20*4,10*4};//判断为上升遇到障碍的最大最小AD差值
uint8_t A_obs_correction,B_obs_correction;
float temp1;
float over_weight_current;
float addvalue_A,addvalue_B;
// 假设的Flash参数
#define FLASH_SECTOR_SIZE  512  // 假设扇区大小为512字节，根据实际芯片调整
#define FLASH_START_ADDR   0x0000
#define FLASH_END_ADDR     0xFFFF

 
/**
 * @brief 将结构体写入Flash存储器
 * @param u32StartAddr Flash起始地址(必须位于扇区起始地址)
 * @param pData 指向要写入的结构体数据
 * @param size 要写入的数据大小(字节)
 * @return 写入结果: Ok(0)或Error(1)
 */
uint8_t Flash_WriteStruct(uint32_t u32StartAddr, void *pData, uint16_t size)
{
    uint8_t *pByteData = (uint8_t *)pData;
    uint32_t u32Addr = u32StartAddr;
    uint8_t ret = Ok;

    ///< FLASH目标扇区擦除
//    while(Ok != Flash_SectorErase(u32Addr))
//    {
//        // 可以增加超时机制，避免无限循环
//    }
		if(u32StartAddr+size>0x10000)
		{

		ret = Error;  // 校验失败

		 return ret;

		}
    for (uint32_t i = 0; i < size; i++, u32Addr++, pByteData++)
    {
        ///< FLASH 字节写
        if (Ok != Flash_WriteByte(u32Addr, *pByteData))
        {
            ret = Error;  // 写入失败
            break;
        }

        ///< 校验写入的数据是否正确
        if (*((volatile uint8_t*)u32Addr) != *pByteData)
        {
            ret = Error;  // 校验失败
            break;
        }
    }

    return ret;
}



 
//读一个字节操作
 uint8_t Read_FLASH_Data(uint32_t Addr)
{
	uint8_t Dat=0;
	
	Dat = *((volatile uint8_t*)Addr);
	
	return Dat;
}
// EEPROM 读多个字节操作
// Addr: 起始地址
// pBuf: 存储读取数据的缓冲区
// LEN: 要读取的字节数
// 返回值: 实际读取的字节数（可用于错误检查）
 uint16_t Read_FLASH_MUIData(uint32_t Addr, uint8_t *pBuf, uint16_t LEN)
{
    uint16_t i;
    
    if (pBuf == NULL || LEN == 0) {
        return 0;  // 参数错误
    }
    
    for (i = 0; i < LEN; i++) {
		
        pBuf[i] = *((volatile uint8_t*)(Addr + i));  // 逐个字节读取

    }
    
    return i;  // 返回实际读取的字节数
}


void match_data_clear(void);
void save_data(void);
void save_current_position(void);

/*遥控器清码*/
void match_data_clear(void)
{
	Flash_Data.Remote_Control_Num=0;
	
	Flash_Data.Remote_Control_Id[0]= Capture_Command.Identify_data;//ID码
	Flash_Data.Remote_Control_Key[0]=Capture_Command.Secret_Key_data;//密钥
	Synchronous_Count_value[0]=0;//同步计数值
	
	Flash_Data.Remote_Control_Id[1]= Capture_Command.Identify_data;//ID码
	Flash_Data.Remote_Control_Key[1]=Capture_Command.Secret_Key_data;//密钥
	Synchronous_Count_value[1]=0;//同步计数值
	
	Flash_Data.Remote_Control_Id[2]= Capture_Command.Identify_data;//ID码
	Flash_Data.Remote_Control_Key[2]=Capture_Command.Secret_Key_data;//密钥
	Synchronous_Count_value[2]=0;//同步计数值
	
	Flash_WriteStruct(FDL_Block0+FDL_Block0_Save_Times*sizeof(Flash_Data),(uint8_t*)&Flash_Data,sizeof(Flash_Data));//写入一次	
	Device_State_Data.Control_num=Flash_Data.Remote_Control_Num;
}
//存储块0
void save_data(void)
{	
	
	if(FDL_Block0_Save_Times>=FDL_ONCE_MAX_WRITE_NUM-2)//fdl区存满了，最后一块区留给掉电保存
	{
	
			Flash_SectorErase(FDL_Block0);
		  FDL_Block0_Save_Times=0;
	}
	else
	{
		FDL_Block0_Save_Times++;	
	}
	
   Flash_WriteStruct(FDL_Block0+FDL_Block0_Save_Times*sizeof(Flash_Data),(uint8_t*)&Flash_Data,sizeof(Flash_Data));//写入一次	
}
//存储块1
void save_current_position(void)
{	 
	if(FDL_Block1_Save_Times>=FDL_ONCE_MAX_WRITE_NUM-2)//fdl区存满了，最后一块区留给掉电保存
		{
		
				Flash_SectorErase(FDL_Block1);
				FDL_Block1_Save_Times=0;
		}
		else
		{
			FDL_Block1_Save_Times++;	
		}
 	Flash_WriteStruct(FDL_Block1+FDL_Block1_Save_Times*sizeof(Ell_Data),(uint8_t*)&Ell_Data,sizeof(Ell_Data));//并写入一次
}

/*遥控器对码及添码处理*/
void match_data_del(void)
{
	
	Up_State_Data.Control_num=0;
	uint8_t i;	
	for(i=0;i<Flash_Data.Remote_Control_Num;i++)
	{
		if((Capture_Command.Identify_data==Flash_Data.Remote_Control_Id[i])//已经存在遥控器
		&&(Capture_Command.Secret_Key_data==Flash_Data.Remote_Control_Key[i]))
		{
			return;
		}	
	}
if(FDL_Block0_Save_Times>=FDL_ONCE_MAX_WRITE_NUM-2)//fdl区存满了
	{		
	while(Flash_SectorErase(FDL_Block0) != 0)
       FDL_Block0_Save_Times=0;
	}
	else
	{
    	FDL_Block0_Save_Times++;
  }
	if(Flash_Data.Remote_Control_Num>=3)
	{
		for(i=0;i<2;i++)
		{
			Flash_Data.Remote_Control_Id[i]=Flash_Data.Remote_Control_Id[i+1];//ID
			Flash_Data.Remote_Control_Key[i]=Flash_Data.Remote_Control_Key[i+1];//密钥
			Synchronous_Count_value[i]=Synchronous_Count_value[i+1];//同步计数值
		}
		Flash_Data.Remote_Control_Id[2]= Capture_Command.Identify_data;//ID码
		Flash_Data.Remote_Control_Key[2]=Capture_Command.Secret_Key_data;//密钥
		Synchronous_Count_value[2]=0;//同步计数值
	}
	else 
	{			
		Flash_Data.Remote_Control_Id[Flash_Data.Remote_Control_Num]= Capture_Command.Identify_data;//ID码
		Flash_Data.Remote_Control_Key[Flash_Data.Remote_Control_Num]=Capture_Command.Secret_Key_data;//密钥
		Synchronous_Count_value[Flash_Data.Remote_Control_Num]=0;
		Flash_Data.Remote_Control_Num++;	
	}		
	Flash_WriteStruct(FDL_Block0+FDL_Block0_Save_Times*sizeof(Flash_Data),(uint8_t*)&Flash_Data,sizeof(Flash_Data));//写入一次		
  Device_State_Data.Control_num=Flash_Data.Remote_Control_Num;
}
//掉电保存
void _220AC_Power_down_save(void)//掉电保存
{
	if(!Flag.Already_save_data_flag)//未进行掉电保存
	{
		if(Flag._220AC_Power_Down_flag)
		{
				BuzVcc_OFF;
				BUZ_OFF;
				//Tim3_M23_Stop();                            //运行。
				//Adt_StopCount(M0P_ADTIM6); 
				//LED24V_IO2(0);
				//  LED24V_IO(0);
				MOTOR_DN_IO(0);//停止电机
				MOTOR_UP_IO(0);
				B_MOTOR_DN_IO(0);
				B_MOTOR_UP_IO(0);

				Flag.Already_save_data_flag=1;//已掉电保存一次
				Flag._220AC_Power_Down_flag=1;
		}
	}
}


//硬件自检
void PCBA_Self_Inspection(void)
{
	 static uint16_t  time=0;//3s
	 static uint16_t  time_1min=0;
	 M0P_ADTIM_TypeDef *ADTx= M0P_ADTIM6;
	while(1)
	{
     Wdt_Feed();
		if(Flag.time10ms_flag)
		{
			Flag.time10ms_flag=0;
			CmdBzhx();//蜂鸣器动作
			if(time>0)
			{
				time--;
			}
			time_1min++;
			if((time_1min>300)&&(Self_Inspection_step!=7))
			{

				Self_Inspection_step=7;
			}
		}		
		switch(Self_Inspection_step)
		{
			case 0:
			{
				 if((!YZ)&&(!UP_LIMIT)&&(!HOLL_DIR)&&(!HOLL_NUM)&&(!YZ_B)&&(!B_UP_LIMIT)&&(!B_HOLL_DIR)&&(!B_HOLL_NUM))
				{
					Delay1ms(5);
					if((!YZ)&&(!UP_LIMIT)&&(!HOLL_DIR)&&(!HOLL_NUM)&&(!YZ_B)&&(!B_UP_LIMIT)&&(!B_HOLL_DIR)&&(!B_HOLL_NUM))
					{
						time=100;
						Self_Inspection_step = 1;
						LED_LIGHT_ON;
						M0P_TIM3_MODE23->CCR1A_f.CCR1A=3200;
						LIGHT_WHITE_VALUE=3200;
					}
				}
			}break;	
			case 1:
			{     			
				if(time==0)
				{
				   LED_LIGHT_OFF;
					 Self_Inspection_step=2;
					 time=100;
					 motor_down();//电机下行
					 motor_down_B();//电机下行
				}					
				time_1min=0;
			}break;
			case 2:
			{		
				if(time==0)
				{
					Self_Inspection_step=3;
					motor_stop();//电机停止
					motor_stop_B();//电机停止
					time=50;
				}
				time_1min=0;
			}break;
			
			case 3:
			{
				if(time==0)
				{
					Self_Inspection_step=4;
					time=100;
					motor_up();//电机上行
					motor_up_B();//电机上行
				}
				time_1min=0;
			}break;
			case 4:
			{	
				if(time==0)
				{
					Self_Inspection_step=5;
					motor_stop();//电机停止
					motor_stop_B();//电机停止
					r_uart0_protocol_init();//清除wifi模组接收缓存
			   	UpMCUversion();//上报版本
					time=10;
				}
				time_1min=0;
			}break;		
			case 5:
			{	
				r_uart0_service();
				if(time==0)
				{
					if(Pcba_test.wifi_test_flag==1)
					{
						Self_Inspection_step=6;//自检完成
						time_1min=0;
						time=200;
 
						SetupBzhx(LONG_BEEP,1);	
					}
					else
					{
				   	UpMCUversion();
						time=10;
						Time.Uart_Receive_Interval_time=0;
					}
				}
			}break;
			
			case 6:
			{	
				time_1min=0;
				if(time==0)
				{
					SetupBzhx(LONG_BEEP,1);
					time=100;
				}
				
			}break;
			case 7:
			{			
	      App_AdvTimerInit(FREQ_1288,FREQ_1288/2);    //蜂鸣器  PWM控制输出50%   频率控制 10kz    				
				BUZ_ON;
				BuzVcc_ON; 
				while(1)
				{
							 Wdt_Feed();
				}
			}break;
		}
	}
}


/*读取存储数据*/
void read_data(void)
{
    uint16_t i = 0;

    /* 读取块0的Flash数据 */
    for(i = FDL_ONCE_MAX_WRITE_NUM; i > 0; i--)
    {
        Read_FLASH_MUIData(FDL_Block0 + (i - 1) * sizeof(Flash_Data), (uint8_t*)&Flash_Data, sizeof(Flash_Data));
        if(Flash_Data.Check == 0x5AA5)
        {
            if(i == FDL_ONCE_MAX_WRITE_NUM)//flash，满了需要擦除
            {
							__disable_irq();
                Flash_SectorErase(FDL_Block0);//block 0
							
                FDL_Block0_Save_Times = 0;
                Flash_WriteStruct(FDL_Block0, (uint8_t*)&Flash_Data, sizeof(Flash_Data));//并写入一次	
							__enable_irq();
            }
            else
            {
                FDL_Block0_Save_Times = i - 1;//
            }
            break;
        }
    }
    
    /* 如果块0无数据或出错，初始化默认值 */
    if((i == 0)||Flash_Data.Max_Light_Value>100)//内存块0无数据或出错
    {
			__disable_irq();
        Flash_SectorErase(FDL_Block0);//block 0
			__enable_irq();
        Flash_Data.Check = 0X5AA5;
        
        Flash_Data.Remote_Control_Num = 0;
        Flash_Data.Max_Light_Value = 100;

        Flash_Data.Electronic_Down_Limit = 100;
        Flash_Data.B_electronic_down_limit = 100;
        
        Flash_Data.Overweight_Correct = 125;//
        Flash_Data.B_Overweight_Correct = 125;//
        
        Flash_Data.Electronic_Up_Limit = UP_L;
        Flash_Data.B_electronic_up_limit = UP_L;
        
        FDL_Block0_Save_Times = 0;
        
				Flash_Data.Light_open_hour_time=23;
				Flash_Data.Light_open_min_time=0;
				Flash_Data.Light_close_hour_time=6;
				Flash_Data.Light_close_min_time=0;	

				Flash_Data.Light_Bright_Enable=0;
				Flag.Brightness_time_flag=0;
				Flash_Data.Light_Bright_Value=20;
        
        Flash_Data.Beep_open = 1;
        Flash_Data.Control_open =0;

    }	
    
    /* 读取块1的Flash数据 — 从后往前搜索最新有效数据 */
    for(i = FDL_BLOCK1_ONCE_MAX_WRITE_NUM; i > 0; i--)
    {
        Read_FLASH_MUIData(FDL_Block1 + (i - 1) * sizeof(Ell_Data), (uint8_t*)&Ell_Data, sizeof(Ell_Data));

        /* 校验位置数据是否在有效物理范围内（1000~4000） */
        if (Ell_Data.Motor_Current_Position >= (UP_POSITION_VALUE-100) &&
            Ell_Data.Motor_Current_Position <= (DOWN_POSITION_VALUE + 100) &&
            Ell_Data.B_Motor_Current_Position >= (UP_POSITION_VALUE-100) &&
            Ell_Data.B_Motor_Current_Position <= (DOWN_POSITION_VALUE + 100))
        {
            /* 找到有效数据 */
            if(i == FDL_BLOCK1_ONCE_MAX_WRITE_NUM)  // 槽位写满，擦除后重新整理
            {
									__disable_irq();
                Flash_SectorErase(FDL_Block1);
                FDL_Block1_Save_Times = 0;
                Flash_WriteStruct(FDL_Block1, (uint8_t*)&Ell_Data, sizeof(Ell_Data));
								__enable_irq();
            }
            else
            {
                FDL_Block1_Save_Times = i - 1;
            }
            break;
        }
        /* 数据不在有效范围内 → continue，继续搜索下一个槽位 */
    }

    /* 所有槽位均无有效数据，初始化默认值 */
    if(i == 0)
    {
				__disable_irq();
        Flash_SectorErase(FDL_Block1);
			__enable_irq();
        Ell_Data.Motor_Run_Circle = 0;
        Ell_Data.B_Motor_Run_Circle = 0;
        Ell_Data.Motor_Current_Position = DOWN_POSITION_VALUE + 100;
        Ell_Data.B_Motor_Current_Position = DOWN_POSITION_VALUE + 100;
        FDL_Block1_Save_Times = 0;
    }
    
    /* 设置电子限位值 */
    set_down_limit_value = Flash_Data.Electronic_Down_Limit * ONE_PERCENT_PULSE + UP_POSITION_VALUE;//电子下限位对应霍尔脉冲数
    B_set_down_limit_value = Flash_Data.B_electronic_down_limit * ONE_PERCENT_PULSE + UP_POSITION_VALUE;//电子下限位对应霍尔脉冲数
    
    if(Flash_Data.Electronic_Up_Limit == 0)
    {
        set_up_limit_value = Flash_Data.Electronic_Up_Limit * ONE_PERCENT_PULSE;//电子上限位对应霍尔脉冲数
    }
    else
    {
        set_up_limit_value = Flash_Data.Electronic_Up_Limit * ONE_PERCENT_PULSE + UP_POSITION_VALUE;
    }
    
    if(Flash_Data.B_electronic_up_limit == 0)
    {
        B_set_up_limit_value = Flash_Data.B_electronic_up_limit * ONE_PERCENT_PULSE;//电子上限位对应霍尔脉冲数
    }
    else
    {
        B_set_up_limit_value = Flash_Data.B_electronic_up_limit * ONE_PERCENT_PULSE + UP_POSITION_VALUE;
    }	
    
    /* 校验和修正超重校正值 */
    if((Flash_Data.Overweight_Correct == 0xff) || (Flash_Data.Overweight_Correct == 0))
    {
        Flash_Data.Overweight_Correct = 125;	
    }
    if((Flash_Data.B_Overweight_Correct == 0xff) || (Flash_Data.B_Overweight_Correct == 0))
    {
        Flash_Data.B_Overweight_Correct = 125;	
    }	
    
    /* 校验和修正灯光相关参数 */
    if(Flash_Data.Max_Light_Value > 100)
    {
        Flash_Data.Max_Light_Value = 100;	
    }
    if(Flash_Data.Light_Bright_Value > 100)
    {
        Flash_Data.Light_Bright_Value = 100;	
    }
    if(Flash_Data.Light_CT_value > 5700)
    {
        Flash_Data.Light_CT_value = 5700;	
    }	
    
    /* 设置灯光值 */
    set_light_value = (Flash_Data.Max_Light_Value * 100) * LIGHT_VALUE;//最大亮度对应脉宽数值

    /* 更新设备状态数据 */
    Device_State_Data.Beep_Enable = Flash_Data.Beep_open;
    Device_State_Data.electronic_down_limit = Flash_Data.Electronic_Down_Limit;
    Device_State_Data.B_electronic_down_limit = Flash_Data.B_electronic_down_limit;
    
    Device_State_Data.electronic_up_limit = Flash_Data.Electronic_Up_Limit;
    Device_State_Data.B_electronic_up_limit = Flash_Data.B_electronic_up_limit;
    

    Device_State_Data.Light_Bright = Flash_Data.Max_Light_Value;
    
    Device_State_Data.Light_colour_temperature = Flash_Data.Light_CT_value;
    colour_temperature_temp = (Device_State_Data.Light_colour_temperature - 2700) / 5;//步长5  0~LIGHT_MAX_STEP
    
    Device_State_Data.nightlight_switch = Flash_Data.Light_Bright_Enable;
    Device_State_Data.brightness = Flash_Data.Light_Bright_Value;
    
		Device_State_Data.start_time_hour=Flash_Data.Light_open_hour_time;
		Device_State_Data.start_time_min=Flash_Data.Light_open_min_time;
		Device_State_Data.end_time_hour=Flash_Data.Light_close_hour_time;
		Device_State_Data.end_time_min=Flash_Data.Light_close_min_time;
		

}

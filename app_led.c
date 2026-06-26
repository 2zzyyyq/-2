#include "ALL_Var.h"
const uint16_t  LIGHE_COLOR[3]={5700,4300,2700};
__IO uint8_t light_key_time;//照明键连续按下次数
uint16_t  Light_colour_temperature_gear,colour_temperature_temp;
uint16_t  brightness_target,TDR07_target_value,TDR05_target_value,brightness_temp;
uint8_t  step;	
uint8_t  step_value;

//照明按键响应
void Cmd_Light(void)
{
	if(Device_State_Data.Light_State_Byte==LIGHT_OFF)//当前关
	{
		Device_State_Data.Light_State_Byte=LIGHT_ON;
		SetupBzhx(LIGHT_ON_BEEP,Flash_Data.Beep_open);
		if(Time.Light_key_space_time<Light_KEY_SPACE_TIME)
		{
	 			if(Device_State_Data.Light_colour_temperature==0)
			{
				light_key_time=0;	
			} else if(Device_State_Data.Light_colour_temperature>2700&&Device_State_Data.Light_colour_temperature<4300)
			{
				light_key_time=2;	
			}
       else if(Device_State_Data.Light_colour_temperature>4300&&Device_State_Data.Light_colour_temperature<5700)
			{
				light_key_time=1;	
			}
			Device_State_Data.Light_colour_temperature=LIGHE_COLOR[light_key_time];
	  	Flash_Data.Light_CT_value=Device_State_Data.Light_colour_temperature;
		 	light_key_time++;
			Device_State_Data.lightmode='4';
			if(light_key_time>=3)
			{
				light_key_time=0;
			}
		}
		colour_temperature_temp=(Flash_Data.Light_CT_value-2700)/5;//开灯时色温不渐变，直接是最终状态
		if(Flag.Brightness_time_flag)
		{
			Device_State_Data.Light_Bright=Flash_Data.Light_Bright_Value;
		}
		else
		{
			Device_State_Data.Light_Bright=Flash_Data.Max_Light_Value;
		}
		Led.Light_Led_state=0;//有操作照明键，退出led指示状态
	}
	else if(Device_State_Data.Light_State_Byte==LIGHT_ON)//当前开
	{
		Device_State_Data.Light_State_Byte=LIGHT_OFF;
		Flag.Brightness_change=0;
		
		SetupBzhx(LIGHT_OFF_BEEP,Flash_Data.Beep_open);
		Time.Light_key_space_time=0;
		
		if(Led.Light_Led_state==3)
		{
			LED_LIGHT_OFF;
		}
		Led.Light_Led_state=0;//有操作照明键，退出led指示状态
	}	
}

void light_led_work(void)//照明灯做指示灯动效
{
	M0P_ADTIM_TypeDef *ADTx= M0P_ADTIM6;
	switch(Led.Light_Led_state)
	{
		case 0://无效果
		{
			
		}break;
		case 1://做呼吸灯，记忆色温,渐亮
		{
			LED_LIGHT_ON;
			if(brightness_temp<100)
			{
				brightness_temp++;	
				LIGHT_WHITE_VALUE=(float)((brightness_temp))*Light_colour_temperature_gear*LIGHT_TEM_RATIO;//
				LIGHT_YELLOW_VALUE=(float)(brightness_temp)*(LIGHT_MAX_STEP-Light_colour_temperature_gear)*LIGHT_TEM_RATIO;//
	
			}
			else
			{
				Led.Light_Led_state=2;
				Led.Light_Breathing_time++;
				if(Led.Light_Breathing_time>=6)
				{
					Led.Light_Breathing_time=0;
					Led.Light_Led_state=0;
				}
			}			
		}break;
		case 2://做呼吸灯，记忆色温,渐灭
		{
      LED_LIGHT_ON;
			if(brightness_temp>10)
			{
				brightness_temp--;	
			  LIGHT_WHITE_VALUE=(float)((brightness_temp))*Light_colour_temperature_gear*LIGHT_TEM_RATIO;//
				LIGHT_YELLOW_VALUE=(float)(brightness_temp)*(LIGHT_MAX_STEP-Light_colour_temperature_gear)*LIGHT_TEM_RATIO;//
	     
			}
			else
			{
				Led.Light_Breathing_time++;
				Led.Light_Led_state=1;
				if(Led.Light_Breathing_time>=6)
				{
					Led.Light_Breathing_time=0;
					Led.Light_Led_state=0;
				}
			}
		}break;
	
		case 3://固定色温、亮度
		{
			if(Time.WIFI_enable_config_time<WAIT_CONFIG_NETTIME)//等待配网30min钟到
			{
				 
			}
			else
			{
				Led.Light_Led_state=0;	
				if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
				{
					LED_LIGHT_OFF;
				}
				Time.WIFI_enable_config_time=0;
				Flag.Reset_model=1;
			}
		}break;
		default:break;
	}
}

void light_work(uint8_t light_state)
{
	M0P_ADTIM_TypeDef *ADTx= M0P_ADTIM6;
	brightness_target=Device_State_Data.Light_Bright;//0~100	
	Light_colour_temperature_gear=(Device_State_Data.Light_colour_temperature-2700)/5;//步长5  0~600  
	switch(light_state)
	{
		case 0://灯关闭,渐暗
		{
				if(brightness_temp>0)
				{
				  Flag.Lighting_setup=1;
					brightness_temp--;
					LIGHT_WHITE_VALUE=(uint16_t)((brightness_temp))*Light_colour_temperature_gear*LIGHT_TEM_RATIO;//
					LIGHT_YELLOW_VALUE=(uint16_t)(brightness_temp)*(LIGHT_MAX_STEP-Light_colour_temperature_gear)*LIGHT_TEM_RATIO;//
				}
				else
				{
					Flag.Lighting_setup=0;
					LED_LIGHT_OFF;
				}			
		}break;
		case 1://灯渐亮
		{	
       LED_LIGHT_ON;
			if(brightness_temp!=brightness_target)//Device_State_Data.Light_Bright
			{
				Flag.Lighting_setup=1;
			if(brightness_temp<brightness_target)//Device_State_Data.Light_Bright
				{
					brightness_temp++;		
				}
				else
				{
					brightness_temp--;
				}

				 LIGHT_WHITE_VALUE=(uint16_t)(brightness_temp*0.95+5)*Light_colour_temperature_gear*LIGHT_TEM_RATIO;//
		  	 LIGHT_YELLOW_VALUE=(uint16_t)(brightness_temp*0.95+5)*(LIGHT_MAX_STEP-Light_colour_temperature_gear)*LIGHT_TEM_RATIO;//	
			}
			else if(colour_temperature_temp!=Light_colour_temperature_gear)//
			{	
				 Flag.Lighting_setup=1;
				if(colour_temperature_temp<Light_colour_temperature_gear)
				{
					if(Light_colour_temperature_gear<25)
					{
						step_value=25;
					}
					else if(Light_colour_temperature_gear<100)
					{
						step_value=20;
					}
					else if(Light_colour_temperature_gear<200)
					{
						step_value=15;
					}
					else if(Light_colour_temperature_gear<400)
					{
						step_value=10;
					}
					else if(Light_colour_temperature_gear<=LIGHT_MAX_STEP)
					{
						step_value=5;
					}
					if(colour_temperature_temp>LIGHT_MAX_STEP)
					{
						TDR05_target_value=(uint16_t)(Device_State_Data.Light_Bright*0.95+5)*Light_colour_temperature_gear*LIGHT_TEM_RATIO;
						TDR07_target_value=(uint16_t)(Device_State_Data.Light_Bright*0.95+5)*(LIGHT_MAX_STEP-Light_colour_temperature_gear)*LIGHT_TEM_RATIO;//	
						if(LIGHT_WHITE_VALUE<TDR05_target_value)	
						{
							LIGHT_WHITE_VALUE++;
						}
						else if(LIGHT_WHITE_VALUE>TDR05_target_value)	
						{
							LIGHT_WHITE_VALUE--;
						}
						if(LIGHT_YELLOW_VALUE<TDR07_target_value)	
						{
							LIGHT_YELLOW_VALUE++;
						}
						else if(LIGHT_YELLOW_VALUE>TDR07_target_value)	
						{
							LIGHT_YELLOW_VALUE--;
						}	
						if((LIGHT_WHITE_VALUE==TDR05_target_value)||(LIGHT_YELLOW_VALUE==TDR07_target_value))
						{
							colour_temperature_temp=Light_colour_temperature_gear;					
						}				
					}
					else
					{
						colour_temperature_temp+=step_value;
						if(colour_temperature_temp>Light_colour_temperature_gear)
						{
							colour_temperature_temp=Light_colour_temperature_gear;
						}
						LIGHT_WHITE_VALUE=(uint16_t)(Device_State_Data.Light_Bright*0.95+5)*colour_temperature_temp*LIGHT_TEM_RATIO;//
			     	LIGHT_YELLOW_VALUE=(uint16_t)(Device_State_Data.Light_Bright*0.95+5)*(LIGHT_MAX_STEP-colour_temperature_temp)*LIGHT_TEM_RATIO;//
					}
				}
				else
				{
					if(Light_colour_temperature_gear<25)
					{
						step_value=5;
					}
					else if(Light_colour_temperature_gear<100)
					{
						step_value=10;
					}
					else if(Light_colour_temperature_gear<200)
					{
						step_value=15;
					}
					else if(Light_colour_temperature_gear<400)
					{
						step_value=20;
					}
					else if(Light_colour_temperature_gear<=LIGHT_MAX_STEP)
					{
						step_value=25;
					}
					if(colour_temperature_temp<25)
					{
						TDR05_target_value=(uint16_t)(Device_State_Data.Light_Bright*0.95+5)*Light_colour_temperature_gear*LIGHT_TEM_RATIO;
						TDR07_target_value=(uint16_t)(Device_State_Data.Light_Bright*0.95+5)*(LIGHT_MAX_STEP-Light_colour_temperature_gear)*LIGHT_TEM_RATIO;
						if(LIGHT_WHITE_VALUE<TDR05_target_value)	
						{
							LIGHT_WHITE_VALUE++;
						}
						else if(LIGHT_WHITE_VALUE>TDR05_target_value)	
						{
							LIGHT_WHITE_VALUE--;
						}
						if(LIGHT_YELLOW_VALUE<TDR07_target_value)	
						{
							LIGHT_YELLOW_VALUE++;
						}
						else if(LIGHT_YELLOW_VALUE>TDR07_target_value)	
						{
							LIGHT_YELLOW_VALUE--;
						}	
						if((LIGHT_WHITE_VALUE==TDR05_target_value)||(LIGHT_YELLOW_VALUE==TDR07_target_value))
						{
							colour_temperature_temp=Light_colour_temperature_gear;					
						}	
					}
					else
					{
						if(colour_temperature_temp>(Light_colour_temperature_gear+step_value))
						{
							colour_temperature_temp-=step_value;
						}
						else
						{
							colour_temperature_temp=Light_colour_temperature_gear;
						}
					  	LIGHT_WHITE_VALUE=(uint16_t)(Device_State_Data.Light_Bright*0.95+5)*colour_temperature_temp*LIGHT_TEM_RATIO;//
			 
							LIGHT_YELLOW_VALUE=(uint16_t)(Device_State_Data.Light_Bright*0.95+5)*(LIGHT_MAX_STEP-colour_temperature_temp)*LIGHT_TEM_RATIO;
					}
				}
			}else   //色温和亮度执行完毕
			{
 
 			    Flag.Lighting_setup=0;
 
			}
		}break;	
		default:break;		
	}
}





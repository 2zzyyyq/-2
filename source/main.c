#include "gpio.h"
#include "ddl.h"
#include "sysctrl.h"
#include "lvd.h"
#include "wdt.h"
#include "timer3.h"
#include "trim.h"
#include "ddl.h"
#include "flash.h"
#include "adc.h"
#include "uart.h"
#include "bgr.h"
#include "adt.h"
#include "bt.h"
#include "lpuart.h"
#include "lpm.h"
#include "ALL_Var.h"
__IO const uint8_t FLASH_DATE[2] __attribute__((section(".ARM.__at_0x3100")))=
{
		0xA5, 0x5A
};

int32_t main(void)
{
    __enable_irq();
	
    App_ClockCfg();  
	
    Flash_Init(12, TRUE);  
	
    System_init(); 
	
    App_WdtInit(); 

    while (1)
    {
        r_uart0_service();     // wifi处理
      
         //Ovreweight_current_correct();
			
        _220AC_Power_down_save();
			
        limit_protect();       // 限位检测判断
			
        limit_protect_B();
			
        if (Flag.time5ms_flag)
        {
            Flag.time5ms_flag = 0;
            
            Motor_speed();
					
            Motor_speed_B();
            
            capture_433key_judge_answer((button*)&capture_433key, Flag.power_on_10stime_flag); // 检测及响应遥控器

          if (!Flag.Enter_Pcba_test_flag)
            {
                if (!PCBA_SELFTEST)
                {
                    if (Time.Enter_Pcba_test_time++ > 200) // 有自检测试信号
                    {
                        Flag.Enter_Pcba_test_flag = 1;
											
                        Self_Inspection_step = 0;
											
                        SetupBzhx(LONG_BEEP, 1);
											
                        PCBA_Self_Inspection();
                    }
                }
                else
                {
                    Time.Enter_Pcba_test_time = 0;
                }
            }

        }
        
        if (Flag.time10ms_flag)
        {

            Flag.time10ms_flag = 0;
            alarm_signal_test_and_beep();    // 报警信号检测
            alarm_signal_test_and_beep_B();  // 报警信号检测
            CmdBzhx();                       // 蜂鸣器动作
        }
        if (Flag.time100ms_flag)
        {
			    // led_work();                   // wifi状态指示灯
            Flag.time100ms_flag = 0;
          
            PowerOn_ModelSet();
					
            Wdt_Feed();

						#ifdef  TESTADC			
						if(Flag.mcu_wifi_mode)
						{
							volatile uint16_t u32AdcRestult2;
							volatile uint16_t u32AdcRestult3;
							int16_t u32AdcRestult4;
							unsigned char debug_buf[80];
							u32AdcRestult3=Motor_Current_B.ADvalue*1000/4095*1.5/0.1;	
							u32AdcRestult2=Motor_Current.ADvalue*1000/4095*1.5/0.1;	
							snprintf((char*)debug_buf, sizeof(debug_buf), "<any>:%d,%d,%d,%d,%d,%d\r\n",Motor_Current.ADvalue,Ell_Data.Motor_Current_Position,u32AdcRestult2,
                                                              Motor_Current_B.ADvalue,Ell_Data.B_Motor_Current_Position,u32AdcRestult3);
						  r_uart0_send_bytes(debug_buf, strlen( (const char*)debug_buf ));

						}
						#else  

						#endif
        }
        
        /* 每个150ms循环执行 */
        if (Flag.time150ms_flag)
        {
 
            Flag.time150ms_flag = 0;
					
            _150ms_work();                   // wifi状态回馈
        }
    }
}
	void System_init(void)
{
     
			App_LvdInit();                          // 掉电初始化
			App_Timer1PortCfg();
			App_Timer0Cfg(32000);                  // Timer0配置初始化(周期 = 32000(1/32M) = 1ms)
			App_LedInit();                         // 推挽初始化  
			App_UserKeyInit();
			// App_LpUartCfg();
			App_UartCfg();                        
			App_AdvTimerInit(0, 0);                // 蜂鸣器 PWM控制输出50% 频率控制 10kz 
			App_AdcPortInit();                     // ADC 采样端口初始化 AIN2 AIN3 
			App_TimermotorPortCfg();             
			App_TimerLEDPortCfg();
			read_data();                           // 数据初始化
			variable_init();
	    /*关闭所有继电器*/
			MOTOR_DN_IO(0); 
			MOTOR_UP_IO(0);
			B_MOTOR_DN_IO(0);
			B_MOTOR_UP_IO(0);
    // 电机A状态初始化
    if (YZ)
    {
        Flag.Encounter_Obstacle_Motor_stop = 1; // 遇阻
    }
    
    if ((Ell_Data.Motor_Current_Position >= set_down_limit_value)) // ||(DOWN_LIMIT))
    {
        Device_State_Data.Motor_State_Byte = MOTOR_DOWN_STOP_STATE;
    }
    else if ((Ell_Data.Motor_Current_Position <= set_up_limit_value)) // ||(UP_LIMIT))
    {
        Device_State_Data.Motor_State_Byte = MOTOR_UP_STOP_STATE;
    }
    else
    {
        Device_State_Data.Motor_State_Byte = MOTOR_SUSPEND_STATE;
    }
    // 电机B状态初始化
    if (YZ_B)
    {
        Flag.B_Encounter_Obstacle_Motor_stop = 1; // 遇阻
    }
    if ((Ell_Data.B_Motor_Current_Position >= B_set_down_limit_value)) // ||(B_DOWN_LIMIT))
    {
        Device_State_Data.B_Motor_State_Byte = MOTOR_DOWN_STOP_STATE;
    }
    else if ((Ell_Data.B_Motor_Current_Position <= B_set_up_limit_value)) // ||(B_UP_LIMIT))
    {
        Device_State_Data.B_Motor_State_Byte = MOTOR_UP_STOP_STATE;
    }
    else
    {
        Device_State_Data.B_Motor_State_Byte = MOTOR_SUSPEND_STATE;
    }
    // 升级相关处理,升级后复位不响
    if (Flash_Data.Quite_updata)
    {
     SetupBzhx(POWER_ON_BEEP, 1);
    }
    else // 静默升级后复位
    {
        Net_state = NET_CLOUD;
    }
    Flash_Data.Quite_updata = 1;
    // 设备升级后状态恢复
    if (Flash_Data.Device_updata == 2) // 升级后复位,恢复升级前状态，不上报数据
    {
        // 恢复照明状态，并且不上报  
         Device_State_Data.Light_State_Byte = Flash_Data.Device_light_state;
         Up_State_Data.Light_State_Byte = Device_State_Data.Light_State_Byte;
        
         Device_State_Data.Light_Bright = Flash_Data.Light_pwm_value;
         Up_State_Data.Light_Bright = Device_State_Data.Light_Bright;
        
         // 恢复电机状态，不上报
         // Device_State_Data.Motor_State_Byte = Flash_Data.Device_motor_state1 * 3 + Flash_Data.Device_motor_state;
			   //  Device_State_Data.Motor_State_Byte=MOTOR_SUSPEND_STATE;
         Up_State_Data.Motor_State_Byte = Device_State_Data.Motor_State_Byte; // 不上报
 
			//  Device_State_Data.B_Motor_State_Byte=MOTOR_SUSPEND_STATE;
					Up_State_Data.B_Motor_State_Byte=Device_State_Data.B_Motor_State_Byte;
					Flag.Motor_run_now = 0;
					Flag.B_Motor_run_now = 0;
//        if (Device_State_Data.Motor_State_Byte == MOTOR_UP_STATE)
//        {
//            Flag.Motor_run_now = 0;
//        }
//        else if (Device_State_Data.Motor_State_Byte == MOTOR_DOWN_STATE)
//        {
//            Flag.Motor_run_now = 0;
//        }
    }
    
    Flash_Data.Device_updata = 1;
 
}
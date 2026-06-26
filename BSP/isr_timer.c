#include "ALL_Var.h"
uint32_t  Bt_CapTemp_Value1,Bt_CapTemp_Value2;
volatile int32_t   Bt_Capture_Value;
volatile int32_t   Bt_Capture_ValueB;
volatile uint16_t  Bt_Uev_Cnt;
void Tim1_IRQHandler(void)
{
    static uint8_t i;
    //Timer0 模式23 捕获中断B
    if(TRUE == Bt_GetIntFlag(TIM1, BtCA0Irq))
    {
        if(0 == i)
        {
						Bt_CapTemp_Value1 = M0P_TIM1_MODE23->CCR0A_f.CCR0A;  //第一次捕获值
						Bt_Uev_Cnt=0;
						Bt_Capture_ValueB=Bt_CapTemp_Value1-Bt_CapTemp_Value2;
					
						Record_PulseWidth(Bt_Capture_ValueB,&Capture_Command);
            i = 1;
        }
        else if(1 == i)
        {
						Bt_CapTemp_Value2 = M0P_TIM1_MODE23->CCR0A_f.CCR0A;  //第二次捕获值
						Bt_Capture_Value = Bt_Uev_Cnt * 0xFFFF + Bt_CapTemp_Value2 - Bt_CapTemp_Value1;  //两次捕获之间的差值  高电平时间
						i = 0;
				
						Record_PulseWidth(Bt_Capture_Value,&Capture_Command);
        }
        
        Bt_ClearIntFlag(TIM1,BtCA0Irq); //清除中断标志
    }
    if(TRUE == Bt_GetIntFlag(TIM1, BtUevIrq))
    {  
        Bt_Uev_Cnt++;
        Bt_ClearIntFlag(TIM1,BtUevIrq); //清除中断标志
    }
}
 static uint16_t DelayTimer=0;
void Tim2_IRQHandler(void)
{
    if (TRUE == Bt_GetIntFlag(TIM2, BtUevIrq)) // Timer0 模式0 溢出中断
    {
        Adc_JQR_Start();
      
        Bt_ClearIntFlag(TIM2, BtUevIrq); // 中断标志清零
        
        // 时间计数器递增
        Time.time5ms++;
        Time.time10ms++;
        Time.time100ms++;
        Time.time150ms++;
        Time.Test_speed_time++;
        Time.B_Test_speed_time++;
        Time.return_to_up_time++;
        Time.B_return_to_up_time++;
        Time.return_to_down_time++;
        Time.B_return_to_down_time++;
        Time.Motor_No_Encounter_Obstacle_time++;
        Time.B_Motor_No_Encounter_Obstacle_time++;
        Time.Enter_Pcba_test_time++;
        Time.motor_debug_time++;
        Time.key_convergent++;
			  

				Time.WIFI_enable_config_time++;
				Time.Up_Limit_Set_time++;
				Time.Up_B_Limit_Set_time++;
 
			 if(STOP_CMD_Repeat!=0)
				{
					Time.Del_Match_Code_time++;
					if(Time.Del_Match_Code_time>5000)
					{
						Time.Del_Match_Code_time=0;
						STOP_CMD_Repeat=0;
					}
				} 
				else
				{
					Time.Del_Match_Code_time=0;
				}		
				
				
 
				if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
				{
				 if(Time.Light_key_space_time<Light_KEY_SPACE_TIME)
				 {
						 Time.Light_key_space_time++;
				 }
				}
				else
				{
					Time.Light_key_space_time=0;
				}
        
        // 霍尔脉冲滤波时间
        if (Time.Holl_Pluse_Filter_time < 0xff)
        {
            Time.Holl_Pluse_Filter_time++;
        }
        if (Time.B_Holl_Pluse_Filter_time < 0xff)
        {
            Time.B_Holl_Pluse_Filter_time++;
        }
        
        // 串口接收间隔时间
        if (Time.Uart_Receive_Interval_time < RECEIVE_DATA_PACKET_INTERVAL_TIME)
        {
            Time.Uart_Receive_Interval_time++;
        }
        
        // 5ms 定时标志
        if (Time.time5ms >= 5)
        {
            Time.time5ms = 0;
            Flag.time5ms_flag = 1;
        }
        
        // 10ms 定时标志
        if (Time.time10ms >= 10)
        {
            Time.time10ms = 0;
            Flag.time10ms_flag = 1;
        }
        
        // 100ms 定时标志
        if (Time.time100ms >= 100)
        {
            Time.time100ms = 0;
            Flag.time100ms_flag = 1;
            
            // 超重电流校正时间处理
            if (Flag.Overweight_current_correct_start)
            {
                Time.Overweight_Current_time++;
            }
            else
            {
                Time.Overweight_Current_time = 0;
            }
            
            if (Flag.B_Overweight_current_correct_start)
            {
                Time.B_Overweight_Current_time++;
            }
            else
            {
                Time.B_Overweight_Current_time = 0;
            }
        }
        
        // 150ms 定时标志
        if (Time.time150ms >= 150)
        {
            Time.time150ms = 0;
            Flag.time150ms_flag = 1;
        }
        
        // 数据上报延时处理
        if (Time.Up_Data_Delay_time > 0)
        {
            Time.Up_Data_Delay_time--;
            if (Time.Up_Data_Delay_time == 0)
            {
                Up_State_Data.Err_Byte &= 0xfb; // 重新上报电机故障
            }
        }
        
        if (Time.B_Up_Data_Delay_time > 0)
        {
            Time.B_Up_Data_Delay_time--;
            if (Time.B_Up_Data_Delay_time == 0)
            {
                Up_State_Data.B_Err_Byte &= 0xfb; // 重新上报电机故障
            }
        }
        
        // 220V AC 掉电检测
        if (!Flag._220AC_Power_Down_flag)
        {
            Time.Zero_crossing_no_single_time++;
            if (Time.Zero_crossing_no_single_time > ZERO_CROSSING_NO_SINGL_TIME)
            {
                Flag._220AC_Power_Down_flag = 1; // 掉电 写入flash
            }
        }
        
        // 电机A启动上升处理
        if (Flag.A_Motor_Start_up)
        {
            Time.A_Motor_Start_Up_time++;
            
            // 启动后延时一段时间开始检测上升遇到障碍物
            if (Time.A_Motor_Start_Up_time >= DIS_JUDGE_MOTOR_UP_OBS_TIME)
            {
                if (Flag.Enable_Test_Motor_Obs_flag == 0)
                {
                    ADC_sample_num = 0;
                    ADC_sample_max = 0;
                    conform_motor_obs_num = 0;
                    conform_motor_obs_num2 = 0;
                    ADC_sample_min = 65535;
                }
                Flag.Enable_Test_Motor_Obs_flag = 1;
            }
            
            // 启动后延时一段时间开始检测超重
            if (Time.A_Motor_Start_Up_time >= DIS_JUDGE_MOTOR_OVER_WEIGHT_TIME)
            {
                Flag.A_Motor_Start_up = 0;
                Time.A_Motor_Start_Up_time = 0;
                Time.A_Judge_Over_Wight_time = 0;
//              if (Flag.Motor_all_up)
//               {
//                  Flag.Enable_Test_Over_Wight_flag = 1;
//								  Flag.A_Enable_Test_Over_Wight_flag = 1;
//               }
//               else
                {
                    Flag.A_Enable_Test_Over_Wight_flag = 1;
                }
            }
        }
        
        // 电机B启动上升处理
        if (Flag.B_Motor_Start_up)
        {
            Time.B_Motor_Start_Up_time++;
            
            // 启动后延时一段时间开始检测上升遇到障碍物
            if (Time.B_Motor_Start_Up_time >= DIS_JUDGE_MOTOR_UP_OBS_TIME)
            {
                if (Flag.B_Enable_Test_Motor_Obs_flag == 0)
                {
                    ADC_sample_num_B = 0;
                    ADC_sample_max_B = 0;
                    conform_motor_obs_num_B = 0;
                    conform_motor_obs_num2_B = 0;
                    ADC_sample_min_B = 65535;
                }
                Flag.B_Enable_Test_Motor_Obs_flag = 1;
            }
            
            // 启动后延时一段时间开始检测超重
            if (Time.B_Motor_Start_Up_time >= DIS_JUDGE_MOTOR_OVER_WEIGHT_TIME)
            {
                Flag.B_Motor_Start_up = 0;
                Time.B_Motor_Start_Up_time = 0;
                Time.B_Judge_Over_Wight_time = 0;
               // if (!Flag.Motor_all_up)
                {
                    Flag.B_Enable_Test_Over_Wight_flag = 1;
                }
            }
        }
        
        // 电机A换向处理
        if (Flag.Motor_down_to_up) // 电机换向中
        {
            Time.Motor_Dir_Change_Delaytime++;
            if (Time.Motor_Dir_Change_Delaytime >= MOTOR_DIR_CHANGE_DELAY)
            {
                Time.Motor_Dir_Change_Delaytime = 0;
                Flag.Motor_down_to_up = 0;
                SetupBzhx(MOTOR_UP_BEEP, Flash_Data.Beep_open); // 延时完响应蜂鸣器
            }
        }
        else if (Flag.Motor_up_to_down) // 电机换向中
        {
            Time.Motor_Dir_Change_Delaytime++;
            if (Time.Motor_Dir_Change_Delaytime >= MOTOR_DIR_CHANGE_DELAY)
            {
                Time.Motor_Dir_Change_Delaytime = 0;
                Flag.Motor_up_to_down = 0;
                if (current_state == 3)
                {
                    // Up_State_Data.Err_Byte &= 0xfd; // 重新上报超重
                }
                else if (current_state == 4)
                {
                    // Up_State_Data.Err_Byte &= 0xf7; // 重新上报上升遇阻
                }
                else
                {
                    SetupBzhx(MOTOR_DOWN_BEEP, Flash_Data.Beep_open); // 切换完再响蜂鸣器
                }
            }
        }
        else if (Flag.Motor_raise_up) // 轻抬上升
        {
            Time.Motor_Dir_Change_Delaytime++;
            if (Time.Motor_Dir_Change_Delaytime >= MOTOR_DIR_CHANGE_DELAY1)
            {
                Time.Motor_Dir_Change_Delaytime = 0;
                Flag.Motor_raise_up = 0;
                SetupBzhx(GENTLY_LIFT_MOTOR_UP_BEEP, Flash_Data.Beep_open); // 切换完再响蜂鸣器
            }
        }
        
        // 电机B换向处理
        if (Flag.B_Motor_down_to_up) // 电机换向中
        {
            Time.B_Motor_Dir_Change_Delaytime++;
            if (Time.B_Motor_Dir_Change_Delaytime >= MOTOR_DIR_CHANGE_DELAY)
            {
                Time.B_Motor_Dir_Change_Delaytime = 0;
                Flag.B_Motor_down_to_up = 0;
                SetupBzhx(MOTOR_UP_BEEP, Flash_Data.Beep_open); // 延时完响应蜂鸣器
            }
        }
        else if (Flag.B_Motor_up_to_down) // 电机换向中
        {
            Time.B_Motor_Dir_Change_Delaytime++;
            if (Time.B_Motor_Dir_Change_Delaytime >= MOTOR_DIR_CHANGE_DELAY)
            {
                Time.B_Motor_Dir_Change_Delaytime = 0;
                Flag.B_Motor_up_to_down = 0;
                if (current_state_B == 3)
                {
                    // Up_State_Data.B_Err_Byte &= 0xfd; // 重新上报超重
                }
                else if (current_state_B == 4)
                {
                    // Up_State_Data.B_Err_Byte &= 0xf7; // 重新上报上升遇阻
                }
                else
                {
                    SetupBzhx(MOTOR_DOWN_BEEP, Flash_Data.Beep_open); // 切换完再响蜂鸣器
                }
            }
        }
        else if (Flag.B_Motor_raise_up) // 轻抬上升
        {
            Time.B_Motor_Dir_Change_Delaytime++;
            if (Time.B_Motor_Dir_Change_Delaytime >= MOTOR_DIR_CHANGE_DELAY1)
            {
                Time.B_Motor_Dir_Change_Delaytime = 0;
                Flag.B_Motor_raise_up = 0;
                SetupBzhx(GENTLY_LIFT_MOTOR_UP_BEEP, Flash_Data.Beep_open); // 切换完再响蜂鸣器
            }
        }
        
        // 允许添码时间处理
        if (Device_State_Data.Control_open == 1)
        {
            Time.Enable_Add_Match_Code_time++;
            if (Time.Enable_Add_Match_Code_time >= EN_ADD_CODE_TIME) // 添码10s有效
            {

              //  Flag.Add_Match_Code_flag = 0;
							  Device_State_Data.Control_open=0;
							  Flash_Data.Control_open=Device_State_Data.Control_open;
            }
        }
        else
        {
            Time.Enable_Add_Match_Code_time = 0;
        }
        
 
//        if (Err.data != 0) // 有报警信号
//        {
//            Time.beep_off_5stime++;
//            if (Time.beep_off_5stime >= ALARM_BEEP_CYCLE_TIME)
//            {
//                Time.beep_off_5stime = 0;
//                Flag.beep_off_5stime_flag = 1;
//            }
//        }
//        if (Err_B.data != 0) // 有B报警信号
//        {
//            Time.B_beep_off_5stime++;
//            if (Time.B_beep_off_5stime >= ALARM_BEEP_CYCLE_TIME)
//            {
//                Time.B_beep_off_5stime = 0;
//                Flag.B_beep_off_5stime_flag = 1;
//            }
//        }
    
        
        // 上电10秒标志
        if (Flag.power_on_10stime_flag == 0)
        {
            Time.power_on_10stime++;
            if (Time.power_on_10stime >= POWER_ON_ENABLE_STUDY_TIME)
            {
                Time.power_on_10stime = 0;
                Flag.power_on_10stime_flag = 1;
            }
        }
    }
}








































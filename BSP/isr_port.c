#include "ALL_Var.h"

void PortA_IRQHandler(void) //中断处理
{
    static uint8_t temp_level_B;
    static uint8_t down_pluse_flag_B = 0; // 处理逻辑
    if (TRUE == GetIrq_B_HOLL) //电机2霍尔信号
    {
        if ((Time.B_Holl_Pluse_Filter_time >= HOLL_PULSE_FILTER_TIEM))
        {
            if (B_HOLL_NUM) //上升沿
            {
                if ((B_HOLL_DIR != temp_level_B) || (down_pluse_flag_B == 0)) //如果上升沿与下降沿对应另一个霍尔电平相同，认为是电机抖动
                {
                    if (B_HOLL_DIR)
                    {
                        Ell_Data.B_Motor_Current_Position++;
                        B_Motor_Current_Position_pulse++;
                    }
                    else
                    {
                        if (Ell_Data.B_Motor_Current_Position > 0)
                        {
                            Ell_Data.B_Motor_Current_Position--;
                        }
                        B_Motor_Current_Position_pulse--;
                    }
                    Holl_Circle_num_B++;
                }
            }
            else //下降沿
            {
                down_pluse_flag_B = 1;
                temp_level_B = B_HOLL_DIR; //记录下降沿对应另一个霍尔电平
            }
        }
        Time.B_Holl_Pluse_Filter_time = 0;
        CLEAR_B_GetIrq;
    }
		  
}

void PortB_IRQHandler(void) //中断处理
{
    if (TRUE == Gpio_GetIrqStatus(GpioPortB, GpioPin12))
    {
        if (!ZERO_CROSS) //下降沿上电状态
        {
            Time.Zero_crossing_no_single_time = 0;
            Flag.Already_save_data_flag = 0;
            Flag._220AC_Power_Down_flag = 0;
        }
        
        if (!Flag.Enter_Pcba_test_flag) //非自检
        {
            if (Led.Light_Led_state == 0) //无指示灯效果
            {
                light_work(Device_State_Data.Light_State_Byte); //灯开关动作，渐亮，渐暗,过零动作
            }
            else
            {
                light_led_work();
            }
            
            motor_work(Device_State_Data.Motor_State_Byte); //电机运行动作，过零动作
            motor_work_B(Device_State_Data.B_Motor_State_Byte);
        }
        
        Gpio_ClearIrq(GpioPortB, GpioPin12);
    }
}

void PortC_IRQHandler(void) //中断处理
{
    static uint8_t temp_level;
    static uint8_t down_pluse_flag = 0;
    if (TRUE == GetIrq_HOLL) //电机1霍尔信号
    {
        if ((Time.Holl_Pluse_Filter_time >= HOLL_PULSE_FILTER_TIEM))
        {
            if (HOLL_NUM) //上升沿
            {
                if ((HOLL_DIR != temp_level) || (down_pluse_flag == 0)) //如果上升沿与下降沿对应另一个霍尔电平相同，认为是电机抖动
                {
                    if (HOLL_DIR)
                    {
                        Ell_Data.Motor_Current_Position++;
                        Motor_Current_Position_pulse++;
                    }
                    else
                    {
                        if (Ell_Data.Motor_Current_Position > 0)
                        {
                            Ell_Data.Motor_Current_Position--;
                        }
                        Motor_Current_Position_pulse--;
                    }
                    Holl_Circle_num++;
                }
            }
            else //下降沿
            {
                down_pluse_flag = 1;
                temp_level = HOLL_DIR; //记录下降沿对应另一个霍尔电平
            }
        }
        Time.Holl_Pluse_Filter_time = 0;
				CLEAR_HOLL_GetIrq;
    }
}






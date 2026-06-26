#include "ALL_Var.h"
#include "bt.h"
#include <math.h>  // 包含数学函数声明
volatile ELL_DATA  Ell_Data;
DEVICE_STATE_DATA   Device_State_Data;//设备上报状态
DEVICE_STATE_DATA   Up_State_Data;
volatile ERR Err,Err_B;
uint8_t   Soft_start_moveA,Soft_start_moveB;
uint8_t   current_state,current_state_B;//
uint16_t  set_up_limit_value,B_set_up_limit_value;//电子上限位霍尔数
uint16_t  set_down_limit_value,B_set_down_limit_value;//电子下限位霍尔数
void      Delay1ms(uint32_t nTime);
//下降建响应
void Cmd_Motor_Down(void)
{
	Flag.Run_to_set_position_flag=0;
	Err.err_bit.Motor_Obs=0;//碰撞遇阻过程被打断后,也需要清除故障
	if((Flag.Motor_down_to_up==0)&&(Flag.Motor_up_to_down==0))//电机换向延时时间，不响应电机操作
	{	
		if(Err.err_bit.Encounter_Obstacle==1)//遇阻状态重新报遇阻提示音
		{
			Err.err_bit.Encounter_Obstacle=0;
			Time.Encounter_Obstacle_beepnum=0;
			Up_State_Data.Err_Byte&=0xfe;//重新上报app遇阻
		}
		else
		{
		if((!Flag.A_Down_limit)&&((!(Ell_Data.Motor_Current_Position>=(set_down_limit_value-ONE_PERCENT_PULSE)))||Flag.No_judge_electronic_limit))//没有到达电子下限位和机械下限位
			{
				if((Device_State_Data.Motor_State_Byte==MOTOR_UP_STOP_STATE)
			      	||(Device_State_Data.Motor_State_Byte==MOTOR_SUSPEND_STATE)
				||(Device_State_Data.Motor_State_Byte==MOTOR_DOWN_STOP_STATE)||(Device_State_Data.Motor_State_Byte==MOTOR_UPLIM_STATE))//当前是暂停状态
				{
					if((Device_State_Data.Motor_Position>=Device_State_Data.electronic_down_limit)&&(!Flag.No_judge_electronic_limit))
				{
			
				}
			else
				{
					Time.Up_Limit_Set_time =0;//上限位触发按下降，下降时间定时
						Device_State_Data.Motor_State_Byte=MOTOR_DOWN_STATE;
			    	SetupBzhx(MOTOR_DOWN_BEEP,Flash_Data.Beep_open);
						Flag.Motor_run_now=0;
				}
				}
				else if(Device_State_Data.Motor_State_Byte==MOTOR_UP_STATE)//当前是上升状态，先停止
				{
					Device_State_Data.Motor_State_Byte=MOTOR_DOWN_STATE;
					//SetupBzhx(MOTOR_DOWN_BEEP);//换向完再响蜂鸣器	
					Flag.Motor_up_to_down=1;
					Flag.Motor_run_now=0;
				}
				else if(Device_State_Data.Motor_State_Byte==MOTOR_DOWN_STATE)//当前是下降状态
				{
					Device_State_Data.Motor_State_Byte=MOTOR_SUSPEND_STATE;
					SetupBzhx(MOTOR_STOP_BEEP,Flash_Data.Beep_open);
					Flag.Motor_run_now=0;
				}
			}
		}
	}
}



void Cmd_Motor_Down_B(void)
{
	Flag.B_Run_to_set_position_flag=0;
	Err_B.err_bit.Motor_Obs=0;//碰撞遇阻过程被打断后,也需要清除故障
	if((Flag.B_Motor_down_to_up==0)&&(Flag.B_Motor_up_to_down==0))//电机换向延时时间，不响应电机操作
	{	
		if(Err_B.err_bit.Encounter_Obstacle==1)//遇阻状态重新报遇阻提示音
		{
			Err_B.err_bit.Encounter_Obstacle=0;
			Time.B_Encounter_Obstacle_beepnum=0;
			Up_State_Data.B_Err_Byte&=0xfe;//重新上报app遇阻
		}
		else
		{
			if((!Flag.B_Down_limit)&&((!(Ell_Data.B_Motor_Current_Position>=(B_set_down_limit_value-ONE_PERCENT_PULSE)))||Flag.B_No_judge_electronic_limit))//没有到达电子下限位和机械下限位
			{
				if((Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STOP_STATE)
			      	||(Device_State_Data.B_Motor_State_Byte==MOTOR_SUSPEND_STATE)
				||(Device_State_Data.B_Motor_State_Byte==MOTOR_DOWN_STOP_STATE)||(Device_State_Data.B_Motor_State_Byte==MOTOR_UPLIM_STATE))//当前是暂停状态
				{
					if((Device_State_Data.B_Motor_Position>=Device_State_Data.B_electronic_down_limit)&&(!Flag.B_No_judge_electronic_limit))
					{
					
					}
					else
					{
						Time.Up_B_Limit_Set_time=0;
						Device_State_Data.B_Motor_State_Byte=MOTOR_DOWN_STATE;
		     		SetupBzhx(MOTOR_DOWN_BEEP,Flash_Data.Beep_open);
						Flag.B_Motor_run_now=0;
					}
				}
				else if(Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STATE)//当前是上升状态，先停止
				{
					Device_State_Data.B_Motor_State_Byte=MOTOR_DOWN_STATE;
					//SetupBzhx(MOTOR_DOWN_BEEP);//换向完再响蜂鸣器	
					Flag.B_Motor_up_to_down=1;
					Flag.B_Motor_run_now=0;
				}
				else if(Device_State_Data.B_Motor_State_Byte==MOTOR_DOWN_STATE)//当前是下降状态
				{
					Device_State_Data.B_Motor_State_Byte=MOTOR_SUSPEND_STATE;
				  SetupBzhx(MOTOR_STOP_BEEP,Flash_Data.Beep_open);
					Flag.B_Motor_run_now=0;
				}
			}
		}
	}
}

 
void Cmd_Motor_Stop(void)
{
	Flag.Run_to_set_position_flag=0;
	Err.err_bit.Motor_Obs=0;//碰撞遇阻过程被打断后,也需要清除故障
	Err_B.err_bit.Motor_Obs=0;
	if((Flag.Motor_down_to_up==0)&&(Flag.Motor_up_to_down==0))//电机换向延时时间，不响应电机操作
	{
		if((Device_State_Data.Motor_State_Byte==MOTOR_UP_STOP_STATE)
		||(Device_State_Data.Motor_State_Byte==MOTOR_DOWN_STOP_STATE))
		{
			
	//	SetupBzhx(MOTOR_DOWN_BEEP);
		}
		else if(Device_State_Data.Motor_State_Byte==MOTOR_UP_STATE)//当前是上升状态
		{
			if(Flag.Motor_down_to_up!=1)//不在切换到上升状态过程
			{
				Device_State_Data.Motor_State_Byte=MOTOR_SUSPEND_STATE;
				Flag.Motor_run_now=0;
				SetupBzhx(MOTOR_STOP_BEEP,Flash_Data.Beep_open);
				current_state=0;
			}
		}
		else if(Device_State_Data.Motor_State_Byte==MOTOR_DOWN_STATE)//当前是下降状态
		{
			if(Flag.Motor_up_to_down!=1)//不在切换到下降状态过程
			{
				Device_State_Data.Motor_State_Byte=MOTOR_SUSPEND_STATE;
				Flag.Motor_run_now=0;
				SetupBzhx(MOTOR_STOP_BEEP,Flash_Data.Beep_open);
				current_state=0;
			}
		}
	}
}

void Cmd_Motor_Stop_B(void)
{

	Flag.B_Run_to_set_position_flag=0;
	if((Flag.B_Motor_down_to_up==0)&&(Flag.B_Motor_up_to_down==0))//电机换向延时时间，不响应电机操作
	{
		if((Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STOP_STATE)
		||(Device_State_Data.B_Motor_State_Byte==MOTOR_DOWN_STOP_STATE))
		{
			
			//SetupBzhx(MOTOR_DOWN_BEEP);
		}
		else if(Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STATE)//当前是上升状态
		{
			if(Flag.B_Motor_down_to_up!=1)//不在切换到上升状态过程
			{
				Device_State_Data.B_Motor_State_Byte=MOTOR_SUSPEND_STATE;
				Flag.B_Motor_run_now=0;
	    	SetupBzhx(MOTOR_STOP_BEEP,Flash_Data.Beep_open);
				current_state_B=0;
			}
		}
		else if(Device_State_Data.B_Motor_State_Byte==MOTOR_DOWN_STATE)//当前是下降状态
		{
			if(Flag.B_Motor_up_to_down!=1)//不在切换到下降状态过程
			{
				Device_State_Data.B_Motor_State_Byte=MOTOR_SUSPEND_STATE;
				Flag.B_Motor_run_now=0;
	    	SetupBzhx(MOTOR_STOP_BEEP,Flash_Data.Beep_open);
				current_state_B=0;
			}
		}
	}
}


void motor_stop(void)
{		
   Soft_start_moveA=PWM_STOP;
}
void motor_stop_B(void)
{	 
	 Soft_start_moveB=PWM_STOP;
}
void motor_up(void)
{
			MOTOR_DN_IO(0);
			MOTOR_UP_IO(1);
	    Soft_start_moveA=PWM_UP;
}
void motor_up_B(void)
{
		B_MOTOR_UP_IO(1);
		B_MOTOR_DN_IO(0);
		Soft_start_moveB=PWM_UP;
}
void motor_down(void)
{	
		Soft_start_moveA=PWM_DOWN;
		MOTOR_UP_IO(0);	
		MOTOR_DN_IO(1);
	
}
void motor_down_B(void)
{	  
    Soft_start_moveB=PWM_DOWN;
	  B_MOTOR_UP_IO(0);	
	  B_MOTOR_DN_IO(1);
}
void Delay1ms(uint32_t nTime)
{
	uint32_t i , j;
	for (j = nTime; j > 0; j--)
	{
		for (i = 0; i < 4800; i++);
	}
}


void motor_work(uint8_t motor_state)
{
    switch(motor_state)
    {
        case MOTOR_UP_STOP_STATE://电机停止状态，执行电机停止
        case MOTOR_DOWN_STOP_STATE:
        case MOTOR_SUSPEND_STATE:
        {
            Flag.Motor_all_up = 0;
            Flag.Enable_Test_Over_Wight_flag = 0;//检测双杆超重操作
            Flag.A_Enable_Test_Over_Wight_flag = 0;
            Flag.Enable_Test_Motor_Obs_flag = 0;
            motor_stop();
            Flag.No_judge_electronic_limit = 0;	
            Flag.Motor_run_now = 0;
            Time.Motor_Run_time = 0;
            Flag.A_Motor_Start_up = 0;
            Time.A_Motor_Start_Up_time = 0;
            Time.Motor_Err_time = 0;
            //Holl_pulse_temp=Ell_Data.Motor_Current_Position;
            
            //关闭收敛功能
            if(Device_State_Data.convergent == CONVERGENT_ON)
            {
                Device_State_Data.convergent = CONVERGENT_OFF;
            }
            if(Device_State_Data.convergent_all == CONVERGENT_ON)
            {
                Device_State_Data.convergent_all = CONVERGENT_OFF;
            }
        }
        break;
        
        case MOTOR_UP_STATE://电机上升
        {
            if((!Flag.Motor_down_to_up) && (!Flag.Motor_raise_up)&&!Flag.Yz_A)//执行电机上升
            {
                if(!Flag.A_Enable_Test_Over_Wight_flag)//电机有暂停
                {
                    Flag.A_Motor_Start_up = 1;					
                }

                motor_up();
                Time.Over_Wight_beepnum = ALARM_OVER_WEIGHT_BEEP_NUM;
                Time.Motor_Err_beepnum = ALARM_MOTOR_BEEP_NUM;
                if(!Flag.Motor_run_now)
                {
                    Time.Motor_Run_time = 0;
                    //Holl_pulse_temp=Ell_Data.Motor_Current_Position;
                }
                Flag.Motor_run_now = 1;		
            }
            else//电机换向停止过程
            {
                motor_stop();
                Flag.Motor_run_now = 0;
                Time.Motor_Run_time = 0;
                Flag.A_Motor_Start_up = 0;
                Time.A_Motor_Start_Up_time = 0;
                //Holl_pulse_temp=Ell_Data.Motor_Current_Position;
            }
        }
        break;
        
        case MOTOR_DOWN_STATE://电机下降
        {
         
            Flag.Motor_all_up = 0;
            Flag.Enable_Test_Over_Wight_flag = 0;
            Flag.Enable_Test_Motor_Obs_flag = 0;
            if(!Flag.Motor_up_to_down&&!Flag.Yz_A)//执行电机下降
            {
							
							//关闭收敛功能
							if(Device_State_Data.convergent == CONVERGENT_ON)
							{
						  	Device_State_Data.convergent = CONVERGENT_OFF;
							}
							if(Device_State_Data.convergent_all == CONVERGENT_ON)
							{
						  	Device_State_Data.convergent_all = CONVERGENT_OFF;
							}
 
							  Flag.Motor_run_now = 1;
							  motor_down();
                Time.Motor_Err_beepnum = ALARM_MOTOR_BEEP_NUM;
                if(!Flag.Motor_run_now)
                {
                    Time.Motor_Run_time = 0;
                    Holl_pulse_temp = Ell_Data.Motor_Current_Position;
                }
              
            }
            else//电机换向停止过程
            {
                motor_stop();
                Time.Motor_Run_time = 0;
                Flag.Motor_run_now = 0;
                Flag.A_Motor_Start_up = 0;
                Time.A_Motor_Start_Up_time = 0;
                //Holl_pulse_temp=Ell_Data.Motor_Current_Position;
            }
        }
        break;
				case  MOTOR_UPLIM_STATE:
				{
				
							Flag.Motor_all_up = 0;
							Flag.Enable_Test_Over_Wight_flag = 0;//检测双杆超重操作
							Flag.A_Enable_Test_Over_Wight_flag = 0;
							Flag.Enable_Test_Motor_Obs_flag = 0;
							motor_stop();
							Flag.No_judge_electronic_limit = 0;	
							Flag.Motor_run_now = 0;
							Time.Motor_Run_time = 0;
							Flag.A_Motor_Start_up = 0;
							Time.A_Motor_Start_Up_time = 0;
							Time.Motor_Err_time = 0;
							//Holl_pulse_temp=Ell_Data.Motor_Current_Position;

							//关闭收敛功能
							if(Device_State_Data.convergent == CONVERGENT_ON)
							{
							Device_State_Data.convergent = CONVERGENT_OFF;
							}
							if(Device_State_Data.convergent_all == CONVERGENT_ON)
							{
						  	Device_State_Data.convergent_all = CONVERGENT_OFF;
							}
  
				}
				  break;
    }
}

void Cmd_Motor_Up(void)
{
	Flag.Run_to_set_position_flag=0;
	Err.err_bit.Motor_Obs=0;//碰撞遇阻过程被打断后,也需要清除故障
	if((Flag.Motor_down_to_up==0)&&(Flag.Motor_up_to_down==0))//电机换向延时时间及超重时不响应电机操作
	{
		if(Flag.A_Up_limit&&Device_State_Data.Motor_State_Byte!=MOTOR_UPLIM_STATE)
		{
			Device_State_Data.Motor_Position=0;
			Ell_Data.Motor_Current_Position=UP_POSITION_VALUE;
			Device_State_Data.Motor_State_Byte=MOTOR_UP_STOP_STATE;			
		}
		if(Err.err_bit.Encounter_Obstacle==1)//遇阻状态重新报遇阻提示音
		{
			Err.err_bit.Encounter_Obstacle=0;
			Time.Encounter_Obstacle_beepnum=0;
			Up_State_Data.Err_Byte&=0xfe;//重新上报遇阻
		}
		else
		{
		if((!Flag.A_Up_limit)&&((!(Ell_Data.Motor_Current_Position<=(set_up_limit_value+ONE_PERCENT_PULSE-10))||Flag.No_judge_electronic_limit))&&(current_state==0))//没有到达电子上限位和机械上限位、非超重下降状态
			{
				if((Device_State_Data.Motor_State_Byte==MOTOR_UP_STOP_STATE)
			      	||(Device_State_Data.Motor_State_Byte==MOTOR_SUSPEND_STATE)
				||(Device_State_Data.Motor_State_Byte==MOTOR_DOWN_STOP_STATE)||(Device_State_Data.Motor_State_Byte==MOTOR_UPLIM_STATE))//当前是暂停状态
				{
					SetupBzhx(MOTOR_UP_BEEP,Flash_Data.Beep_open);
					Device_State_Data.Motor_State_Byte=MOTOR_UP_STATE;
					Flag.Motor_run_now=0;	
					if(Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STATE)
					{
						Flag.Motor_all_up=1;
					}
				}
				else if(Device_State_Data.Motor_State_Byte==MOTOR_UP_STATE)//当前是上升状态
				{
					SetupBzhx(MOTOR_STOP_BEEP,Flash_Data.Beep_open);
					Device_State_Data.Motor_State_Byte=MOTOR_SUSPEND_STATE;
					Flag.Motor_run_now=0;
				}
				else if(Device_State_Data.Motor_State_Byte==MOTOR_DOWN_STATE)//当前是下降状态，换向
				{
					Device_State_Data.Motor_State_Byte=MOTOR_UP_STATE;
					Flag.Motor_run_now=0;
			
					Flag.Motor_down_to_up=1;
					if(Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STATE)
					{
						Flag.Motor_all_up=1;
					}
				}
			}
		}
	}
}
void Cmd_Motor_Up_B(void)
{
	Flag.B_Run_to_set_position_flag=0;
	Err_B.err_bit.Motor_Obs=0;
	if((Flag.B_Motor_down_to_up==0)&&(Flag.B_Motor_up_to_down==0))//电机换向延时时间及超重时不响应电机操作
	{
		if(Flag.B_Up_limit&&Device_State_Data.B_Motor_State_Byte!=MOTOR_UPLIM_STATE)
		{
			Device_State_Data.B_Motor_Position=0;
			Ell_Data.B_Motor_Current_Position=UP_POSITION_VALUE;
			Device_State_Data.B_Motor_State_Byte=MOTOR_UP_STOP_STATE;			
		}
		if(Err_B.err_bit.Encounter_Obstacle==1)//遇阻状态重新报遇阻提示音
		{
			Err_B.err_bit.Encounter_Obstacle=0;
			Time.B_Encounter_Obstacle_beepnum=0;
			Up_State_Data.B_Err_Byte&=0xfe;//重新上报遇阻
		}
		else
		{
			if((!Flag.B_Up_limit)&&((!(Ell_Data.B_Motor_Current_Position<=(B_set_up_limit_value+ONE_PERCENT_PULSE-10))||Flag.B_No_judge_electronic_limit))&&(current_state_B==0))//没有到达电子上限位和机械上限位、非超重下降状态
			{
				if((Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STOP_STATE)
			      	||(Device_State_Data.B_Motor_State_Byte==MOTOR_SUSPEND_STATE)
				||(Device_State_Data.B_Motor_State_Byte==MOTOR_DOWN_STOP_STATE)||(Device_State_Data.B_Motor_State_Byte==MOTOR_UPLIM_STATE))//当前是暂停状态
				{
					SetupBzhx(MOTOR_UP_BEEP,Flash_Data.Beep_open);
					Device_State_Data.B_Motor_State_Byte=MOTOR_UP_STATE;
					Flag.B_Motor_run_now=0;	
					if(Device_State_Data.Motor_State_Byte==MOTOR_UP_STATE)
					{
						Flag.Motor_all_up=1;
					}
				}
				else if(Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STATE)//当前是上升状态
				{
					SetupBzhx(MOTOR_STOP_BEEP,Flash_Data.Beep_open);
					Device_State_Data.B_Motor_State_Byte=MOTOR_SUSPEND_STATE;
					Flag.B_Motor_run_now=0;
				}
				else if(Device_State_Data.B_Motor_State_Byte==MOTOR_DOWN_STATE)//当前是下降状态，换向
				{
					Device_State_Data.B_Motor_State_Byte=MOTOR_UP_STATE;
					Flag.B_Motor_run_now=0;
				
					Flag.B_Motor_down_to_up=1;
					if(Device_State_Data.Motor_State_Byte==MOTOR_UP_STATE)
					{
						Flag.Motor_all_up=1;
					}
				}
			}
		}
	}
}
void Cmd_Motor_Up_Longpress(void)//
{
	
	Flag.Run_to_set_position_flag=0;
	Err.err_bit.Motor_Obs=0;//碰撞遇阻过程被打断后,也需要清除故障
	if((Flag.Motor_down_to_up==0)&&(Flag.Motor_up_to_down==0))//电机换向延时时间及超重时不响应电机操作
	{
		if(Err.err_bit.Encounter_Obstacle==1)//遇阻状态重新报遇阻提示音
		{
			Err.err_bit.Encounter_Obstacle=0;
			Time.Encounter_Obstacle_beepnum=0;
			Up_State_Data.Err_Byte&=0xfe;//重新上报遇阻
		}
		else
		{
			if((!Flag.A_Up_limit)&&(Err.err_bit.Encounter_Obstacle==0))//执行长按上升
			{
				if((Device_State_Data.Motor_State_Byte==MOTOR_UP_STOP_STATE)
			      	||(Device_State_Data.Motor_State_Byte==MOTOR_SUSPEND_STATE)
				||(Device_State_Data.Motor_State_Byte==MOTOR_DOWN_STOP_STATE)||(Device_State_Data.Motor_State_Byte==MOTOR_UPLIM_STATE))//当前是暂停状态
				{
					SetupBzhx(MOTOR_UP_BEEP,Flash_Data.Beep_open);
					Device_State_Data.Motor_State_Byte=MOTOR_UP_STATE;
					Flag.Motor_run_now=0;	
					Flag.Run_to_set_position_flag=1;
					set_position_pulse=0;	
					Device_State_Data.convergent=CONVERGENT_ON;
					if(Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STATE)
					{
						Flag.Motor_all_up=1;
					}
					
				}
				else if(Device_State_Data.Motor_State_Byte==MOTOR_DOWN_STATE)//当前是下降状态，换向
				{
					Device_State_Data.Motor_State_Byte=MOTOR_UP_STATE;
					Flag.Motor_run_now=0;
					Flag.Motor_down_to_up=1;
					Flag.Run_to_set_position_flag=1;
					set_position_pulse=0;
					Device_State_Data.convergent=CONVERGENT_ON;
					if(Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STATE)
					{
						Flag.Motor_all_up=1;
					}					
				}	
			}
		}
	}		
}
void Cmd_Motor_Up_Longpress_B(void)//
{

	Flag.B_Run_to_set_position_flag=0;
	Err_B.err_bit.Motor_Obs=0;
	if((Flag.B_Motor_down_to_up==0)&&(Flag.B_Motor_up_to_down==0))//电机换向延时时间及超重时不响应电机操作
	{
		if(Err_B.err_bit.Encounter_Obstacle==1)//遇阻状态重新报遇阻提示音
		{
			Err_B.err_bit.Encounter_Obstacle=0;
			Time.B_Encounter_Obstacle_beepnum=0;
			Up_State_Data.B_Err_Byte&=0xfe;//重新上报遇阻
		}
		else
		{
			if((!Flag.B_Up_limit)&&(Err_B.err_bit.Encounter_Obstacle==0))//执行长按上升
			{
				if((Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STOP_STATE)
			      	||(Device_State_Data.B_Motor_State_Byte==MOTOR_SUSPEND_STATE)
				||(Device_State_Data.B_Motor_State_Byte==MOTOR_DOWN_STOP_STATE)||(Device_State_Data.B_Motor_State_Byte==MOTOR_UPLIM_STATE))//当前是暂停状态
				{
					SetupBzhx(MOTOR_UP_BEEP,Flash_Data.Beep_open);
					Device_State_Data.B_Motor_State_Byte=MOTOR_UP_STATE;
					Flag.B_Motor_run_now=0;	
					Flag.B_Run_to_set_position_flag=1;
					set_position_pulse_B=0;	
					Device_State_Data.B_convergent=CONVERGENT_ON;
					if(Device_State_Data.Motor_State_Byte==MOTOR_UP_STATE)
					{
						Flag.Motor_all_up=1;
					}
				}
				else if(Device_State_Data.B_Motor_State_Byte==MOTOR_DOWN_STATE)//当前是下降状态，换向
				{
					Device_State_Data.B_Motor_State_Byte=MOTOR_UP_STATE;
					Flag.B_Motor_run_now=0;
					Flag.B_Motor_down_to_up=1;
					Flag.B_Run_to_set_position_flag=1;
					set_position_pulse_B=0;
					Device_State_Data.B_convergent=CONVERGENT_ON;
					if(Device_State_Data.Motor_State_Byte==MOTOR_UP_STATE)
					{
						Flag.Motor_all_up=1;
					}
				}	
			}
		}
	}		
}


void motor_work_B(uint8_t motor_state)
{
    switch(motor_state)
    {
        case MOTOR_UP_STOP_STATE://电机停止状态，执行电机停止
				case MOTOR_UPLIM_STATE://电机停止状态，执行电机停止	
        case MOTOR_DOWN_STOP_STATE:
        case MOTOR_SUSPEND_STATE:
        {
            Flag.Motor_all_up = 0;
            Flag.Enable_Test_Over_Wight_flag = 0;//检测双杆超重操作
            Flag.B_Enable_Test_Over_Wight_flag = 0;
            Flag.B_Enable_Test_Motor_Obs_flag = 0;//
            motor_stop_B();
            Flag.B_No_judge_electronic_limit = 0;	
            Flag.B_Motor_run_now = 0;
            Time.B_Motor_Run_time = 0;
            Flag.B_Motor_Start_up = 0;
            Time.B_Motor_Start_Up_time = 0;
            Time.B_Motor_Err_time = 0;
            //Holl_pulse_temp_B=Ell_Data.B_Motor_Current_Position;
            
            //关闭B电机收敛功能
            if(Device_State_Data.B_convergent == CONVERGENT_ON)
            {
                Device_State_Data.B_convergent = CONVERGENT_OFF;
            }
        }
        break;
        
        case MOTOR_UP_STATE://电机上升
        {
            if((!Flag.B_Motor_down_to_up) && (!Flag.B_Motor_raise_up)&&!Flag.Yz_B)//执行电机上升
            {
                if(!Flag.B_Enable_Test_Over_Wight_flag)//电机有暂停
                {
                    Flag.B_Motor_Start_up = 1;					
                }
                motor_up_B();
                Time.B_Over_Wight_beepnum = ALARM_OVER_WEIGHT_BEEP_NUM;
                Time.B_Motor_Err_beepnum = ALARM_MOTOR_BEEP_NUM;
                if(!Flag.B_Motor_run_now)
                {
                    Time.B_Motor_Run_time = 0;
                    Holl_pulse_temp_B = Ell_Data.B_Motor_Current_Position;
                }
                Flag.B_Motor_run_now = 1;		
            }
            else//电机换向停止过程
            {
                motor_stop_B();
                Flag.B_Motor_run_now = 0;
                Time.B_Motor_Run_time = 0;
                Flag.B_Motor_Start_up = 0;
                Time.B_Motor_Start_Up_time = 0;
                //Holl_pulse_temp_B=Ell_Data.B_Motor_Current_Position;
            }
        }
        break;
        
        case MOTOR_DOWN_STATE://电机下降
        {
         
            Flag.Motor_all_up = 0;
            Flag.Enable_Test_Over_Wight_flag = 0;
            Flag.B_Enable_Test_Motor_Obs_flag = 0;
            if(!Flag.B_Motor_up_to_down&&!Flag.Yz_B)//执行电机下降
            {
															//关闭B电机收敛功能
								if(Device_State_Data.B_convergent == CONVERGENT_ON)
								{
										Device_State_Data.B_convergent = CONVERGENT_OFF;
								}
							  motor_down_B();
							  Flag.B_Motor_run_now = 1;
                Time.B_Motor_Err_beepnum = ALARM_MOTOR_BEEP_NUM;
                if(!Flag.B_Motor_run_now)
                {
                    Time.B_Motor_Run_time = 0;
                    Holl_pulse_temp_B = Ell_Data.B_Motor_Current_Position;
                }
               
            }
            else//电机换向停止过程
            {
                motor_stop_B();
                Time.B_Motor_Run_time = 0;
                Flag.B_Motor_run_now = 0;
                Flag.B_Motor_Start_up = 0;
                Time.B_Motor_Start_Up_time = 0;
                Holl_pulse_temp_B = Ell_Data.B_Motor_Current_Position;
            }
        }
        break;

    }
}
static  uint8_t  u16CHxACompare;
static  uint8_t  u16CHxbCompare;
/**
  * @brief  电机a速度控制函数
  * @param  motor_state 电机状态（参数未使用，可考虑移除）
  * @note   该函数用于控制电机A的PWM输出，实现软启动和软停止功能
  */
void Motor_speed()
{   
    static uint8_t stepCounter = 0;     
    static uint8_t laststate = 0;      
    volatile float angle = (float)stepCounter / (float)PWM_MAX_COMPARE * 3.1415926f / 2.0f;  
    volatile float sinRatio = sinf(angle);   
    u16CHxACompare = (uint8_t)(sinRatio * PWM_MAX_COMPARE); 
    volatile  uint16_t pwmDuty = PWM_BASE_VALUE + (uint16_t)((PWM_MAX_VALUE - PWM_BASE_VALUE) * sinRatio);
    volatile  uint16_t pwmDuty2 = (PWM_MAX_VALUE * sinRatio);
    volatile M0P_TIM0_MODE23_TypeDef *pstcM0PBt = (M0P_TIM0_MODE23_TypeDef *)((uint32_t)M0P_TIM0_MODE23 + 0x100 * 0);
#ifdef  NO_PWM_MOTOR	
    if((Soft_start_moveA == PWM_DOWN || Soft_start_moveA == PWM_UP))
    {
        
    }
    else if(Soft_start_moveA == PWM_STOP)
    {
        MOTOR_DN_IO(0);
        MOTOR_UP_IO(0);
    }
#else  
    if((Soft_start_moveA == PWM_DOWN || Soft_start_moveA == PWM_UP) && u16CHxACompare <PWM_MAX_COMPARE)
    {
        if(laststate != Soft_start_moveA)
        {
            stepCounter = 0;
            laststate = Soft_start_moveA;
        }
        
        MotorA_PWM_Start;
        
        if (stepCounter < PWM_MAX_COMPARE) 
        {
            stepCounter   +=   PWM_START_SPEED;
        }
        // 设置PWM输出
        if (stepCounter >= PWM_MAX_COMPARE) 
        {

								MotorA_PWM_MAX;  // 达到最大值
				      	u16CHxACompare=PWM_MAX_COMPARE;
        } 
        else 
        {
							MotorA_PWM_Start;
							SET_MOTOR_PWM((uint16_t)pwmDuty);
        }
    }
    if(Soft_start_moveA == PWM_STOP)
    {
        if(Flag.A_Up_limit == 1 || Err.err_bit.Motor_Obs == 1)
        {
            MOTOR_DN_IO(0);
            MOTOR_UP_IO(0);
            u16CHxACompare = 0;
            MotorA_PWM_Stop;
        }
        if(laststate != Soft_start_moveA)
        {
            
            laststate = Soft_start_moveA;
        }
				 // 安全写法：只有 大于 停止速度，才递减
				if (stepCounter > PWM_STOP_SPEED) 
				{
						stepCounter -= PWM_STOP_SPEED;
				}
        // 设置PWM输出
        if (stepCounter <= PWM_STOP_SPEED) 
        {
            MOTOR_DN_IO(0);
            MOTOR_UP_IO(0);
            SET_MOTOR_PWM(0);
            MotorA_PWM_Stop;
					  u16CHxACompare=0;
            stepCounter = 0;
        } 
        else 
        {
            MotorA_PWM_Start;
            SET_MOTOR_PWM((uint16_t)pwmDuty2);
        }		
    }
#endif
}
void Motor_speed_B(void); 
/**
  * @brief  电机B速度控制函数
  * @param  motor_state 电机状态（参数未使用，可考虑移除）
  * @note   该函数用于控制电机B的PWM输出，实现软启动和软停止功能
  */
void Motor_speed_B(void)
{		  
    static  uint8_t stepCounterB = 0;    // 步进计数器
    static uint8_t laststate = 0;      // 最大步数 (对应500ms，假设5ms一步)
        // 获取TIM0模式23的寄存器基地址，偏移0x100*0表示使用TIM0的特定通道
    volatile M0P_TIM0_MODE23_TypeDef *pstcM0PBt = 
        (M0P_TIM0_MODE23_TypeDef *)((uint32_t)M0P_TIM0_MODE23 + 0x100 * 0);
  
    volatile float angle = (float)stepCounterB / (float)PWM_MAX_COMPARE * 3.1415926f / 2.0f;  // 计算正弦半曲线比例 (0 ~ π/2)
    volatile  float sinRatio = sinf(angle);  // 从1 到0
    
		// 直接更新 u16CHxACompare (0 ~ PWM_MAX_COMPARE)
		u16CHxbCompare = (uint16_t)(sinRatio * PWM_MAX_COMPARE);

		// 计算实际PWM占空比
	 volatile	uint16_t pwmDuty = PWM_BASE_VALUE + (uint16_t)((PWM_MAX_VALUE - PWM_BASE_VALUE) * sinRatio);//加速

		// 计算实际PWM占空比
	 volatile	uint16_t pwmDuty2 = (PWM_MAX_VALUE  * sinRatio);//减速
#ifdef  NO_PWM_MOTOR	
    if ((Soft_start_moveB == PWM_DOWN || Soft_start_moveB == PWM_UP)  )
    {
        
    }
    else  if(Soft_start_moveB == PWM_STOP)
    {
        B_MOTOR_DN_IO(0);    // 关闭电机下降控制
        B_MOTOR_UP_IO(0);    // 关闭电机上升控制
    }
    
#else  
    /* 软启动过程：当电机处于上升或下降状态且比较值小于100时 */
    if ((Soft_start_moveB == PWM_DOWN || Soft_start_moveB == PWM_UP) && 
        (u16CHxbCompare < PWM_MAX_COMPARE))
    {  
        if(laststate != Soft_start_moveB)
        {
            stepCounterB = 0;
            laststate = Soft_start_moveB;
        }

        if (stepCounterB < PWM_MAX_COMPARE) 
        {
            stepCounterB +=  PWM_START_SPEED;
        }
        MotorB_PWM_Start;
        // 设置PWM输出
        if (stepCounterB >= PWM_MAX_COMPARE) 
        {

				   MotorB_PWM_MAX;  // 达到最大值
				   u16CHxbCompare=PWM_MAX_COMPARE;
			
        } 
        else 
        {	 
					 
				  	SET_MOTOR_PWM_B((uint16_t)pwmDuty);
        }
    }
    
    /* 软停止过程：当电机需要停止时 */
    if (Soft_start_moveB == PWM_STOP)
    {			    
        if(laststate != Soft_start_moveB)
        {
            
            laststate = Soft_start_moveB;
        }
        // 如果检测到顶部过载信号，立即停止电机并重置比较值
        if (Flag.B_Up_limit == 1 || Err_B.err_bit.Motor_Obs == 1)
        {
            B_MOTOR_DN_IO(0);    // 关闭电机下降控制
            B_MOTOR_UP_IO(0);    // 关闭电机上升控制
            u16CHxbCompare = 0;  // 重置比较值
            MotorB_PWM_Stop;
        }
        if (stepCounterB >PWM_START_SPEED) 
        {
            stepCounterB -=  PWM_STOP_SPEED;
        }
        if (stepCounterB <= PWM_START_SPEED) 
        {
            B_MOTOR_DN_IO(0);
            B_MOTOR_UP_IO(0);
            SET_MOTOR_PWM_B(0);  // 设置PWM输出为0
            MotorB_PWM_Stop;
            stepCounterB = 0;
            u16CHxbCompare = 0;
        } 
        else 
        {
            MotorB_PWM_Start;
            SET_MOTOR_PWM_B(pwmDuty2);
        }		
    } 
#endif
}



		






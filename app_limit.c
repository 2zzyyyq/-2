#include "ALL_Var.h"
static uint8_t DEBUG =0;
/*限位检测保护*/
void limit_protect(void)//
{	
    /*限位信号滤波处理 - 当限位信号消失时清零计时器*/
    if(!UP_LIMIT)    
    {
        Time.return_to_up_time = 0;
			  Time.Up_Limit_Set_time=0;
        Flag.A_Up_limit = 0;
    }
//    if(!DOWN_LIMIT)
//    {
//        Time.return_to_down_time = 0;
//			  Time.Up_B_Limit_Set_time=0;
//        Flag.A_Down_limit = 0;
//    }
    
    /*非强制下降状态下的限位保护*/
    if(!Flag.Motor_force_down)
    {
        if(UP_LIMIT)//到达上机械限位
        {
            if(Time.return_to_up_time >= LIMIT_SIGNAL_FITER_TIME)
            {
                Flag.A_Up_limit = 1;
                Flag.Enable_Test_Motor_Obs_flag = 0;//不检测碰撞保护
                Time.return_to_up_time = 0;
							  //Time.Up_Limit_Set_time=0;
                if(Device_State_Data.Motor_State_Byte == MOTOR_UP_STATE)    //在上升状态的情况下触发上线位
                {		
                    Device_State_Data.Motor_Position = 0;
                    Ell_Data.Motor_Current_Position = UP_POSITION_VALUE;
                    Device_State_Data.Motor_State_Byte = MOTOR_UP_STOP_STATE;
                    SetupBzhx(LONG_BEEP, Flash_Data.Beep_open);
                    Motor_Circle_num++;
                    if(Motor_Circle_num == 10)
                    {
                        Motor_Circle_num = 0;
                        Ell_Data.Motor_Run_Circle++;
                        if(Ell_Data.Motor_Run_Circle >= MOTOR_MAX_CIRCLE)
                        {
                            Ell_Data.Motor_Run_Circle = MOTOR_MAX_CIRCLE;	
                        }
                    }
                }
            }
                  /*电子下限位保护*/
                if(((Ell_Data.Motor_Current_Position > (set_down_limit_value)) && (!Flag.No_judge_electronic_limit)) ||
                   ((Ell_Data.Motor_Current_Position > DOWN_POSITION_VALUE) && (Flag.No_judge_electronic_limit)))//到达电子下限位
                {	
                    Flag.ELE_LIM_DOWN = 1;
                    if(Device_State_Data.Motor_State_Byte == MOTOR_DOWN_STATE)
                    {
                        Device_State_Data.Motor_State_Byte = MOTOR_UPLIM_STATE;
                        SetupBzhx(LONG_BEEP, Flash_Data.Beep_open);
                    }
                }else if(Time.Up_Limit_Set_time >= LIMIT_SIGNAL_SET_TIME&&Flag.Run_to_set_position_flag !=1)
									 {
												if(Device_State_Data.Motor_State_Byte == MOTOR_DOWN_STATE) 
												{
													Time.Up_Limit_Set_time =0;
													Device_State_Data.Motor_State_Byte = MOTOR_UPLIM_STATE;
													SetupBzhx(LONG_BEEP, Flash_Data.Beep_open);
												}
									 }
        }
        //下机械限位保护代码被注释
        //else if(DOWN_LIMIT)//到达下机械限位
        //{
        //    if(Time.return_to_down_time>=LIMIT_SIGNAL_FITER_TIME)
        //    {
        //        Flag.A_Down_limit=1;
        //        Device_State_Data.Motor_Position=100;	
        //        Time.return_to_down_time=0;
        //        if(Device_State_Data.Motor_State_Byte==MOTOR_DOWN_STATE)
        //        {			
        //            Device_State_Data.Motor_State_Byte=MOTOR_DOWN_STOP_STATE;
        //            SetupBzhx(LONG_BEEP,Flash_Data.Beep_open);			
        //        }
        //    }
        //}		
        else
        {			
            /*运行到指定位置模式*/
            if(Flag.Run_to_set_position_flag == 1)
            {
                if(set_position_pulse == 0)
                {
                    if(UP_LIMIT)//到达上机械限位
                    {
                        if(Time.return_to_up_time >= LIMIT_SIGNAL_FITER_TIME)
                        {
                            Flag.A_Up_limit = 1;
                            Flag.Enable_Test_Motor_Obs_flag = 0;//不检测碰撞保护
                            Time.return_to_up_time = 0;
                            if(Device_State_Data.Motor_State_Byte == MOTOR_UP_STATE)
                            {		
                                Device_State_Data.Motor_Position = 0;
                                Ell_Data.Motor_Current_Position = UP_POSITION_VALUE;
                                Device_State_Data.Motor_State_Byte = MOTOR_UP_STOP_STATE;
                                SetupBzhx(LONG_BEEP, Flash_Data.Beep_open);
                                Motor_Circle_num++;
                                if(Motor_Circle_num == 10)
                                {
                                    Motor_Circle_num = 0;
                                    Ell_Data.Motor_Run_Circle++;
                                    if(Ell_Data.Motor_Run_Circle >= MOTOR_MAX_CIRCLE)
                                    {
                                        Ell_Data.Motor_Run_Circle = MOTOR_MAX_CIRCLE;	
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    /*WIFI控制下降到指定位置*/
                    if(Device_State_Data.Motor_State_Byte == MOTOR_DOWN_STATE)
                    {
                        /*到达指定位置*/
                        if((Ell_Data.Motor_Current_Position) >= (set_position_pulse - ONE_PERCENT_PULSE))
                        {
                            Flag.Run_to_set_position_flag = 0;
                            if(set_position_pulse < set_down_limit_value)
                            {
                                Device_State_Data.Motor_State_Byte = MOTOR_SUSPEND_STATE;
                            }
                            else
                            {
                                Device_State_Data.Motor_State_Byte = MOTOR_DOWN_STOP_STATE;
                            }
                            SetupBzhx(LONG_BEEP, Flash_Data.Beep_open);
                            Err.err_bit.Motor_Obs = 0;
                        }
                    }
                    /*WIFI控制上升到指定位置*/
                    else if(Device_State_Data.Motor_State_Byte == MOTOR_UP_STATE)
                    {
                        if((Ell_Data.Motor_Current_Position) <= (set_position_pulse + HALL_OFFSET_COMP_VAL))
                        {
                            Flag.Run_to_set_position_flag = 0;
                            if(set_position_pulse > set_up_limit_value)
                            {
                                Device_State_Data.Motor_State_Byte = MOTOR_SUSPEND_STATE;
                            }
                            else
                            {
                                Device_State_Data.Motor_State_Byte = MOTOR_UP_STOP_STATE;
                            }
                            SetupBzhx(LONG_BEEP, Flash_Data.Beep_open);
                        }
                    }
                }
            }
            /*非指定位置运行模式*/
            else
            {
                /*电子下限位保护*/
                if(((Ell_Data.Motor_Current_Position > (set_down_limit_value- ONE_PERCENT_PULSE)) && (!Flag.No_judge_electronic_limit)) ||
                   ((Ell_Data.Motor_Current_Position >  (DOWN_POSITION_VALUE- ONE_PERCENT_PULSE)) && (Flag.No_judge_electronic_limit)))//到达电子下限位
                {	
                    Flag.ELE_LIM_DOWN = 1;
                    if(Device_State_Data.Motor_State_Byte == MOTOR_DOWN_STATE)
                    {
                        Device_State_Data.Motor_State_Byte = MOTOR_DOWN_STOP_STATE;
                        SetupBzhx(LONG_BEEP, Flash_Data.Beep_open);
//                        if(DEBUG)//运行到下限位自动上升
//                        {
//                            Device_State_Data.Motor_State_Byte = MOTOR_UP_STATE;
//                            Flag.Motor_run_now = 0;
//                            //SetupBzhx(MOTOR_UP_BEEP);//换向完再响蜂鸣器
//                            Flag.Motor_down_to_up = 1;
//                        }
                    }
                }
                /*电子上限位保护*/
                else if(((Ell_Data.Motor_Current_Position < (set_up_limit_value+HALL_OFFSET_COMP_VAL)) && (!Flag.No_judge_electronic_limit)) ||
                        ((Flag.No_judge_electronic_limit) && (Ell_Data.Motor_Current_Position < UP_ELE_VALUE)))//到电子上限位
                {
                    Flag.ELE_LIM = 1;
                    if(Device_State_Data.Motor_State_Byte == MOTOR_UP_STATE)
                    {
                      
                        Device_State_Data.Motor_State_Byte = MOTOR_UP_STOP_STATE;
                        SetupBzhx(LONG_BEEP, Flash_Data.Beep_open);
                        Motor_Circle_num++;
                        if(Motor_Circle_num == 10)
                        {
                            Motor_Circle_num = 0;
                            Ell_Data.Motor_Run_Circle++;
                            if(Ell_Data.Motor_Run_Circle >= MOTOR_MAX_CIRCLE)
                            {
                                Ell_Data.Motor_Run_Circle = MOTOR_MAX_CIRCLE;	
                            }
                        }
                    }
//                    /*调试模式下自动下降*/
//                    if(DEBUG)
//                    {
//                        if(Device_State_Data.Motor_State_Byte == MOTOR_UP_STOP_STATE)	
//                        {
//                            if(Time.motor_debug_time > 100000)
//                            {	
//                                Device_State_Data.Motor_State_Byte = MOTOR_DOWN_STATE;
//                                SetupBzhx(MOTOR_DOWN_BEEP, Flash_Data.Beep_open);
//                                Flag.Motor_run_now = 0;
//                                Time.motor_debug_time = 0;
//                            }
//                        }
//                    }
                }
                /*未到达电子限位*/
                else 
                {
                    Flag.ELE_LIM = 0; 
                    Flag.ELE_LIM_DOWN = 0;					
                }
            }
        }
    }
}
void limit_protect_B(void)//
{	
    /*限位信号滤波处理*/
    if(!B_UP_LIMIT)
    {
        Time.B_return_to_up_time = 0;
			  Time.Up_B_Limit_Set_time=0;
        Flag.B_Up_limit = 0;
    }
//    if(!B_DOWN_LIMIT)
//    {
//        Time.B_return_to_down_time = 0;
//        Flag.B_Down_limit = 0;
//    }
    
    /*非强制下降状态下的限位保护*/
    if(!Flag.B_Motor_force_down)
    {
        if(B_UP_LIMIT)//到达上机械限位
        {
            if(Time.B_return_to_up_time >= LIMIT_SIGNAL_FITER_TIME)
            {
                Flag.B_Up_limit = 1;
                Flag.B_Enable_Test_Motor_Obs_flag = 0;//不检测碰撞保护
                Time.B_return_to_up_time = 0;
			
                if(Device_State_Data.B_Motor_State_Byte == MOTOR_UP_STATE)
                {	
                    Device_State_Data.B_Motor_Position = 0;
                    Ell_Data.B_Motor_Current_Position = UP_POSITION_VALUE;
                    Device_State_Data.B_Motor_State_Byte = MOTOR_UP_STOP_STATE;
                    SetupBzhx(LONG_BEEP, Flash_Data.Beep_open);
                    Motor_Circle_num_B++;
                    if(Motor_Circle_num_B == 10)
                    {
                        Motor_Circle_num_B = 0;
                        Ell_Data.B_Motor_Run_Circle++;
                        if(Ell_Data.B_Motor_Run_Circle >= MOTOR_MAX_CIRCLE)
                        {
                            Ell_Data.B_Motor_Run_Circle = MOTOR_MAX_CIRCLE;	
                        }
                    }
                }
            }
			                 /*电子下限位保护*/
                if(((Ell_Data.B_Motor_Current_Position > (B_set_down_limit_value- ONE_PERCENT_PULSE)) && (!Flag.B_No_judge_electronic_limit)) ||
                   ((Ell_Data.B_Motor_Current_Position > DOWN_POSITION_VALUE) && (Flag.B_No_judge_electronic_limit)))
                {	
                    Flag.B_ELE_LIM_DOWN = 1;
                    if(Device_State_Data.B_Motor_State_Byte == MOTOR_DOWN_STATE)
                    {
                        Device_State_Data.B_Motor_State_Byte = MOTOR_UPLIM_STATE;
                        SetupBzhx(LONG_BEEP, Flash_Data.Beep_open);	
                    }
                }else if(Time.Up_B_Limit_Set_time >= LIMIT_SIGNAL_SET_TIME&&Flag.B_Run_to_set_position_flag != 1)
								{
										if(Device_State_Data.B_Motor_State_Byte == MOTOR_DOWN_STATE) 
										{
											Time.Up_B_Limit_Set_time=0;
									  	Device_State_Data.B_Motor_State_Byte = MOTOR_UPLIM_STATE;
								  		SetupBzhx(LONG_BEEP, Flash_Data.Beep_open);
										}
								}
	
        }
        //下机械限位保护代码被注释
        //else if(B_DOWN_LIMIT)//到达下机械限位
        //{
        //    if(Time.B_return_to_down_time>=LIMIT_SIGNAL_FITER_TIME)
        //    {
        //        Flag.B_Down_limit=1;
        //        Device_State_Data.B_Motor_Position=100;
        //        Time.B_return_to_down_time=0;
        //        if(Device_State_Data.B_Motor_State_Byte==MOTOR_DOWN_STATE)
        //        {			
        //            Device_State_Data.B_Motor_State_Byte=MOTOR_DOWN_STOP_STATE;
        //            SetupBzhx(LONG_BEEP,Flash_Data.Beep_open);			
        //        }
        //    }
        //}		
        else
        {			
            /*运行到指定位置模式*/
            if(Flag.B_Run_to_set_position_flag == 1)
            {
                if(set_position_pulse_B == 0)
                {
                    if(B_UP_LIMIT)//到达上机械限位
                    {
                        if(Time.B_return_to_up_time >= LIMIT_SIGNAL_FITER_TIME)
                        {
                            Flag.B_Up_limit = 1;
                            Flag.B_Enable_Test_Motor_Obs_flag = 0;//不检测碰撞保护
                            Time.B_return_to_up_time = 0;
                            if(Device_State_Data.B_Motor_State_Byte == MOTOR_UP_STATE)
                            {	
                                Device_State_Data.B_Motor_Position = 0;//编码值%比
                                Ell_Data.B_Motor_Current_Position = UP_POSITION_VALUE;//重制上限位编码值
                                Device_State_Data.B_Motor_State_Byte = MOTOR_UP_STOP_STATE;
                                SetupBzhx(LONG_BEEP, Flash_Data.Beep_open);
                                Motor_Circle_num_B++;
                                if(Motor_Circle_num_B == 10)//堵转值加大
                                {
                                    Motor_Circle_num_B = 0;
                                    Ell_Data.B_Motor_Run_Circle++;
                                    if(Ell_Data.B_Motor_Run_Circle >= MOTOR_MAX_CIRCLE)
                                    {
                                        Ell_Data.B_Motor_Run_Circle = MOTOR_MAX_CIRCLE;	
                                    }	
                                }
                            }
                        }
                    }
                }
                else
                {
                    /*下降过程中到达目标位置*/
                    if(Device_State_Data.B_Motor_State_Byte == MOTOR_DOWN_STATE)
                    {
                        if(Ell_Data.B_Motor_Current_Position >= (set_position_pulse_B - ONE_PERCENT_PULSE))
                        {
                            Flag.B_Run_to_set_position_flag = 0;
                            if(set_position_pulse_B < B_set_down_limit_value)
                            {
                                Device_State_Data.B_Motor_State_Byte = MOTOR_SUSPEND_STATE;
                            }
                            else
                            {
                                Device_State_Data.B_Motor_State_Byte = MOTOR_DOWN_STOP_STATE;
                            }
                            SetupBzhx(LONG_BEEP, Flash_Data.Beep_open);
                            Err_B.err_bit.Motor_Obs = 0;
                        }
                    }
                    /*上升过程中到达目标位置*/
                    else if(Device_State_Data.B_Motor_State_Byte == MOTOR_UP_STATE)
                    {
                        if(Ell_Data.B_Motor_Current_Position <= (set_position_pulse_B + HALL_OFFSET_COMP_VAL))
                        {
                            Flag.B_Run_to_set_position_flag = 0;
                            if(set_position_pulse_B > B_set_up_limit_value)
                            {
                                Device_State_Data.B_Motor_State_Byte = MOTOR_SUSPEND_STATE;
                            }
                            else
                            {
                                Device_State_Data.B_Motor_State_Byte = MOTOR_UP_STOP_STATE;
                            }
                            SetupBzhx(LONG_BEEP, Flash_Data.Beep_open);
                        }
                    }
                }
            }
            /*非指定位置运行模式*/
            else
            {
                /*电子下限位保护*/
                if(((Ell_Data.B_Motor_Current_Position > (B_set_down_limit_value- ONE_PERCENT_PULSE)) && (!Flag.B_No_judge_electronic_limit)) ||
                   ((Ell_Data.B_Motor_Current_Position > (DOWN_POSITION_VALUE- ONE_PERCENT_PULSE)) && (Flag.B_No_judge_electronic_limit)))
                {	
                    Flag.B_ELE_LIM_DOWN = 1;
                    if(Device_State_Data.B_Motor_State_Byte == MOTOR_DOWN_STATE)
                    {
                        Device_State_Data.B_Motor_State_Byte = MOTOR_DOWN_STOP_STATE;
                        SetupBzhx(LONG_BEEP, Flash_Data.Beep_open);	
//                        /*调试模式下自动上升*/
//                        if(DEBUG)
//                        {
//                            Device_State_Data.B_Motor_State_Byte = MOTOR_UP_STATE;
//                            Flag.B_Motor_run_now = 0;
//                            //SetupBzhx(MOTOR_UP_BEEP);//换向完再响蜂鸣器
//                            Time.Motor_Dir_Change_Delaytime = 500;//换向时间500ms
//                            Flag.B_Motor_down_to_up = 1;
//                        }
                    }
                }
                /*电子上限位保护*/
                else if(((Ell_Data.B_Motor_Current_Position < (B_set_up_limit_value+HALL_OFFSET_COMP_VAL)) && (!Flag.B_No_judge_electronic_limit)) ||
                        ((Ell_Data.B_Motor_Current_Position < UP_ELE_VALUE) && (Flag.B_No_judge_electronic_limit)))
                {
                    Flag.B_ELE_LIM = 1;
                    if(Device_State_Data.B_Motor_State_Byte == MOTOR_UP_STATE)
                    {
                        Device_State_Data.B_Motor_State_Byte = MOTOR_UP_STOP_STATE;
                        SetupBzhx(LONG_BEEP, Flash_Data.Beep_open);
                        Motor_Circle_num_B++;
                        if(Motor_Circle_num_B == 10)
                        {
                            Motor_Circle_num_B = 0;
                            Ell_Data.B_Motor_Run_Circle++;
                            if(Ell_Data.B_Motor_Run_Circle >= MOTOR_MAX_CIRCLE)
                            {
                                Ell_Data.B_Motor_Run_Circle = MOTOR_MAX_CIRCLE;	
                            }
                        }
                    }
//                    /*调试模式下自动下降*/
//                    if(DEBUG)
//                    {
//                        if(Device_State_Data.B_Motor_State_Byte == MOTOR_UP_STOP_STATE)	
//                        {
//                            if(Time.motor_debug_time > 100000)
//                            {	
//                                Device_State_Data.B_Motor_State_Byte = MOTOR_DOWN_STATE;
//                                SetupBzhx(MOTOR_DOWN_BEEP, Flash_Data.Beep_open);
//                              //  Flag.Motor_up_to_down = 1;//	   
//                               // Time.Motor_Dir_Change_Delaytime = 500;//换向时间500ms
//                                Flag.B_Motor_run_now = 0;
//                                Time.motor_debug_time = 0;
//                            }
//                        }
//                    }					
                }
                /*未到达电子限位*/
                else 
                {
                    Flag.B_ELE_LIM = 0;
                    Flag.B_ELE_LIM_DOWN = 0;
                }
            }
        }
    }
}

/*报警信号检测及蜂鸣器*/
void alarm_signal_test_and_beep(void)
{
    uint8_t  i;
    static uint8_t _10ms_step=0;
    static uint16_t sum=0;//10个值取平均
    static  uint8_t Fold_restore=0;
    if((Device_State_Data.Motor_State_Byte==MOTOR_UP_STATE)||(Device_State_Data.Motor_State_Byte==MOTOR_DOWN_STATE))//电机运行
    {
        if(YZ)
        {
            Time.Motor_Stop_Encounter_Obstacle_time++;
            if(Time.Motor_Stop_Encounter_Obstacle_time>=ENCOUNTER_Obstacle_TIME1)//遇阻停止
            {
                Flag.Yz_A=1;  
                Time.Motor_Stop_Encounter_Obstacle_time=0;   
                if(!Err.err_bit.Motor_Obs)//非碰撞状态
                {
                    Device_State_Data.Motor_State_Byte=MOTOR_SUSPEND_STATE;//		 
                    SetupBzhx(LONG_BEEP,1);//长鸣一下
                    Flag.Encounter_Obstacle_Motor_stop=1;//遇阻停止
                    Up_State_Data.Motor_Position=0xff;
                    Flag.Run_to_set_position_flag=0;//运行到指定位置过程被打断		 
                }
            }
        }
        else
        {
            if(Time.Motor_No_Encounter_Obstacle_time>LIMIT_SIGNAL_FITER_TIME)
            {
                Flag.Yz_A=0;
                Time.Motor_No_Encounter_Obstacle_time=0;
                if(Err.err_bit.Encounter_Obstacle)
                {
                    Err.err_bit.Encounter_Obstacle=0;
                    Time.Encounter_Obstacle_beepnum=0;
                }
            }
            Time.Motor_Stop_Encounter_Obstacle_time=0; 
        }
    }
    else//电机停止
    {
        if(YZ)
        {	
            if(!Err.err_bit.Encounter_Obstacle)//
            {	    
                if(Time.Motor_Stop_Encounter_Obstacle_time>=ENCOUNTER_Obstacle_TIME2)//遇阻信号持续3s报警
                {		
                    Flag.beep_off_5stime_flag=1;
                    Time.beep_off_5stime=0;
                    Time.Encounter_Obstacle_beepnum=0;
                    Err.err_bit.Encounter_Obstacle=1;
                }	
                else
                {
                    Time.Motor_Stop_Encounter_Obstacle_time++;
                    if(Time.Motor_Stop_Encounter_Obstacle_time>=ENCOUNTER_Obstacle_TIME1)
                    {
                        Flag.Yz_A=1;
                    }
                }
            }
            Time.Motor_No_Encounter_Obstacle_time=0;
        }
        else//
        {	
            if(Time.Motor_No_Encounter_Obstacle_time>LIMIT_SIGNAL_FITER_TIME)//真的是解除遇阻
            {
                Flag.Yz_A=0;
                Time.Motor_No_Encounter_Obstacle_time=0;
                if(Err.err_bit.Encounter_Obstacle)
                {
                    Err.err_bit.Encounter_Obstacle=0;
                    Time.Encounter_Obstacle_beepnum=0;
                  //  SetupBzhx(BEEP_PWM_OFF,1);	
                }
                else//
                {
                    if(Flag.Encounter_Obstacle_Motor_stop==1)//遇阻停止后的操作不计入轻抬上升
                    {
                        Flag.Encounter_Obstacle_Motor_stop=0;		
                    }
                    else 
                    {
                        if(ENCOUNTER_Obstacle_TIME3<Time.Motor_Stop_Encounter_Obstacle_time)//小于3s大于200ms轻抬上升
                        {
                            if((!Flag.A_Up_limit)&&(!(Ell_Data.Motor_Current_Position<set_up_limit_value)))//如果没到上限位和电子上限位
                            {
													
																	Device_State_Data.Motor_State_Byte=MOTOR_UP_STATE;//电机状态
																	Flag.Motor_raise_up=1;
																	Time.Motor_Run_time=0;
																	Holl_pulse_temp=Ell_Data.Motor_Current_Position;
                            }
                        }
                    }
                }
                Time.Motor_Stop_Encounter_Obstacle_time=0; 
            } 
        } 
    }
    
    switch(current_state)//电机超重及堵转
    {
        case 0:
        {	
            if(Motor_Current.ADvalue>MOTOR_ERR_CURRENT_ADVALUE)//大于堵转电流
            {
                Time.Judge_Motor_Err_time++;
                if(Time.Judge_Motor_Err_time>MOTOR_STALL_TIME)//100ms
                {
                    Time.Judge_Motor_Err_time=0;    
                    Flag.beep_off_5stime_flag=1;
                    Time.beep_off_5stime=0;
                    Time.Motor_Err_beepnum=0;
                    Time.Up_Data_Delay_time=1000;//延时1s上报故障
                    Err.err_bit.Motor_Err=1;
                    motor_stop();//电机停止
                    Device_State_Data.Motor_State_Byte=MOTOR_SUSPEND_STATE;//停止	
                    Flag.No_judge_electronic_limit=0;
                }
            }
            else
            {	
                if(Motor_Current.ADvalue<(MOTOR_ERR_CURRENT_ADVALUE-MOTOR_ERR_CURRENT_ADVALUE_OFFSET))
                {
                    Time.Judge_Motor_Err_time=0;
                }
                
                if(Flag.Motor_run_now)
                {
                    if(Time.Motor_Run_time<200)//继电器给信号后延时2s
                    {
                        Time.Motor_Run_time++;
                        if(Time.Motor_Run_time==95)
                        {
                            Time.Test_speed_time=0;
                            Holl_test_speed_temp=Ell_Data.Motor_Current_Position;
                        }
                        else if(Time.Motor_Run_time==195)//得到速度值，单位1%/100ms行程
                        {
                            if(Ell_Data.Motor_Current_Position>Holl_test_speed_temp)
                            {
                                Device_State_Data.current_speed=(Ell_Data.Motor_Current_Position-Holl_test_speed_temp)/2.8;
                            }
                            else if(Ell_Data.Motor_Current_Position<Holl_test_speed_temp)
                            {
                                Device_State_Data.current_speed=(Holl_test_speed_temp-Ell_Data.Motor_Current_Position)/2.8;
                            }
                        }
                        Motor_Current_Position_pulse=10000;
                        Holl_pulse_temp=10000;
                        Time.Motor_Err_time=0;
                    }
                    else
                    {
                        if(Time.Motor_Err_time++>100)//1s检测一次霍尔数值
                        {
                            Time.Motor_Err_time=0;
                            if(((Device_State_Data.Motor_State_Byte==MOTOR_UP_STATE)&&(Motor_Current_Position_pulse>=Holl_pulse_temp))//上升脉冲在变大或不变
                                ||((Device_State_Data.Motor_State_Byte==MOTOR_DOWN_STATE)&&(Motor_Current_Position_pulse<=Holl_pulse_temp)))//下降脉冲在变小或不变 
                            {
															if(Ell_Data.Motor_Current_Position<(UP_POSITION_VALUE+200)&&(Ell_Data.Motor_Current_Position>1000)&&Device_State_Data.Motor_State_Byte==MOTOR_UP_STATE)//处理因为上限位没有触发一直卡死暂停，误以为是上限位到达
															{
														      current_state=1;
																 	Fold_restore=1;
															}else
															{	
                                Flag.beep_off_5stime_flag=1;
                                Time.beep_off_5stime=0;
                                Time.Motor_Err_beepnum=0;
                                Err.err_bit.Motor_Err=1;
                                Time.Up_Data_Delay_time=1000;//延时1s上报故障，防止app在更新按键状态时清除故障信息
                                motor_stop();//电机停止
                                Flag.No_judge_electronic_limit=0;
                                Device_State_Data.Motor_State_Byte=MOTOR_SUSPEND_STATE;//暂停	
															}
                            }	
                            else
                            {
                                Holl_pulse_temp=Motor_Current_Position_pulse;
                                Err.err_bit.Motor_Err=0;							
                            }
                        }	
                    }
                }
                else
                {
                    Device_State_Data.current_speed=40;
                }
			          if((Flag.Enable_Test_Over_Wight_flag)&&(!Flag.Overweight_current_correct_start))
								{
									if((Device_State_Data.Motor_State_Byte==MOTOR_UP_STATE)&&(Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STATE))	
									{
										if((Motor_Current.ADvalue+Motor_Current_B.ADvalue)>
											(motor_current_to_advalue(Ell_Data.Motor_Run_Circle)+motor_current_to_advalue_B(Ell_Data.B_Motor_Run_Circle)))//大于超重电流
													{
															Time.Judge_Over_Wight_time++;
															if(Time.Judge_Over_Wight_time>=MOTOR_OVER_WEIGHT_TIEM)//持续超重电流
															{
																Time.Judge_Over_Wight_time=0;
																current_state=2;	
																current_state_B=2;
															}
													}
										else
										{
											if(Motor_Current.ADvalue<(motor_current_to_advalue(Ell_Data.Motor_Run_Circle)
											-OVER_WIGHT_CURRENT_ADVALUE_OFFSET))//电流波动超出超重判断回滞值
											{
												Time.Judge_Over_Wight_time=0;
												Err.err_bit.Over_Wight=0;
												Err_B.err_bit.Over_Wight=0;
											}
										}			
									}
								}
              else  if((Flag.A_Enable_Test_Over_Wight_flag)&&(!Flag.Overweight_current_correct_start))//允许检测超重
                {
								  	#ifdef	LIMIT_OVER_VALVE
												if( Ell_Data.Motor_Current_Position>(UP_POSITION_VALUE+240)&&Ell_Data.Motor_Current_Position<set_up_limit_value)
												{
	 
														if(Motor_Current.ADvalue>(motor_current_to_advalue(Ell_Data.Motor_Run_Circle)-OVER_WIGHT_CURRENT_ADVALUE_LIMIT))//大于超重电流
														{
																Time.Judge_Over_Wight_time++;
																if(Time.Judge_Over_Wight_time>=MOTOR_OVER_WEIGHT_TIEM)//持续超重电流
																{
																		Time.Judge_Over_Wight_time=0;
																		current_state=2;
																	//	Fold_restore=1;
																			 
																}
														}
														else
														{
																if(Motor_Current.ADvalue<(motor_current_to_advalue(Ell_Data.Motor_Run_Circle)-OVER_WIGHT_CURRENT_ADVALUE_LIMIT-OVER_WIGHT_CURRENT_ADVALUE_OFFSET))//电流波动超出超重判断回滞值
																{
																		Time.Judge_Over_Wight_time=0;
																		Err.err_bit.Over_Wight=0;
																}
														}
												}
											 else 
											 {
		 
														if(Motor_Current.ADvalue>motor_current_to_advalue(Ell_Data.Motor_Run_Circle))//大于超重电流
														{
																	Time.Judge_Over_Wight_time++;
																if(Time.Judge_Over_Wight_time>=MOTOR_OVER_WEIGHT_TIEM)//持续超重电流
																{
																	 Time.Judge_Over_Wight_time=0;
																	if(Ell_Data.Motor_Current_Position<(UP_POSITION_VALUE+220))
																	{
																		current_state=1;

																		Fold_restore=1;
																	}else
																	{
																		current_state=2;	
																	}

																}
														}
														else
														{
																if(Motor_Current.ADvalue<(motor_current_to_advalue(Ell_Data.Motor_Run_Circle)-OVER_WIGHT_CURRENT_ADVALUE_OFFSET))//电流波动超出超重判断回滞值
																{
																		Time.Judge_Over_Wight_time=0;
																		Err.err_bit.Over_Wight=0;
																}
														}
											 }
                      #else  
												if(Motor_Current.ADvalue>motor_current_to_advalue(Ell_Data.Motor_Run_Circle))//大于超重电流
												{
													   	Time.Judge_Over_Wight_time++;
														if(Time.Judge_Over_Wight_time>=MOTOR_OVER_WEIGHT_TIEM)//持续超重电流
														{
														   Time.Judge_Over_Wight_time=0;
															if(Ell_Data.Motor_Current_Position<(UP_POSITION_VALUE+220))
															{
																 current_state=1;
																 Fold_restore=1;
															}else
															{
																current_state=2;	
															}

														}
												}
												else
												{
														if(Motor_Current.ADvalue<(motor_current_to_advalue(Ell_Data.Motor_Run_Circle)-OVER_WIGHT_CURRENT_ADVALUE_OFFSET))//电流波动超出超重判断回滞值
														{
																Time.Judge_Over_Wight_time=0;
																Err.err_bit.Over_Wight=0;
														}
												}
											#endif
                }
                if((Flag.Enable_Test_Motor_Obs_flag)&&(Device_State_Data.Motor_Position>1))//允许检测向上遇到阻碍
                {	
                   
                    sum += Motor_Current.ADvalue;
                    _10ms_step++;	
                    if(_10ms_step>=AD_AVERAGE_NUM)//100ms
                    {
                        _10ms_step=0;
                        if(ADC_sample_num<AD_BUFF_DIP)
                        {
                            ADC_sample_num++;
                        }
                        ADC_read_buf[ADC_sample_num-1]=sum/AD_AVERAGE_NUM;
											  sum=0;

	
                        if(ADC_sample_num==AD_BUFF_DIP)
                        {		
                            ADC_sample_max=ADC_read_buf[0];
													
                            conform_motor_obs_num=0;
                         
													  for(i = 1; i < AD_BUFF_DIP; i++)
															{						
																	// 改进峰值检测：
																	if(i < (AD_BUFF_DIP - 1))
																	{
																			// 只有当下降幅度较大时才重置
																			int8_t drop1 = ADC_read_buf[i] - ADC_read_buf[i - 1];
																			int8_t drop2 = ADC_read_buf[i + 1] - ADC_read_buf[i];
																			if(drop1 < -15 && drop2 < -15)  // 下降超过3才认为是中断
																			{
																					conform_motor_obs_num = 0;
																			}
																	}
																	
																	// 峰值检测条件：当前值大于历史最大值，并且比前一个值大8以上
																	if(ADC_read_buf[i] > ADC_sample_max && 
																		 ADC_read_buf[i] - ADC_read_buf[i - 1] >= AD_DIFF_NUM)
																	{
																			ADC_sample_max = ADC_read_buf[i];
																			conform_motor_obs_num++;	
																	}
															}
/*
                            for(i=1;i<AD_BUFF_DIP;i++)
                            {						
                                if(ADC_read_buf[i]>ADC_sample_max)
                                {
                                    ADC_sample_max=ADC_read_buf[i];
                                    conform_motor_obs_num++;	

                                }
                     					 // 改进峰值检测：
																		if(i < (AD_BUFF_DIP-1))
																		{
																				// 只有当下降幅度较大时才重置
																				int8_t drop1 = ADC_read_buf[i] - ADC_read_buf[i-1];
																				int8_t drop2 = ADC_read_buf[i+1] - ADC_read_buf[i];
																				if(drop1 < -5 && drop2 < -5)  // 下降超过5才认为是中断
																				{
																						conform_motor_obs_num = 0;
																				}
																		}
                            }
*/
                            for(i=0;i<AD_BUFF_DIP-1;i++)
                            {
                                ADC_read_buf[i]=ADC_read_buf[i+1];	
                            }
											
                            if( Ell_Data.Motor_Current_Position<(UP_POSITION_VALUE+240))
                            {
																	if(conform_motor_obs_num>=(UPTREND_NUM+3))//判定为向上遇到障碍
																		{
																			Fold_restore=1;
																			conform_motor_obs_num=0;
																			current_state=1;
																		}
														 
                            }
                            else if(conform_motor_obs_num>=(UPTREND_NUM+3))//判定为向上遇到障碍
                            {
															  if( Ell_Data.Motor_Current_Position<(set_up_limit_value))
															  {
																	 Fold_restore=1;
															  }
                                conform_motor_obs_num=0;
                                current_state=1;
                            }
                        }
                    }
                }	
                else
                {
                    _10ms_step=0;
                }
            }
        }
        break;
        
        case 1:
        {
            Flag.beep_off_5stime_flag=1;
            Time.beep_off_5stime=0;
            Time.Encounter_Obstacle_beepnum=0;    
            Time.Motor_Obs_Down_time=0;
					  Err.err_bit.Motor_Obs=1; 
            Up_State_Data.Err_Byte&=0xf7;//重新上报上升遇阻
            if((Fold_restore ==1|| Device_State_Data.Motor_Position<(Device_State_Data.electronic_up_limit)))
						{
								Device_State_Data.set_position=Device_State_Data.electronic_up_limit;
								Run_to_position();
								Fold_restore=0;

						}else
						{
							  
                Flag.Run_to_set_position_flag=0;
								Flag.Motor_up_to_down=1;	
								Time.Motor_Dir_Change_Delaytime=500;//延时500ms换向
								Device_State_Data.Motor_State_Byte=MOTOR_DOWN_STATE;//下降			

						}
            current_state=4;    
        }
        break;
        
        case 2:
        {
            Flag.beep_off_5stime_flag=1;
            Time.beep_off_5stime=0;
            Time.Over_Wight_beepnum=0;
            Err.err_bit.Over_Wight=1;	
            Up_State_Data.Err_Byte&=0xfd;//重新上报超重
            Device_State_Data.Motor_State_Byte=MOTOR_DOWN_STATE;//下降
            Flag.Motor_up_to_down=1;//	   
            Time.Motor_Dir_Change_Delaytime=500;//换向时间500ms
            current_state=3;      
        }
        break;
        
        case 3:
        {
            if((Device_State_Data.Motor_State_Byte==MOTOR_DOWN_STOP_STATE)
                ||(Device_State_Data.Motor_State_Byte==MOTOR_SUSPEND_STATE))//超重后需要到下限位或暂停
            {
                current_state=0;
            }
        }
        break;
        
        case 4:
        {	
					if(Device_State_Data.Motor_Position>=(Device_State_Data.electronic_up_limit))
					{
							if(Time.Motor_Obs_Down_time++>300)
							{
									Device_State_Data.Motor_State_Byte=MOTOR_SUSPEND_STATE;
									Time.Motor_Obs_Down_time=0;	
							}
					}
					/*    每次遇阻下降300ms就停止 不判断是否上限位微动是否触发
					  Time.Motor_Stop_Encounter_Obstacle_time=0;//不做遇阻判断，判断时间清0
            if(Time.Motor_Obs_Down_time++>300)
            {
                Device_State_Data.Motor_State_Byte=MOTOR_SUSPEND_STATE;
                Time.Motor_Obs_Down_time=0;	
            }
					*/
            if((Device_State_Data.Motor_State_Byte==MOTOR_DOWN_STOP_STATE)
                ||(Device_State_Data.Motor_State_Byte==MOTOR_SUSPEND_STATE))//暂停
            {
                current_state=0;
                Err.err_bit.Motor_Obs=0;
            }
        }
        break;
        
        case 5:
        {
            Time.Motor_Stop_Encounter_Obstacle_time=0;//不做遇阻判断，判断时间清0
            if(Time.Motor_Force_Down_time++>100)
            {	
                if((Flag.Yz_A)||(Device_State_Data.Motor_Position==100))
                {
                    Device_State_Data.Motor_State_Byte=MOTOR_SUSPEND_STATE;
                }
                Time.Motor_Force_Down_time=0;	
                current_state=0;
                Flag.Motor_force_down=0;	
            }	
            if((Device_State_Data.Motor_State_Byte==MOTOR_SUSPEND_STATE)||(Device_State_Data.Motor_State_Byte==MOTOR_UP_STATE))
            {
                Time.Motor_Force_Down_time=0;	
                current_state=0;
                Flag.Motor_force_down=0;		
            }
        }
        break;
        
        default:
            break;	    
    }
    
    if(Flag.beep_off_5stime_flag)//报警提示音循环时间到
    {
        Flag.beep_off_5stime_flag=0;
        if((Err.err_bit.Encounter_Obstacle)||(Err.err_bit.Motor_Obs))
        {
            if((++Time.Encounter_Obstacle_beepnum)<=ALARM_YZ_BEEP_NUM)
            {
                /*if((Time.Encounter_Obstacle_beepnum==1)&&(Flag.Beep_open_now==1))
                {
                    Time.Encounter_Obstacle_beepnum=ALARM_YZ_BEEP_NUM;	
                }
                else
                {
                    SetupBzhx(YZ_ALARM_BEEP,1);
                }*/
                SetupBzhx(YZ_ALARM_BEEP,1);
            }
            else
            {
                Time.Encounter_Obstacle_beepnum=ALARM_YZ_BEEP_NUM;
            }
        }	
        else if(Err.err_bit.Motor_Err)
        {
            if((++Time.Motor_Err_beepnum)<=ALARM_MOTOR_BEEP_NUM)
            {	
                /*if((Time.Motor_Err_beepnum==1)&&(Flag.Beep_open_now==1))
                {
                    Time.Motor_Err_beepnum=ALARM_MOTOR_BEEP_NUM;	
                }
                else
                {
                    SetupBzhx(MOTOR_ERR_ALARM_BEEP,1);
                }*/
                SetupBzhx(MOTOR_ERR_ALARM_BEEP,1);
            }
            else
            {
                Time.Motor_Err_beepnum=ALARM_MOTOR_BEEP_NUM;
            }
        }
        else if(Err.err_bit.Over_Wight)
        {
            if((++Time.Over_Wight_beepnum)<=ALARM_OVER_WEIGHT_BEEP_NUM)
            {	    
                /*if((Time.Over_Wight_beepnum==1)&&(Flag.Beep_open_now==1))
                {
                    Time.Over_Wight_beepnum=ALARM_OVER_WEIGHT_BEEP_NUM;	
                }
                else
                {
                    SetupBzhx(OVER_WIGHT_ALARM_BEEP,1);
                }*/
                SetupBzhx(OVER_WIGHT_ALARM_BEEP,1);
            }
            else
            {
                Time.Over_Wight_beepnum=ALARM_OVER_WEIGHT_BEEP_NUM;
            }
        }
    }
    
    Device_State_Data.Err_Byte=Err.data;
    // 当Motor_Err故障延时上报激活时，预同步上报状态中的Motor_Err位，
    // 避免properties_up()立即上报7 1 3，1s后由定时器ISR清除该位触发上报
    if(Err.err_bit.Motor_Err && Time.Up_Data_Delay_time > 0)
    {
        Up_State_Data.Err_Byte |= 0x04;
    }
}

void alarm_signal_test_and_beep_B(void)
{	
    uint8_t  i;
    static uint8_t _10ms_step=0;
    static uint16_t sum=0;//10个值取平均
    static uint8_t  Fold_restoreB=0;
    if((Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STATE)||(Device_State_Data.B_Motor_State_Byte==MOTOR_DOWN_STATE))//电机运行
    {
        if(YZ_B)
        {
            Time.B_Motor_Stop_Encounter_Obstacle_time++;
            if(Time.B_Motor_Stop_Encounter_Obstacle_time>=ENCOUNTER_Obstacle_TIME1)//遇阻停止
            { 
                Flag.Yz_B=1;
                Time.B_Motor_Stop_Encounter_Obstacle_time=0;   
                if(!Err_B.err_bit.Motor_Obs)//碰撞遇阻过程
                {
                    Device_State_Data.B_Motor_State_Byte=MOTOR_SUSPEND_STATE;//		 
                    SetupBzhx(LONG_BEEP,1);//长鸣一下
                    Flag.B_Encounter_Obstacle_Motor_stop=1;//遇阻停止 
                    Up_State_Data.B_set_position=0XFF;
                    Up_State_Data.B_Motor_Position=0xff;
                    Flag.B_Run_to_set_position_flag=0;//运行到指定位置过程被打断
                }
            }
            Time.B_Motor_No_Encounter_Obstacle_time=0;
        }
        else
        {
            if(Time.B_Motor_No_Encounter_Obstacle_time>LIMIT_SIGNAL_FITER_TIME)
            {
                Flag.Yz_B=0;
                Time.B_Motor_No_Encounter_Obstacle_time=0;
                if(Err_B.err_bit.Encounter_Obstacle)
                {
                    Err_B.err_bit.Encounter_Obstacle=0;
                    Time.B_Encounter_Obstacle_beepnum=0;
                }
            }
            Time.B_Motor_Stop_Encounter_Obstacle_time=0;
        }
    }
    else//电机停止
    {
        if(YZ_B)
        {	
            if(!Err_B.err_bit.Encounter_Obstacle)//
            {	    
                if(Time.B_Motor_Stop_Encounter_Obstacle_time>=ENCOUNTER_Obstacle_TIME2)//遇阻信号持续3s报警
                {		
                    Flag.B_beep_off_5stime_flag=1;
                    Time.B_beep_off_5stime=0;
                    Time.B_Encounter_Obstacle_beepnum=0;
                    Err_B.err_bit.Encounter_Obstacle=1;
                }	
                else
                {
                    Time.B_Motor_Stop_Encounter_Obstacle_time++;
                    if(Time.B_Motor_Stop_Encounter_Obstacle_time>=ENCOUNTER_Obstacle_TIME1)
                    {
                        Flag.Yz_B=1;
                    }
                }
            }
            Time.B_Motor_No_Encounter_Obstacle_time=0;
        }
        else//
        {	
            if(Time.B_Motor_No_Encounter_Obstacle_time>LIMIT_SIGNAL_FITER_TIME)//真的是解除遇阻
            {
                Flag.Yz_B=0;
                Time.B_Motor_No_Encounter_Obstacle_time=0;
                if(Err_B.err_bit.Encounter_Obstacle)
                {
                    Err_B.err_bit.Encounter_Obstacle=0;
                    Time.B_Encounter_Obstacle_beepnum=0;
                    //SetupBzhx(LONG_BEEP,1);//取消遇阻报警声		
                }
                else//
                {
                    if(Flag.B_Encounter_Obstacle_Motor_stop==1)//遇阻停止后的操作不计入轻抬上升
                    {
                        Flag.B_Encounter_Obstacle_Motor_stop=0;		
                    }
                    else 
                    {
                        if(ENCOUNTER_Obstacle_TIME3<Time.B_Motor_Stop_Encounter_Obstacle_time)//小于3s大于200ms轻抬上升
                        {
                            if((!Flag.B_Up_limit)&&(!(Ell_Data.B_Motor_Current_Position<B_set_up_limit_value)))//如果没到上限位和电子上限位
                            {
															 
																	Device_State_Data.B_Motor_State_Byte=MOTOR_UP_STATE;//电机状态
																	Flag.B_Motor_raise_up=1;
																	Time.B_Motor_Run_time=0;
																	Holl_pulse_temp_B=Ell_Data.B_Motor_Current_Position;
                            }
                        }
                    }
                }
                Time.B_Motor_Stop_Encounter_Obstacle_time=0; 
            } 
        } 
    }
    
    switch(current_state_B)//电机超重及堵转
    {
        case 0:
        {	
            if(Motor_Current_B.ADvalue>MOTOR_ERR_CURRENT_ADVALUE)//大于堵转电流
            {
                Time.B_Judge_Motor_Err_time++;
                if(Time.B_Judge_Motor_Err_time>MOTOR_STALL_TIME)//100ms
                {
                    Time.B_Judge_Motor_Err_time=0;    
                    Flag.B_beep_off_5stime_flag=1;
                    Time.B_beep_off_5stime=0;
                    Time.B_Motor_Err_beepnum=0;
                    Time.Up_Data_Delay_time=1000;//延时1s上报故障
                    Err_B.err_bit.Motor_Err=1;
                    motor_stop_B();//电机停止
                    Device_State_Data.B_Motor_State_Byte=MOTOR_SUSPEND_STATE;//停止	
                    Flag.B_No_judge_electronic_limit=0;
                }
            }
            else
            {	
                if(Motor_Current_B.ADvalue<(MOTOR_ERR_CURRENT_ADVALUE-MOTOR_ERR_CURRENT_ADVALUE_OFFSET))//大于堵转电流
                {
                    Time.B_Judge_Motor_Err_time=0;
                }
                
                if(Flag.B_Motor_run_now)
                {
                    if(Time.B_Motor_Run_time<200)//继电器给信号后延时2s
                    {
                        Time.B_Motor_Run_time++;
                        if(Time.B_Motor_Run_time==95)
                        {
                            Time.B_Test_speed_time=0;
                            Holl_test_speed_temp_B=Ell_Data.B_Motor_Current_Position;
                        }
                        else if(Time.B_Motor_Run_time==195)//得到速度值，单位1%/100ms行程
                        {
                            if(Ell_Data.B_Motor_Current_Position>Holl_test_speed_temp_B)
                            {
                                Device_State_Data.B_current_speed=(Ell_Data.B_Motor_Current_Position-Holl_test_speed_temp_B)/2.5;
                            }
                            else if(Ell_Data.B_Motor_Current_Position<Holl_test_speed_temp_B)
                            {
                                Device_State_Data.B_current_speed=(Holl_test_speed_temp_B-Ell_Data.B_Motor_Current_Position)/2.5;
                            }
                        }
                        B_Motor_Current_Position_pulse=10000;
                        Holl_pulse_temp_B=10000;
                        Time.B_Motor_Err_time=0;
                    }
                    else
                    {
                        if(Time.B_Motor_Err_time++>100)//1s检测一次霍尔数值
                        {
                            Time.B_Motor_Err_time=0;
                            if(((Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STATE)&&(B_Motor_Current_Position_pulse>=Holl_pulse_temp_B))//上升脉冲在变大或不变
                                ||((Device_State_Data.B_Motor_State_Byte==MOTOR_DOWN_STATE)&&(B_Motor_Current_Position_pulse<=Holl_pulse_temp_B)))//下降脉冲在变小或不变
                            {
															if(Ell_Data.B_Motor_Current_Position<(UP_POSITION_VALUE+200)&&(Ell_Data.B_Motor_Current_Position>1000)&&Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STATE)
															{
																current_state_B=1;
																Fold_restoreB=1;
															}else
															{
															  Flag.B_beep_off_5stime_flag=1;
                                Time.B_beep_off_5stime=0;
                                Time.B_Motor_Err_beepnum=0;
                                Err_B.err_bit.Motor_Err=1;
                                Time.B_Up_Data_Delay_time=1000;//延时1s上报故障，防止app在更新按键状态时清除故障信息
                                motor_stop_B();//电机停止
                                Flag.B_No_judge_electronic_limit=0;
                                Device_State_Data.B_Motor_State_Byte=MOTOR_SUSPEND_STATE;//暂停	
															}
 
                            }	
                            else
                            {
                                Holl_pulse_temp_B=B_Motor_Current_Position_pulse;
                                Err_B.err_bit.Motor_Err=0;							
                            }
                        }	
                    }
                }
                else
                {
                    Device_State_Data.B_current_speed=40;
                }
                
                if((Flag.Enable_Test_Over_Wight_flag)&&(!Flag.B_Overweight_current_correct_start))
                {
                    
 
									
                }
                else if((Flag.B_Enable_Test_Over_Wight_flag)&&(!Flag.B_Overweight_current_correct_start))//允许检测超重
                {

									#ifdef	LIMIT_OVER_VALVE
										if(Ell_Data.B_Motor_Current_Position<(B_set_up_limit_value)&&Ell_Data.B_Motor_Current_Position>(UP_POSITION_VALUE+240))
										{
		
											 if(Motor_Current_B.ADvalue>(motor_current_to_advalue_B(Ell_Data.B_Motor_Run_Circle)-OVER_WIGHT_CURRENT_ADVALUE_LIMIT))//大于超重电流
												{
														Time.B_Judge_Over_Wight_time++;
														if(Time.B_Judge_Over_Wight_time>=MOTOR_OVER_WEIGHT_TIEM)//持续超重电流
														{
																 Time.B_Judge_Over_Wight_time=0;
																 current_state_B=2;
																// Fold_restoreB=1;
														
														}
												}
												else
												{
														if(Motor_Current_B.ADvalue<(motor_current_to_advalue_B(Ell_Data.B_Motor_Run_Circle)-OVER_WIGHT_CURRENT_ADVALUE_LIMIT-OVER_WIGHT_CURRENT_ADVALUE_OFFSET))//电流波动超出超重判断回滞值
														{
																Time.B_Judge_Over_Wight_time=0;
																Err_B.err_bit.Over_Wight=0;
														}
												}
		 
										}else
										{
													if(Motor_Current_B.ADvalue>motor_current_to_advalue_B(Ell_Data.B_Motor_Run_Circle))//大于超重电流
														{
																Time.B_Judge_Over_Wight_time++;
																if(Time.B_Judge_Over_Wight_time>=MOTOR_OVER_WEIGHT_TIEM)//持续超重电流
																{
																		Time.B_Judge_Over_Wight_time=0;
																		if(Ell_Data.B_Motor_Current_Position<(UP_POSITION_VALUE+220))
																		{
																		 current_state_B=1;
																		 Fold_restoreB=1;
																		}else
																		{
																			current_state_B=2;	
																		}
																}
														}
														else
														{
																if(Motor_Current_B.ADvalue<(motor_current_to_advalue_B(Ell_Data.B_Motor_Run_Circle)-OVER_WIGHT_CURRENT_ADVALUE_OFFSET))//电流波动超出超重判断回滞值
																{
																		Time.B_Judge_Over_Wight_time=0;
																		Err_B.err_bit.Over_Wight=0;
																}
														}
 
										}
 
								 	#else  
										
								  if(Motor_Current_B.ADvalue>motor_current_to_advalue_B(Ell_Data.B_Motor_Run_Circle))//大于超重电流
                    {
                        Time.B_Judge_Over_Wight_time++;
                        if(Time.B_Judge_Over_Wight_time>=MOTOR_OVER_WEIGHT_TIEM)//持续超重电流
                        {
                            Time.B_Judge_Over_Wight_time=0;
														if(Ell_Data.B_Motor_Current_Position<(UP_POSITION_VALUE+220))
														{
															current_state_B=1;//到二级限位
														   Fold_restoreB=1;
														}else
														{
														  current_state_B=2;	
														}
                        }
                    }
                    else
                    {
                        if(Motor_Current_B.ADvalue<(motor_current_to_advalue_B(Ell_Data.B_Motor_Run_Circle)-OVER_WIGHT_CURRENT_ADVALUE_OFFSET))//电流波动超出超重判断回滞值
                        {
                            Time.B_Judge_Over_Wight_time=0;
                            Err_B.err_bit.Over_Wight=0;
                        }
                    }
									#endif
                }
                
                if((Flag.B_Enable_Test_Motor_Obs_flag)&&(Device_State_Data.B_Motor_Position>1))//允许检测向上遇到阻碍
                {	
									
                    sum += Motor_Current_B.ADvalue;
                    _10ms_step++;	
                    if(_10ms_step>=AD_AVERAGE_NUM)//100ms
                    {
                        _10ms_step=0;
                        if(ADC_sample_num_B<AD_BUFF_DIP)
                        {
                            ADC_sample_num_B++;
                        }
                        ADC_read_buf_B[ADC_sample_num_B-1]=sum/AD_AVERAGE_NUM;
                        sum=0;
                        if(ADC_sample_num_B==AD_BUFF_DIP)
                        {		
                             ADC_sample_max_B=ADC_read_buf_B[0];
                             conform_motor_obs_num2_B=0;
												  	
                            for(i=1;i<AD_BUFF_DIP;i++)
                            { 
					                      	 // 改进峰值检测：
																		if(i < (AD_BUFF_DIP-1))
																		{
																				// 只有当下降幅度较大时才重置
																				int8_t drop1 = ADC_read_buf_B[i] - ADC_read_buf_B[i-1];
																				int8_t drop2 = ADC_read_buf_B[i+1] - ADC_read_buf_B[i];
																				if(drop1 < -15 && drop2 < -15)  // 下降超过3才认为是中断
																				{
																						conform_motor_obs_num2_B = 0;
																				}
																		}															
																				// 峰值检测条件：当前值大于历史最大值，并且比前一个值大7以上
																		if(ADC_read_buf_B[i] > ADC_sample_max_B && 
																			 ADC_read_buf_B[i] - ADC_read_buf_B[i - 1] >= AD_DIFF_NUM)
																		{
																				ADC_sample_max_B = ADC_read_buf_B[i];
																				conform_motor_obs_num2_B++;	
																		}
                            }
														/* for(i = 1; i < AD_BUFF_DIP; i++)
															{						
																	// 改进峰值检测：
																	if(i < (AD_BUFF_DIP - 1))
																	{
																			// 只有当下降幅度较大时才重置
																			int8_t drop1 = ADC_read_buf[i] - ADC_read_buf[i - 1];
																			int8_t drop2 = ADC_read_buf[i + 1] - ADC_read_buf[i];
																			if(drop1 < -5 && drop2 < -5)  // 下降超过5才认为是中断
																			{
																					conform_motor_obs_num = 0;
																			}
																	}
																	
																	// 峰值检测条件：当前值大于历史最大值，并且比前一个值大10以上
																	if(ADC_read_buf[i] > ADC_sample_max && 
																		 ADC_read_buf[i] - ADC_read_buf[i - 1] >= 10)
																	{
																			ADC_sample_max = ADC_read_buf[i];
																			conform_motor_obs_num++;	
																	}
															}
															*/
                            for(i=0;i<AD_BUFF_DIP-1;i++)
                            {
                                ADC_read_buf_B[i]=ADC_read_buf_B[i+1];	
                            }
														
																if( Ell_Data.B_Motor_Current_Position<(UP_POSITION_VALUE+220) )
																{
																		
																			if(conform_motor_obs_num2_B>=(UPTREND_NUM+3))//判定为向上遇到障碍
																			{
																				Fold_restoreB=1;
																				conform_motor_obs_num2_B=0;
																				current_state_B=1;
																			}
																}
															else if(conform_motor_obs_num2_B>=(UPTREND_NUM+3))//判定为向上遇到障碍
																{
																		 if( Ell_Data.B_Motor_Current_Position<(B_set_up_limit_value) )
																		  {
																				Fold_restoreB=1;
																		  }
																			conform_motor_obs_num2_B=0;
																			current_state_B=1;
																}
                        }
                    }
                }									
                else
                {
                    _10ms_step=0;
                }
            }
        }
        break;
        
        case 1:
        {
          
            Flag.B_beep_off_5stime_flag=1;
            Time.B_beep_off_5stime=0;
            Time.B_Encounter_Obstacle_beepnum=0;
            Err_B.err_bit.Motor_Obs=1; 
            Up_State_Data.Err_Byte&=0xf7;//重新上报上升遇阻
            Time.B_Motor_Obs_Down_time=0;	 
           
						if((Fold_restoreB==1|| Device_State_Data.B_Motor_Position<(Device_State_Data.B_electronic_up_limit)))
						{
							Device_State_Data.B_set_position=Device_State_Data.B_electronic_up_limit;
							Run_to_position_B();
							Fold_restoreB=0;
						}else
						{
						
							Flag.B_Run_to_set_position_flag=0;
							Device_State_Data.B_Motor_State_Byte=MOTOR_DOWN_STATE;//下降	         
							Flag.B_Motor_up_to_down=1;	
							Time.B_Motor_Dir_Change_Delaytime=500;//延时500ms换向
						}
						 current_state_B=4;
        }
        break;
        
        case 2:
        {
            Flag.B_beep_off_5stime_flag=1;
            Time.B_beep_off_5stime=0;
            Time.B_Over_Wight_beepnum=0;
            Err_B.err_bit.Over_Wight=1;	
            Up_State_Data.B_Err_Byte&=0xfd;//重新上报超重
            Device_State_Data.B_Motor_State_Byte=MOTOR_DOWN_STATE;//下降
            Flag.B_Motor_up_to_down=1;//	
            Time.Motor_Dir_Change_Delaytime=10;//换向时间500ms
            current_state_B=3;      
        }
        break;
        
        case 3:
        {
            if((Device_State_Data.B_Motor_State_Byte==MOTOR_DOWN_STOP_STATE)
                ||(Device_State_Data.B_Motor_State_Byte==MOTOR_SUSPEND_STATE))//超重后需要到下限位或暂停
            {
                current_state_B=0;
            }
        }
        break;
        
        case 4:
        {	
					if(Device_State_Data.B_Motor_Position>=Device_State_Data.B_electronic_up_limit)
					 {
					 			 if(Time.B_Motor_Obs_Down_time++>300)
								{
										Device_State_Data.B_Motor_State_Byte=MOTOR_SUSPEND_STATE;
										Time.B_Motor_Obs_Down_time=0;	
								}
					 }
      /*    	
					 }
			
				 if(Ell_Data.B_Motor_Current_Position>(UP_POSITION_VALUE+220))
						{
								 Time.B_Motor_Stop_Encounter_Obstacle_time=0;//不做遇阻判断，判断时间清0
								if(Time.B_Motor_Obs_Down_time++>300)
								{
										Device_State_Data.B_Motor_State_Byte=MOTOR_SUSPEND_STATE;
										Time.B_Motor_Obs_Down_time=0;	
								}
						} 
			*/
            if((Device_State_Data.B_Motor_State_Byte==MOTOR_DOWN_STOP_STATE)
                ||(Device_State_Data.B_Motor_State_Byte==MOTOR_SUSPEND_STATE))//暂停
            {
                current_state_B=0;
                Err_B.err_bit.Motor_Obs=0; //
            }
        }
        break;
        
        case 5:
        {
            Time.B_Motor_Stop_Encounter_Obstacle_time=0;//不做遇阻判断，判断时间清0	
            if(Time.B_Motor_Force_Down_time++>100)
            {	
                if((Flag.Yz_B)||(Device_State_Data.B_Motor_Position==100))
                {
                    Device_State_Data.B_Motor_State_Byte=MOTOR_SUSPEND_STATE;
                }
                Time.B_Motor_Force_Down_time=0;	
                current_state_B=0;
                Flag.B_Motor_force_down=0;	
            }		
            if((Device_State_Data.B_Motor_State_Byte==MOTOR_SUSPEND_STATE)||(Device_State_Data.B_Motor_State_Byte==MOTOR_UP_STATE))
            {
                Time.B_Motor_Force_Down_time=0;	
                current_state_B=0;
                Flag.B_Motor_force_down=0;				
            }
        }
        break;
        
        default:
            break;	    
    }
    
    if(Flag.B_beep_off_5stime_flag)//报警提示音循环时间到
    {
        Flag.B_beep_off_5stime_flag=0;
        if((Err_B.err_bit.Encounter_Obstacle)||(Err_B.err_bit.Motor_Obs))
        {
            if((++Time.B_Encounter_Obstacle_beepnum)<=ALARM_YZ_BEEP_NUM)
            {    
                /*if((Time.B_Encounter_Obstacle_beepnum==1)&&(Flag.Beep_open_now==1))//
                {
                    Time.B_Encounter_Obstacle_beepnum=ALARM_YZ_BEEP_NUM;	
                }
                else
                {
                    SetupBzhx(YZ_ALARM_BEEP,1);
                }*/
                SetupBzhx(YZ_ALARM_BEEP,1);
            }
            else
            {
                Time.B_Encounter_Obstacle_beepnum=ALARM_YZ_BEEP_NUM;
            }
        }	
        else if(Err_B.err_bit.Motor_Err)
        {
            if((++Time.B_Motor_Err_beepnum)<=ALARM_MOTOR_BEEP_NUM)
            {	
                /*if((Time.B_Motor_Err_beepnum==1)&&(Flag.Beep_open_now==1))//
                {
                    Time.B_Motor_Err_beepnum=ALARM_MOTOR_BEEP_NUM;	
                }
                else
                {
                    SetupBzhx(MOTOR_ERR_ALARM_BEEP,1);
                }*/
                SetupBzhx(MOTOR_ERR_ALARM_BEEP,1);
            }
            else
            {
                Time.B_Motor_Err_beepnum=ALARM_MOTOR_BEEP_NUM;
            }
        }
        else if(Err_B.err_bit.Over_Wight)
        {
            if((++Time.B_Over_Wight_beepnum)<=ALARM_OVER_WEIGHT_BEEP_NUM)
            {	    
                /*if((Time.B_Over_Wight_beepnum==1)&&(Flag.Beep_open_now==1))//
                {
                    Time.B_Over_Wight_beepnum=ALARM_OVER_WEIGHT_BEEP_NUM;	
                }
                else
                {
                    SetupBzhx(OVER_WIGHT_ALARM_BEEP,1);
                }*/
                SetupBzhx(OVER_WIGHT_ALARM_BEEP,1);
            }
            else
            {
                Time.B_Over_Wight_beepnum=ALARM_OVER_WEIGHT_BEEP_NUM;
            }
        }
    }
    
    Device_State_Data.B_Err_Byte=Err_B.data;
    // 当B杆Motor_Err故障延时上报激活时，预同步上报状态中的Motor_Err位，
    // 避免properties_up()立即上报6 1 3，1s后由定时器ISR清除该位触发上报
    if(Err_B.err_bit.Motor_Err && Time.B_Up_Data_Delay_time > 0)
    {
        Up_State_Data.B_Err_Byte |= 0x04;
    }
}











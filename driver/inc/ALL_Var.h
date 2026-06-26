#ifndef __ALL_VAr_H__
#define __ALL_VAr_H__
#include "adc.h"
#include "bgr.h"
#include "lvd.h"
#include "gpio.h"
#include "ddl.h"
#include "sysctrl.h"
#include "lvd.h"
#include "bt.h"
#include "timer3.h"
#include "trim.h"
#include "uart.h"
#include "adt.h"
#include "flash.h"
#include "wdt.h"
#include "head_define.h"
#include "head_function.h"
#include "lpuart.h"
#include "lpm.h"
#include "adc.h"

typedef struct
{
	uint8_t  Capture_433key;	
	uint8_t  Capture_433key_State;
	uint16_t Capture_433key_Judgetime;//判断松手时间
	uint16_t Capture_433key_Cmd;
	
	uint8_t  Capture_State;//状态
	uint8_t  Capture_Pulse_Wide_Sequence;//脉宽序列
	uint16_t Capture_Pulse_Wide_value[140];//捕获脉宽变量
	uint16_t Command_data;//8位命令码
	uint32_t Identify_data;//24位识别码加8位命令码
	uint32_t Secret_Key_data;//32位秘钥
}CAPTURE_COMMAND;
extern volatile CAPTURE_COMMAND CAPTURE_COMMAND1;

typedef enum {
    PRESS_DOWN = 0,
    PRESS_UP,
    PRESS_REPEAT,
    SINGLE_CLICK,
    DOUBLE_CLICK,
    THREE_CLICK,
    FORE_CLICK,       // 注意：可能是拼写错误，应为FOUR_CLICK？
    FIVE_CLICK,
    SIX_CLICK,
    SEVEN_CLICK,
    LONG_RRESS_START,  // 注意：可能是拼写错误，应为LONG_PRESS_START？
    LONG_PRESS_HOLD,
    LONG_PRESS_UP,
    NONE_PRESS
} PressEvent;

extern PressEvent PressEven;  // 声明PressEvent类型的变量
// 结构体类型定义
typedef struct {
    uint16_t ticks;
    uint8_t  repeat       : 4;
    uint8_t  event        : 4;
    uint8_t  state        : 4;
    uint8_t  debounce_cnt : 4; 
    uint8_t  button_level;
  uint8_t  (*hal_button_Level)(void);
} button;

typedef struct
{  
    uint8_t Encounter_Obstacle_beepnum ;//蜂鸣器遇阻报警声次数
    uint8_t B_Encounter_Obstacle_beepnum ;//蜂鸣器遇阻报警声次数
    uint8_t Over_Wight_beepnum         ;//蜂鸣器超重报警声次数
    uint8_t B_Over_Wight_beepnum         ;//蜂鸣器超重报警声次数
    uint8_t Motor_Err_beepnum          ;//电机故障报警声次数
    uint8_t B_Motor_Err_beepnum          ;//电机故障报警声次数
    
    uint8_t Holl_Pluse_Filter_time     ;//霍尔脉冲滤波时间
    uint8_t B_Holl_Pluse_Filter_time     ;//霍尔脉冲滤波时间
    uint8_t return_to_up_time          ;//到达机械上限位滤波时间
    uint8_t B_return_to_up_time          ;//到达机械上限位滤波时间
    uint8_t return_to_down_time	       ;//到达机械下限位滤波时间  
    uint8_t B_return_to_down_time      ;//到达机械下限位滤波时间      
    
    uint8_t Enter_Pcba_test_time       ;//进入PCB自检滤波时间
    uint8_t Uart_Receive_Interval_time ;//串口接收数据包间隔
    uint8_t _150ms_Count;//150ms时基
    uint8_t Zero_crossing_no_single_time;
    
    uint8_t Motor_Err_time;//有驱动继电器，电机无电流
    uint8_t B_Motor_Err_time;//有驱动继电器，电机无电流
    
    uint8_t time5ms;
    uint8_t time10ms;
    uint8_t time100ms;
 
    uint16_t time150ms;
    uint16_t beep_off_5stime;//蜂鸣器报警重复间隔时间
    uint16_t B_beep_off_5stime;//蜂鸣器报警重复间隔时间
    uint16_t power_on_10stime;//上电10s
    uint16_t Light_key_space_time;//照明按键间隔
		
		
    uint16_t time_5min;
    uint16_t  Test_speed_time;
    uint16_t  B_Test_speed_time;
    
    uint16_t Motor_Dir_Change_Delaytime;//电机换向停止延时
    uint16_t B_Motor_Dir_Change_Delaytime;//电机换向停止延时
    
    uint16_t Motor_Stop_Encounter_Obstacle_time;//电机停止时遇阻时间
    uint16_t B_Motor_Stop_Encounter_Obstacle_time;//电机停止时遇阻时间
    
    uint8_t  Motor_No_Encounter_Obstacle_time;//非遇阻时间
    uint8_t  B_Motor_No_Encounter_Obstacle_time;//非遇阻时间
    
    uint16_t Enable_Add_Match_Code_time;//添码操作有效时间,5s有效
    
    uint16_t Judge_Over_Wight_time;//超重判断滤波时间
    uint16_t A_Judge_Over_Wight_time;//超重判断滤波时间
    uint16_t B_Judge_Over_Wight_time; 
    
    uint16_t Judge_Motor_Err_time;//电机堵转判断滤波时间
    uint16_t B_Judge_Motor_Err_time;//电机堵转判断滤波时间
    
    uint16_t Motor_Start_Up_time;//电机同时开始上升计时
    uint16_t A_Motor_Start_Up_time;//电机开始上升计时
    uint16_t B_Motor_Start_Up_time;//电机开始上升计时
    
    uint16_t Motor_Run_time;//I/O启动到实际电机得电，当前设为1s保证继电器吸合
    uint16_t B_Motor_Run_time;//I/O启动到实际电机得电，当前设为1s保证继电器吸合
    
    uint16_t Up_Data_Delay_time;//电机故障上报停止后间隔再报电机故障
    uint16_t B_Up_Data_Delay_time;//电机故障上报停止后间隔再报电机故障
    
    uint32_t WIFI_enable_config_time;//模组允许配网时间，
    
    uint16_t Overweight_Current_time;//校正程序运行时间
    uint16_t B_Overweight_Current_time;//校正程序运行时间
    
    uint16_t  Motor_Obs_Down_time;//上升遇到阻碍后下降时间
    uint16_t  B_Motor_Obs_Down_time;//上升遇到阻碍后下降时间 
    
    uint16_t  Motor_Force_Down_time;//强制下降时间
    uint16_t  B_Motor_Force_Down_time;//强制下降时间
    
    uint32_t  motor_debug_time;
    
		uint32_t  key_convergent;//强制下降时间

		uint16_t  Up_Limit_Set_time;//上限位触发按下降，下降时间定时
		
		uint16_t  Up_B_Limit_Set_time;
		
		uint16_t  Del_Match_Code_time;
		
}TIME_SLICE;
extern volatile TIME_SLICE  Time;

typedef struct
{
    uint8_t   Function;
    uint16_t  ADvalue;
    uint16_t  AD_sumple_num; 
    uint32_t  AD_sum;
}FUNCTION_SELECT;

extern volatile FUNCTION_SELECT  Function_select,Motor_Current,Motor_Current_B;

typedef struct
{
    uint8_t Power_Failure_flag                :1; //检测到断电标志位

    uint8_t AD0_Change_end                    :1;//AD0检测完毕
    uint8_t AD19_Change_start                 :1;//AD检测通道切换到AD19
    
    uint8_t power_on_10stime_flag             :1;
    uint8_t beep_off_5stime_flag              :1;
    uint8_t B_beep_off_5stime_flag            :1;
    uint8_t time5ms_flag                      :1;
    uint8_t time10ms_flag                     :1;
    uint8_t time100ms_flag                    :1;
    uint8_t time150ms_flag                    :1;   
     
    uint8_t Motor_run_now                     :1;//电机已通电
    uint8_t B_Motor_run_now                   :1;//电机已通电
    uint8_t Motor_up_to_down                  :1;//上升换成下降
    uint8_t B_Motor_up_to_down                :1;//上升换成下降
    uint8_t Motor_down_to_up                  :1;//下降换成上升
    uint8_t B_Motor_down_to_up                :1;//下降换成上升
    uint8_t Motor_raise_up                    :1;//轻抬上升
    uint8_t B_Motor_raise_up                  :1;//轻抬上升
    
    uint8_t Encounter_Obstacle_Motor_stop     :1;//遇阻停止
    uint8_t B_Encounter_Obstacle_Motor_stop   :1;//遇阻停止
    
    uint8_t Add_Match_Code_flag               :1;//添加新遥控器码
    
    uint8_t Motor_all_up    	                :1;//电机同时上升
    uint8_t A_Motor_Start_up    	            :1;//电机开始上升
    uint8_t B_Motor_Start_up  	              :1;//电机开始上升
    
    uint8_t Enable_Test_Over_Wight_flag       :1;//允许整机检测超重 
    uint8_t A_Enable_Test_Over_Wight_flag     :1;//允许检测超重  
    uint8_t B_Enable_Test_Over_Wight_flag     :1;//允许检测超重 
    
    uint8_t Enable_Test_Motor_Obs_flag        :1;//允许检测上升遇阻碍
    uint8_t B_Enable_Test_Motor_Obs_flag      :1;//允许检测上升遇阻碍
    
    uint8_t Enter_Pcba_test_flag              :1;// 进入Pcba自检 
    
    uint8_t _220AC_Power_Down_flag            :1;//掉电标志
    
    uint8_t Already_save_data_flag            :1;//掉电已经保存一次数据
    
    uint8_t No_judge_electronic_limit         :1;//不判断电子限位
    uint8_t B_No_judge_electronic_limit	      :1;//不判断电子限位
    
    uint8_t Brightness_time_flag              :1;//夜灯时间
    
    uint8_t Brightness_change                 :1;//夜灯状态下调节了亮度
    
    uint8_t Run_to_set_position_flag          :1;//运行到指定位置
    uint8_t B_Run_to_set_position_flag        :1;//运行到指定位置
    
    uint8_t Up_event_finalposition            :1;//需要上报最终位置事件
    
    uint8_t Auto_light                        :1;//亮度自动化
    
    uint8_t Reset_model                       :1;//防止遥控器配网重置蜂鸣器响应后，收到模组状态改变命令再响一次
    
    uint8_t Need_433_data_compensate          :1;//433数据需要补偿
    
    uint8_t Overweight_current_correct_start  :1;//开始校正程序
    uint8_t B_Overweight_current_correct_start  :1;//开始校正程序
    
    uint8_t Motor_force_down                  :1;//强制下降
    uint8_t B_Motor_force_down                :1;//强制下降
    
    uint8_t Uart1_Err                         :1;//接收wifi数据出错
    
    uint8_t Low_power    		                  :1;
    uint8_t Beep_open_now                     :1;
    
    uint8_t A_Up_limit                          :1;
    uint8_t B_Up_limit                          :1;
    uint8_t A_Down_limit                        :1;
    uint8_t B_Down_limit                        :1;
    uint8_t Yz_A                                :1;
    uint8_t Yz_B                                :1;
		uint8_t led_state_cnt                           ;   
		uint8_t led_state                           :4;  //灯光状态

		uint8_t ELE_LIM                              :1; 
		uint8_t B_ELE_LIM                            :1; 
		uint8_t ELE_LIM_DOWN                         :1; 
		uint8_t B_ELE_LIM_DOWN                       :1;
		uint8_t Lighting_setup                       :1;  //色温或者亮度在设置过程中
		uint8_t mcu_wifi_mode                        :1;  //色温或者亮度在设置过程中

}FLAG;
// 全局标志
extern volatile FLAG Flag;
 
typedef union 
{
    uint8_t data;
    struct
    {
		uint8_t  Encounter_Obstacle     :1;//向上遇阻
		uint8_t  Over_Wight             :1;
		uint8_t  Motor_Err              :1;
		uint8_t  Motor_Obs		          :1;
    }err_bit;
}ERR;
extern volatile  ERR Err,Err_B;

typedef struct
{	
//siid=2
    uint8_t Err_Byte;//piid=1故障插件属性:读、上报;Value 0: 正常 1: 遇阻 2: 超重 4: 电机故障                                  
    uint8_t Motor_State_Byte;//piid=2电机状态插件属性:读、上报 ;Value 0: 上限位停止 1: 上升 2: 下降 3: 暂停 4: 下限位停止                                              
    				//piid=5电机控制插件 写属性                                                                                           
    uint8_t Motor_Position;//piid=4晾杆当前位置插件属性：读、上报,(0-100%)                                                
    uint8_t set_position;//piid=5 运行到指定位置，插件属性：读、写、上报   
    uint8_t electronic_up_limit;//piid=6 电子上限位插件属性：读、写、上报 value 0-49%                                                  
    uint8_t electronic_down_limit;//piid=7 电子下限位插件属性：读、写、上报 value 51-100%  
    
    uint8_t convergent;//bool piid=8 一键收合     
    uint16_t current_speed;//piid=9  读 上报，1-500对应0.1-50  实际是Float       
    uint8_t preset_position;//piid=10 一键晾晒预设位置     
                                                                                                                                        
//siid=3      
    uint8_t Light_State_Byte;//bool piid=1 照明开关插件属性：读、写、上报                                                                                                      
    uint8_t Light_Bright;//piid=2 照明亮度插件属性：读、写、上报 value（0-100%）                                                                                 
    uint32_t Light_colour_temperature;//piid=4 色温  3000-6500  ，步长5     
    uint8_t nightlight_switch; //piid=4   夜灯开关
    uint8_t brightness;//piid=5     夜灯亮度插件属性新增
//siid=5, 
    uint8_t Beep_Enable;//bool piid=1蜂鸣器提示音开关插件属性：读、写、上报 value：0：关闭 1：开启                            
//siid=6
    uint8_t B_Err_Byte;//B杆故障
    //piid=2,电机控制
    uint8_t B_Motor_State_Byte;//piid=3，B杆工作状态 
  
    uint8_t B_Motor_Position;  
    uint8_t B_set_position;   
    uint8_t B_electronic_down_limit;
    uint8_t B_electronic_up_limit;
  
    uint8_t B_preset_position;
    uint8_t B_convergent;
    uint16_t B_current_speed;
   //siid=7
    uint8_t Err_Byte_all;//双杆故障	
    //piid=2,双杆控制
    uint8_t convergent_all;//双杆收合piid=16,bool
    uint8_t set_position_all;  
   // uint8_t optimal_dry;//最佳晾晒 piid=17，bool
    uint8_t lightmode;
		//夜灯起始时间
    uint8_t start_time_hour;//piid=9 夜灯开启时间小时插件属性 
    uint8_t start_time_min;//piid=9  夜灯开启时间分钟插件属性  
    uint8_t end_time_hour;//piid=9   夜灯关闭时间小时插件属性 
    uint8_t end_time_min; //piid=9  夜灯关闭时间分钟插件属性 
		
		uint8_t  Control_open;
    uint8_t  Control_num;
}DEVICE_STATE_DATA;//与app通信数据段数据

extern  DEVICE_STATE_DATA   Device_State_Data,Up_State_Data;//设备上报状态
extern volatile uint8_t  FDL_Block0_Save_Times,FDL_Block1_Save_Times;//flash_data存储次数，
typedef struct
{
    uint8_t    Max_Light_Value    ;//日光亮度 
    uint8_t    Light_Bright_Value :7;//夜光亮度 0：日光  1：夜光
    uint8_t    Light_Bright_Enable:1;//开启夜灯，标插不用记忆
		uint8_t    Quite_updata         :1;//自动升级后复位无声音，0有效     
		uint8_t    Beep_open            :1;//提示音开关，1：开
	
		uint8_t    Control_open         :1;//提示音开关，1：开
		uint8_t    lightmode            :3;//预留3位
	
	  uint8_t    Light_open_hour_time :6;//夜灯开启小时 
    uint8_t    Device_light_state   :2;//升级前照明状态
	
    uint8_t    Light_open_min_time  :6;//夜灯开启分钟
    uint8_t    Device_updata        :2;//升级后复位，0有效
	
    uint8_t    Light_close_hour_time:6;//夜灯关闭小时
    uint8_t    Device_motor_state   :2;//升级前电机A状态
	
    uint8_t    Light_close_min_time :6;//夜灯关闭分钟
	  uint8_t    B_Device_motor_state :2;//升级前电机B状态  


    uint8_t   Electronic_Up_Limit   ;//电子上限位，百分比   
    uint8_t   B_electronic_up_limit   ;//电子上限位，百分比
    
    uint8_t   Electronic_Down_Limit;//电子下限位	,百分比
    uint8_t   B_electronic_down_limit;//电子下限位,百分比
    
    uint8_t    Remote_Control_Num   ;//存储遥控器个数
    uint8_t    Light_pwm_value;      //照明亮度百分比，为了记忆是否更新程序之前是否是夜灯亮度    
    uint16_t   Light_CT_value;      //照明色温值
    
    uint8_t   Overweight_Correct;//125对应0，126对应默认值增加0.01A，124对应默认值减少0.01A，类推
    uint8_t   B_Overweight_Correct;//125对应0，126对应默认值增加0.01A，124对应默认值减少0.01A，类推
 
    
    uint32_t  Remote_Control_Key[3];//遥控器密钥
    uint32_t  Remote_Control_Id[3];//遥控器Id码
    uint32_t  reserve;//预留1个字节
    uint16_t  Check;
}FDL_DATA; //56
extern volatile FDL_DATA  Flash_Data;
extern uint16_t  Synchronous_Count_value[3];//同步计数值

typedef struct
{
    uint16_t  Motor_Current_Position;//电机当前位置对应脉冲数，
    uint16_t  Motor_Run_Circle;//电机循环运行次数，10次为单位
    
    uint16_t  B_Motor_Current_Position;//电机当前位置对应脉冲数，
    uint16_t  B_Motor_Run_Circle;//电机循环运行次数，10次为单位
}ELL_DATA;
extern volatile ELL_DATA  Ell_Data;

typedef struct
{
  uint8_t  Light_Led_state;
	uint8_t  Light_Breathing_time;
	uint8_t  Led_Blue_state          :2;//蓝灯显示状态
}LED;
extern  LED Led ;

typedef struct
{
	uint8_t rf433_test_flag: 	              1;
	uint8_t input_YZ_test_flag:             1;
	uint8_t input_uplimit_test_flag:        1;
	uint8_t input_downlimit_test_flag:      1;
	uint8_t wifi_test_flag:                 1;	
}PCBA_TEST;
extern  PCBA_TEST Pcba_test;

typedef struct//超重值校正
{
	uint16_t Sample_num;//采样次数
	uint32_t Sample_sum;//采样值求和
	float Sample_ave;//平均AD值
	uint16_t sample_current;	
}Overweight_correct;
extern  Overweight_correct Overweight_Correct,B_Overweight_Correct;

////============= BUZ Frequency ================
////2.3  2.6  2.9  Fpclk 24M
//#define   FREQ_NO     0
//#define   FREQ_2300   5217//3478//6957
//#define   FREQ_2600   4617//3077//6154
//#define   FREQ_2900   4140//2759//5517.
//                     
//#define   FREQ_2000   6000//4000//8000
//#define   FREQ_1238   9657//6437//12874   //12924
//#define   FREQ_1288   4000            //  3000//6186//12372   //12422 
//#define   FREQ_1342   8907//5937//11873   //11923
//============= BUZ Kinds =====================
typedef	struct tBUZHX
{
	uint16_t BzFre;	   //Frequency
	uint8_t  T_BzO;    //Frequency time
	uint8_t  T_BzV;    //Vcc		
}tBuzHx ;
 
//============= function call ==================

// ADC 相关变量
extern int16_t u32AdcRestult4;
extern uint8_t ADC_sample_num, min_serial_num, max_serial_num;
extern uint8_t ADC_sample_num_B, min_serial_num_B, max_serial_num_B;
extern uint8_t ADC_sample_num_yz, min_serial_num_yz, max_serial_num_yz;
extern uint8_t ADC_sample_num_yz_B, min_serial_num_yz_B, max_serial_num_yz_B;
extern uint16_t ADC_sample_max, ADC_sample_min, ADC_sample_max_yz, ADC_sample_min_yz;
extern uint16_t ADC_sample_max_B, ADC_sample_min_B, ADC_sample_max_yz_B, ADC_sample_min_yz_B;
extern uint16_t ADC_read_buf[AD_BUFF_DIP], ADC_read_buf_B[AD_BUFF_DIP];

// 按键相关变量
extern button key_set_key, capture_433key;
extern uint8_t key_value;
extern uint8_t Study_Cmd_Repeat ,STOP_CMD_Repeat ;//连续按两下添码
// 电机检测相关变量
extern uint8_t conform_motor_obs_num, conform_motor_obs_num2;
extern uint8_t conform_motor_obs_num_B, conform_motor_obs_num2_B;
extern uint8_t A_obs_correction, B_obs_correction;
//extern uint8_t UP_state_obs,UP_state_obsB ;

// 系统状态变量
extern uint8_t Self_Inspection_step;           // 自检步骤
extern volatile uint8_t Holl_Circle_num, Motor_Circle_num;
extern uint8_t Holl_Circle_num_B, Motor_Circle_num_B;
extern uint16_t Holl_pulse_temp, Holl_test_speed_temp_B;
extern volatile int B_Motor_Current_Position_pulse, Motor_Current_Position_pulse, Holl_pulse_temp_B, Holl_test_speed_temp;

// 限位设置变量
extern uint16_t set_down_limit_value, B_set_down_limit_value;  // 电子下限位霍尔数
extern uint16_t set_up_limit_value, B_set_up_limit_value;      // 电子上限位霍尔数
extern uint16_t set_position_pulse, set_position_pulse_B;      // 指定位置对应霍尔数

// 灯光控制变量
extern uint16_t set_light_value;               // 开灯时灯实时亮度
extern uint16_t brightness_target, TDR02_target_value, TDR03_target_value, brightness_temp;
extern uint16_t Light_colour_temperature_gear, colour_temperature_temp;

// 状态机和通信变量
extern uint8_t current_state, current_state_B; // 超重过程状态机
extern uint8_t Net_state;

extern uint16_t target_position;


// 通信相关变量
extern uint8_t send_busy_asr;

extern uint8_t send_busy;

// 数据缓冲区
extern uint8_t uart0_rx_buf[180];              // wifi接收串口数据
extern uint8_t uart0_tx_buf[180];              // 发送给wifi串口数据

// 消息数据
extern uint8_t Obs1[8];                        // 遇阻
extern uint8_t Obs2[8];                        // 遇阻
extern uint8_t OVER_WIGHT1[8];                 // 超重
extern uint8_t OVER_WIGHT2[8];                 // 超重
//extern uint8_t Upper_limit_tip[8];
//extern uint8_t Confirm_asr[8];

// 校验和临时变量
extern uint16_t crc;
extern float temp1;
extern float over_weight_current;
extern float addvalue_A, addvalue_B;



// 硬件自检函数声明
extern void PCBA_Self_Inspection(void);

// 遥控器相关函数声明
extern void Record_PulseWidth(uint32_t g_tau0_ch7_width, CAPTURE_COMMAND *Capture_Command);
extern uint8_t analysis_capture_command(CAPTURE_COMMAND *Capture_Command, uint8_t power_on_10stime_flag);  // 解析遥控器按键
extern uint8_t judge_remote_control(uint32_t roll_code);
extern void capture_433key_judge_answer(button* handle, uint8_t power_on_10stime_flag);

// 遥控器命令结构体
extern CAPTURE_COMMAND Capture_Command;

#endif

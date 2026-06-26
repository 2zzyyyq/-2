#ifndef __HEAD_DEFINE_H__
#define __HEAD_DEFINE_H__
/**   define定义     **/  
// #define  TESTADC
//#define  NO_PWM_MOTOR
#define  XIMI               
//#define   LIMIT_OVER_VALVE
/********************************GPIO引脚定义********************************/
#define YZ_PIN               GpioPin10
#define YZ_PORT              GpioPortA

#define YZ_B_PIN             GpioPin11
#define YZ_B_PORT            GpioPortA

#define B_HOLL_DIR_PIN       GpioPin9
#define B_HOLL_DIR_PORT      GpioPortA

#define B_HOLL_NUM_PIN       GpioPin8
#define B_HOLL_NUM_PORT      GpioPortA

#define HOLL_DIR_PIN         GpioPin14
#define HOLL_DIR_PORT        GpioPortC

#define HOLL_NUM_PIN         GpioPin15
#define HOLL_NUM_PORT        GpioPortC

#define UP_LIMIT_PIN         GpioPin6 
#define UP_LIMIT_PORT        GpioPortB 

#define DOWN_LIMIT_PIN       GpioPin7 
#define DOWN_LIMIT_PORT      GpioPortB

#define B_UP_LIMIT_PIN       GpioPin4 
#define B_UP_LIMIT_PORT      GpioPortB 

#define B_DOWN_LIMIT_PIN     GpioPin5 
#define B_DOWN_LIMIT_PORT    GpioPortB 

#define MOTOR_DN_PIN         GpioPin15
#define MOTOR_DN_PORT        GpioPortA

#define MOTOR_UP_PIN           GpioPin3 
#define MOTOR_UP_PORT          GpioPortB

#define B_MOTOR_DN_PIN         GpioPin6
#define B_MOTOR_DN_PORT        GpioPortD

#define B_MOTOR_UP_PIN         GpioPin7
#define B_MOTOR_UP_PORT        GpioPortD

#define ZERO_CROSS_PIN         GpioPin12
#define ZERO_CROSS_PORT        GpioPortB

#define PCBA_SELFTEST_PIN       GpioPin2
#define PCBA_SELFTEST_PORT      GpioPortB

/********************************输入状态读取宏********************************/
#define PCBA_SELFTEST        Gpio_GetInputIO(PCBA_SELFTEST_PORT, PCBA_SELFTEST_PIN)
#define YZ                   Gpio_GetInputIO(YZ_PORT, YZ_PIN) 
#define YZ_B                 Gpio_GetInputIO(YZ_B_PORT, YZ_B_PIN) 
#define GetIrq_B_HOLL        Gpio_GetIrqStatus(B_HOLL_NUM_PORT, B_HOLL_NUM_PIN)
#define CLEAR_B_GetIrq       Gpio_ClearIrq(B_HOLL_NUM_PORT, B_HOLL_NUM_PIN)
#define B_HOLL_DIR           Gpio_GetInputIO(B_HOLL_DIR_PORT, B_HOLL_DIR_PIN)
#define B_HOLL_NUM           Gpio_GetInputIO(B_HOLL_NUM_PORT, B_HOLL_NUM_PIN)
#define CLEAR_HOLL_GetIrq    Gpio_ClearIrq(HOLL_NUM_PORT, HOLL_NUM_PIN)
#define GetIrq_HOLL          Gpio_GetIrqStatus(HOLL_NUM_PORT, HOLL_NUM_PIN)
#define HOLL_DIR             Gpio_GetInputIO(HOLL_DIR_PORT, HOLL_DIR_PIN)
#define HOLL_NUM             Gpio_GetInputIO(HOLL_NUM_PORT, HOLL_NUM_PIN)
#define ZERO_CROSS           Gpio_GetInputIO(ZERO_CROSS_PORT, ZERO_CROSS_PIN)

#define UP_LIMIT             Gpio_GetInputIO(UP_LIMIT_PORT, UP_LIMIT_PIN)
#define DOWN_LIMIT           Gpio_GetInputIO(DOWN_LIMIT_PORT, DOWN_LIMIT_PIN)
#define B_UP_LIMIT           Gpio_GetInputIO(B_UP_LIMIT_PORT, B_UP_LIMIT_PIN)
#define B_DOWN_LIMIT         Gpio_GetInputIO(B_DOWN_LIMIT_PORT, B_DOWN_LIMIT_PIN)

/********************************输出控制宏********************************/
#define MOTOR_DN_IO(value)   ((value) ? Gpio_SetIO(MOTOR_DN_PORT, MOTOR_DN_PIN) : Gpio_ClrIO(MOTOR_DN_PORT, MOTOR_DN_PIN)) 
#define MOTOR_UP_IO(value)   ((value) ? Gpio_SetIO(MOTOR_UP_PORT, MOTOR_UP_PIN) : Gpio_ClrIO(MOTOR_UP_PORT, MOTOR_UP_PIN))
#define B_MOTOR_DN_IO(value) ((value) ? Gpio_SetIO(B_MOTOR_DN_PORT, B_MOTOR_DN_PIN) : Gpio_ClrIO(B_MOTOR_DN_PORT, B_MOTOR_DN_PIN))
#define B_MOTOR_UP_IO(value) ((value) ? Gpio_SetIO(B_MOTOR_UP_PORT, B_MOTOR_UP_PIN) : Gpio_ClrIO(B_MOTOR_UP_PORT, B_MOTOR_UP_PIN))
#define LED24V_IO(value)     ((value) ? Gpio_SetIO(GpioPortB, GpioPin13) : Gpio_ClrIO(GpioPortB, GpioPin13))
#define LED24V_IO2(value)    ((value) ? Gpio_SetIO(GpioPortC, GpioPin13) : Gpio_ClrIO(GpioPortC, GpioPin13))
/********************************LED灯光控制********************************/
#define LIGHT_WHITE_VALUE    ADTx->GCMAR
#define LIGHT_YELLOW_VALUE   M0P_TIM3_MODE23->CCR1A_f.CCR1A
// LED关灯
#define LED_LIGHT_OFF do { \
    Gpio_ClrIO(GpioPortC, GpioPin13); \
    Adt_StopCount(M0P_ADTIM6);       \
    LED24V_IO(0);                    \
    Tim3_M23_Stop();                 \
} while(0)

// LED开灯
#define LED_LIGHT_ON do { \
    Gpio_SetIO(GpioPortC, GpioPin13); \
    Adt_StartCount(M0P_ADTIM6);       \
    Tim3_M23_Run();                  \
    LED24V_IO(0);                     \
} while(0)
/********************************时间参数定义********************************/
#define TICKS_INTERVAL                       5   // ms
#define DEBOUNCE_TICKS                       3   // MAX 8
#define SHORT_TICKS                          (300 / TICKS_INTERVAL)
#define LONG_TICKS                           (10000 / TICKS_INTERVAL)  // 10s
#define QUEUE_BUF_DEEP                       256
#define RECEIVE_DATA_PACKET_INTERVAL_TIME    50  // 大于50ms判定为新数据包 步长1ms

/********************************行程参数********************************/
#define UP_POSITION_VALUE       1000
#define DOWN_POSITION_VALUE     3900
#define STROKE_TOTAL            2900     
#define UP_ELE_VALUE            1450   //15%
#define ONE_PERCENT_PULSE       (float)(STROKE_TOTAL/100)  // 百分之一对应霍尔数

#define HALL_OFFSET_COMP_VAL     20 // 霍尔偏移补偿值

#define UP_L                     15 // 二级限位百分比


/********************************AD转换参数********************************/
#define AD_BUFF_DIP             11     
#define AD_AVERAGE_NUM          10     // 值越大越难触发
#define UPTREND_NUM             4      // 1s内上升趋势次数 值越大越难触发
#define AD_DIFF_NUM             8      // AD最大值相差值
/********************************照明参数********************************/
#define LIGHT_VALUE             16
#define A_OPTIMAL_DRY_POSITION  10
#define B_OPTIMAL_DRY_POSITION  60

/********************************功能选择AD值********************************/
#define OFFSET                  5  // 判断窗口AD值
#define FUNCTION_L_D_AD         (1.0/11*255)+OFFSET      // 照明消毒
#define FUNCTION_L_F_AD         (2.0/12*255)+OFFSET      // 照明风干
#define FUNCTION_L_H_AD         (3.0/13*255)+OFFSET      // 照明烘干风干
#define FUNCTION_L_D_F_AD       (4.3/14.3*255)+OFFSET    // 照明消毒风干
#define FUNCTION_L_D_F_H_AD     (5.6/15.6*255)+OFFSET    // 照明消毒风干烘干

#define FUNCTION_L              0X00    // 照明
#define FUNCTION_L_D            0X01    // 照明消毒
#define FUNCTION_L_F            0X02    // 照明风干
#define FUNCTION_L_H            0X03    // 照明烘干风干
#define FUNCTION_L_D_F          0X04    // 照明消毒风干
#define FUNCTION_L_D_F_H        0X05    // 照明消毒风干烘干

/********************************电机保护参数********************************/
#define OVER_WIGHT_REAL_CURRENT             (1.73)  // 真实电流值,默认每根杆超重21kg  1.72 17.5kg    // 采样电流值  1.61
#define OVER_WIGHT_CURRENT                  (float)(OVER_WIGHT_REAL_CURRENT*1.0717-0.1581) 
#define MOTOR_OVER_WEIGHT_TIEM              60     // 超重判定持续时间,步长10ms
#define OVER_WIGHT_CURRENT_ADVALUE_OFFSET   8      // 进入超重电流后的回滞AD值
#define OVER_WIGHT_CURRENT_ADVALUE_LIMIT    350    // 进入二级限位超重值 527-350=177
#define MOTOR_MAX_CIRCLE                    600    // 最多存6000次电机周期

#define MOTOR_ERR_CURRENT                  (5.5) 
#define MOTOR_ERR_CURRENT_ADVALUE           (float)(MOTOR_ERR_CURRENT*0.1/1.5*4096)  // 堵转电流对应AD值 1,638.4
#define MOTOR_STALL_TIME                    10      // 堵转判定持续时间,步长10ms
#define MOTOR_ERR_CURRENT_ADVALUE_OFFSET    60      // 进入堵转判断后，取消堵转的回滞AD值

/********************************Flash存储参数********************************/
#define FDL_DATA_BLOCK1   512/sizeof(Flash_Data)  
#define FDL_DATA_BLOCK2   512/sizeof(Ell_Data) 
#define FDL_ONCE_MAX_WRITE_NUM             FDL_DATA_BLOCK1      // block0一块flash存储次数
#define FDL_BLOCK1_ONCE_MAX_WRITE_NUM      FDL_DATA_BLOCK2      // block1一块flash存储次数
#define FDL_Block0                         0XFC00  // FLASH_DATE
#define FDL_Block1                         0XFE00  // ELL_DATE
/********************************时间常量定义********************************/
#define ZERO_CROSSING_NO_SINGL_TIME         50     // 判定无过零信号时间 步长1ms
#define POWER_ON_ENABLE_STUDY_TIME          10000  // 上电10s内允许遥控器学习，步长1ms
#define EN_ADD_CODE_TIME                    10000   // 允许添码操作时间，步长1ms
#define Light_KEY_SPACE_TIME                3000   // 步长1ms
#define MOTOR_DIR_CHANGE_DELAY              1000   // 电机换向延时时间，步长1ms
#define MOTOR_DIR_CHANGE_DELAY1             200    // 轻抬上升后电机上升延时时间，步长1ms

#define ALARM_YZ_BEEP_NUM                   1      // 蜂鸣器报警重复次数
#define ALARM_OVER_WEIGHT_BEEP_NUM          1      // 蜂鸣器报警重复次数
#define ALARM_MOTOR_BEEP_NUM                1     // 蜂鸣器报警重复次数
#define ALARM_BEEP_CYCLE_TIME               1300   // 报警周期时间 步长1ms

#define DIS_JUDGE_MOTOR_OVER_WEIGHT_TIME    450   // 电机开始上升屏蔽超重检测时间，步长1ms
#define DIS_JUDGE_MOTOR_UP_OBS_TIME         300    // 电机开始上升屏蔽遇障碍物检测，步长1ms

#define WAIT_CONFIG_NETTIME                 1716460  // 等待配网30min后橙灯亮起
#define LED_BLUE_TWINKLE_TIME               1800   // 闪烁持续时间 步长100ms
#define LED_BLUE_ON_TIME                    1800   // 长亮持续时间 步长100ms
#define LED_ORG_ON_TIME                     1800   // 长亮持续时间 步长100ms

#define LED_TWINKLE_ON_TIME                 1      // 闪烁亮灯时间 步长100ms
#define LED_TWINKLE_TIME                    5      // 闪烁周期时间 步长100ms
#define LIMIT_SIGNAL_FITER_TIME             80    //20     // 机械限位及非遇阻检测滤波时间 步长1ms
#define LIMIT_SIGNAL_SET_TIME               500    //20     // 机械限位及非遇阻检测滤波时间 步长1ms
#define HOLL_PULSE_FILTER_TIEM              1      // 霍尔信号滤波时间 步长1ms

/********************************状态定义********************************/
// 电机状态
#define MOTOR_SUSPEND_STATE                 0X00   // 暂停
#define MOTOR_UP_STATE                      0X01   // 上升状态
#define MOTOR_DOWN_STATE                    0X02   // 下降
#define MOTOR_UP_STOP_STATE                 0X03   // 上限位停止状态
#define MOTOR_DOWN_STOP_STATE               0X04   // 下限位停止状态
#define MOTOR_YZ_STATE                      0X05   // 遇阻状态
#define MOTOR_OVER_STATE                    0X06   // 超重状态
#define MOTOR_UPLIM_STATE                   0X07  // 上限位故障状态

// 提示音状态
#define BEEP_OFF                            0X00
#define BEEP_ON                             0X01

// 照明状态
#define LIGHT_OFF                           0X00
#define LIGHT_ON                            0X01

#define CONVERGENT_OFF                      0X00
#define CONVERGENT_ON                       0X01

// 遇阻信号判断时间定义
#define ENCOUNTER_Obstacle_TIME1            8     // 400ms,步长10ms
#define ENCOUNTER_Obstacle_TIME2            300    // 3s,步长10ms
#define ENCOUNTER_Obstacle_TIME3            20     // 步长10ms

// 灯渐亮步长
#define LIGHT_UP_STEP                       1600   // 3s渐亮

#define LIGHT_MAX_STEP                      600    //最大步长

#define LIGHT_MAX_PWM                       1600    //最大步长
#define DELAY_IDLE                          0
#define DELAY_WAITING                       1
#define LIGHT_TEM_RATIO                     0.026  


// 网络状态
#define NET_OFFLINE                         0
#define NET_UAP                             1
#define NET_LOCAL                           2
#define NET_CLOUD                           3
#define NET_UNPROV                          4
#define NET_UPDATING                        5

// 指示灯状态
#define LED_ORG_STATE_OFF                   0
#define LED_ORG_STATE_TWINK                 1
#define LED_ORG_STATE_ON                    2

#define LED_BLUE_STATE_OFF                  0
#define LED_BLUE_STATE_TWINK                1
#define LED_BLUE_STATE_ON                   2

/********************************Flash配置********************************/
#define DATA_FLASH_BLOCK_COUNTER            (2)
#define DATA_FLASH_BLOCK_SIZE               (512)
#define DATA_FLASH_SIZE                     (DATA_FLASH_BLOCK_COUNTER * DATA_FLASH_BLOCK_SIZE)
#define DATA_FLASH_ADDRESS_MAX              (DATA_FLASH_SIZE - 1)

#define FDL_FRQ                             (32)   // 时钟32
#define FDL_VOL                             (01)
#define BLOCK_ZERO                           0      //
#define BLOCK_ONE                            1      // 行程数据及运行次数

/********************************命令定义********************************/
#ifdef XIMI
				#define MOTOR_UP_ALL     0X11
				#define MOTOR_DOWN_ALL   0X33
				#define MOTOR_STOP_CMD   0X55

				#define MOTOR_UP_A       0XAA
				#define MOTOR_DOWN_A     0X88

				#define MOTOR_UP_B       0XBB
				#define MOTOR_DOWN_B     0X99

				#define LIGHT_CMD        0X0F
				#define STUDY_CMD        0X79
#else 
    #define MOTOR_UP_ALL                    0X11 
    #define MOTOR_DOWN_ALL                  0X33 
    #define MOTOR_STOP_CMD                  0X55
    #define MOTOR_UP_A                      0XAA
    #define MOTOR_DOWN_A                    0XDD
    #define MOTOR_UP_B                      0XCC
    #define MOTOR_DOWN_B                    0XEE
    #define LIGHT_CMD                       0X0F
    #define STUDY_CMD                       0X79
		#define KEY_POLE_ONE_HIDE               0x39
    #define KEY_POLE_TWO_HIDE               0x36
		#define SET_POLE_ONE_LIMITT             0x3C
		#define SET_POLE_TWO_LIMITT             0x3A
		/* 晾衣架控制命令 */
		#define YY_START                           0xBB
		#define CLOTHES_RACK_UP                    0x21
		#define POLE_ONE_UP                        0x2B
		#define POLE_TWO_UP                        0x2D
		#define CLOTHES_RACK_HIDE                  0x48
		#define POLE_ONE_HIDE                      0x4B
		#define POLE_TWO_HIDE                      0x4D
		#define CLOTHES_RACK_DOWN                  0x43
		#define POLE_ONE_DOWN                      0x5B
		#define POLE_TWO_DOWN                      0x5D
		#define CLOTHES_RACK_STOP                  0x65
		#define POLE_ONE_STOP                      0x6B
		#define POLE_TWO_STOP                      0x6D
		#define CLOTHES_RACK_UPPER_LIMIT           0x56
		#define POLE_ONE_UPPER_LIMIT               0x71
		#define POLE_TWO_UPPER_LIMIT               0x73
		#define CLOTHES_RACK_LOWER_LIMIT           0x66
		#define POLE_ONE_LOWER_LIMIT               0x7F
		#define POLE_TWO_LOWER_LIMIT               0x7D

		/* 照明控制命令 */
		#define ART_LIGHT_ON                       0x1F
		#define WHITE_LIGHT_ON                     0x89
		#define WARM_LIGHT_ON                      0x8A
		#define COLD_LIGHT_ON                      0x88
		#define DAYLIGHT_ON                        0xC0
		#define MOONLIGHT_ON                       0xB0
		#define ART_LIGHT_OFF                      0x2F
		#define BRIGHTNESS_LEVEL_1                 0x8B  // 30%亮度
		#define BRIGHTNESS_LEVEL_2                 0x8C  // 70%亮度
		#define BRIGHTNESS_LEVEL_3                 0x8D  // 100%亮度
#endif
/********************************433射频参数********************************/
#define OVEREIGHT_CURRENT_CMD              0XFE   // 超重电流校正命令
#define Capture_433KEY_UP_JUDGE_TIME       60     // 判断遥控器松手，步长5ms
#define Capture_433KEY_NONE                0X00
#define Capture_433KEY_PRESS               0X01
#define Capture_433KEY_UP                  0X02
#define Capture_433KEY_PRESS_LONG          0X03
#define RECEIVE_433_DISABLE_TIME           300    // 屏蔽接收时间

// 捕获码状态
#define CAPTURE_NO_DATA                    0X00   // 无数据
#define CAPTURE_HEAD_HIGH                  0X01   // 引导码高电平
#define CAPTURE_HEAD_LOW                   0X02   // 引导码低电平
#define CAPTURE_FIXED_CODE                 0X03   // 识别码
#define CAPTURE_ROLL_CODE                  0X04   // 命令码
#define CAPTURE_COMMANG_END                0X05   // 命令码
#define ALL_CODE_LEVELS                    129    // 最后1bit命令码只测高电平
#define FIXED_CODE_LEVELS                  64

// 引导电平冗余
#define HEAD_HIGH_MIN_TIME                  3800
#define HEAD_HIGH_MAX_TIME                  5000
#define HEAD_LOW_MIN_TIME                  (1200)
#define HEAD_LOW_MAX_TIME                  (1900)

// 数据电平冗余
#define _800US_MIN_TIME                    (650)
#define _800US_MAX_TIME                    (950)
#define _400US_MIN_TIME                    (200)
#define _400US_MAX_TIME                    (600)
#define _433_DATA                          Gpio_GetInputIO(GpioPortA,GpioPin6)
#define INTERFERE_PLUSE_WIDTH              200

/********************************蜂鸣器配置********************************/
// 频率定义
#define FREQ_NO                            0
#define FREQ_2300                          6950   // 3478//6957
#define FREQ_2600                          6150   // 3077//6154
#define FREQ_2900                          5500   // 2759//5517
#define FREQ_2000                          8000   // 4000//8000
#define FREQ_1238                          12924  // 6437//12874   //12924
#define FREQ_1288                          12422  // 6186//12372   //12422 
#define FREQ_1342                          11923  // 5937//11873   //11923
#define FREQ_NULL                          10000  // 作为间隔等待，当个标志

// 蜂鸣器控制
#define BUZ_ON                             Adt_StartCount(M0P_ADTIM5);  // AdvTimer5运行
#define BUZ_OFF                            Adt_StopCount(M0P_ADTIM5);  // AdvTimer5运行
#define BuzVcc_ON                          Gpio_SetIO (GpioPortB, GpioPin1);
#define BuzVcc_OFF                         Gpio_ClrIO (GpioPortB, GpioPin1);
/**
1	上电音	嘀嘀嘀（三音节，升调1）	设备上电、特殊命令成功提示

2	开启音	嘀嘀嘀（三音节，升调）	照明、电机开启成功

3	关闭音	嘀嘀嘀（三音节，降调）	照明等功能关闭

4	提示音1	嘀（短鸣）	用于提示上下限位
5	提示音2	嘀-（长鸣）	用于提示组合按键中间步骤的等等
6	报警1	嘀-（长鸣）	    每隔1.3秒，长鸣1声， 遇阻报警
7	报警2	嘀-（长鸣）     每隔1.3秒，长鸣2声， 超重报警
8	报警3	嘀-（长鸣）   	每隔1.3秒，长鸣3声， 电机过热报警
9	电机停止	嘀-（长鸣）	通过再次点按上升或下降键，当停止电机，设备无提示音。
10	无效操作	无提示	对于设备当前状态无效的操作，设备无反应也不会有声音提示
**/
#define	 	M_BZ		      0    
#define	 	M_BZ_DI	      1	 
#define	 	M_BZ_KEY 	    2	 
#define	 	M_BZ_ON		    3	 
#define	 	M_BZ_OFF	    4	 
#define	 	M_YZ_ONE	    5	 
#define	 	M_CZ_TWO	    6    
#define	 	M_MOR_THREE	  7	
#define	 	M_FUN_ON	    8	
#define	  M_FUN_OFF     9	

#define	 	BEEP_PWM_OFF        0 
#define   SHORT_BEEP          1
#define   LONG_BEEP           2 
#define   RISE_TONE_BEEP      3 

#define   LONG_BEEP_ONE       4
#define   LONG_BEEP_TWO       5
#define   LONG_BEEP_THREE     6

#define   RISE_TONE_BEEP_MODE1      7 
#define   FALL_TONE_BEEP_MODE1      8

#define   POWER_ON_BEEP              	LONG_BEEP//上电音

#define   CODE_MATCH_BEEP             LONG_BEEP
#define   WIFI_RESET_BEEP             LONG_BEEP
#define   ADD_MATCH_CODE_BEEP         LONG_BEEP

#define   LIGHT_ON_BEEP               SHORT_BEEP
#define   LIGHT_OFF_BEEP              LONG_BEEP

#define   FAN_ON_BEEP     SHORT_BEEP
#define   FAN_OFF_BEEP    LONG_BEEP

#define   PTC_ON_BEEP     SHORT_BEEP
#define   PTC_OFF_BEEP    LONG_BEEP

#define   UV_ON_BEEP      SHORT_BEEP
#define   UV_OFF_BEEP     LONG_BEEP

#define   GENTLY_LIFT_MOTOR_UP_BEEP  LONG_BEEP//轻抬上升蜂鸣器

#define   MOTOR_UP_BEEP     SHORT_BEEP  
#define   MOTOR_DOWN_BEEP   SHORT_BEEP  

#define   MOTOR_STOP_BEEP        LONG_BEEP
#define   YZ_MOTOR_STOP_BEEP     LONG_BEEP


#define   YZ_ALARM_BEEP          	       LONG_BEEP_THREE
#define   OVER_WIGHT_ALARM_BEEP          (9)
#define   MOTOR_ERR_ALARM_BEEP           (10)

/********************************PWM电机控制********************************/
#define PWM_STOP                          0
#define PWM_DOWN                          1 
#define PWM_UP                            2 
#define PWM_BASE_VALUE                    1800        // PWM基础值 2200/3200 启动占空比
#define PWM_MAX_VALUE                     3200        // PWM最大占空比值因子

#define PWM_MIN_COMPARE                   5           // 最小比较值阈值
#define PWM_MAX_COMPARE                   100         // 最大比较值阈值 100*5ms=500ms
#define PWM_START_SPEED                   2           // PWM启动速度因子
#define PWM_STOP_SPEED                    2           // PWM停止速度因子


#define PWM_DUTY                          PWM_BASE_VALUE + ((PWM_MAX_VALUE-PWM_BASE_VALUE) / (PWM_MAX_COMPARE *PWM_RAMP_RATE)) *PWM_RAMP_RATE * u16CHxACompare
#define PWM_DUTY_B                        PWM_BASE_VALUE + ((PWM_MAX_VALUE-PWM_BASE_VALUE) / (PWM_MAX_COMPARE *PWM_RAMP_RATE)) *PWM_RAMP_RATE * u16CHxbCompare

#define SET_MOTOR_PWM(value)              (pstcM0PBt->CCR0A_f.CCR0A = (value))
#define SET_MOTOR_PWM_B(value)            (pstcM0PBt->CCR0B_f.CCR0B = (value))

#define MotorA_PWM_Stop                   pstcM0PBt->FLTR_f.OCMA0_FLTA0 = 0;   // 设置高电平
#define MotorA_PWM_Start                  pstcM0PBt->FLTR_f.OCMA0_FLTA0 = 6;   // 设置PWM模式
#define MotorA_PWM_MAX                    pstcM0PBt->FLTR_f.OCMA0_FLTA0 = 1;   // 设置低电平

#define MotorB_PWM_Stop                   pstcM0PBt->FLTR_f.OCMB0_FLTB0 = 0;   // 设置高电平
#define MotorB_PWM_Start                  pstcM0PBt->FLTR_f.OCMB0_FLTB0 = 6;   // 设置PWM模式
#define MotorB_PWM_MAX                    pstcM0PBt->FLTR_f.OCMB0_FLTB0 = 1;   // 设置低电平

/********************************其他定义********************************/
#define REG_EXT                          extern volatile
#define _433_COMMAND                     1
#define APP_COMMAND                      0
#endif

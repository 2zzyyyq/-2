#ifndef __HEAD_FUNCTION_H__
#define __HEAD_FUNCTION_H__

#include "ALL_Var.h"
void System_init(void);
void App_TimerLEDCfg(uint16_t u16Period, uint16_t u16CHxACompare);
void App_TimerLEDPortCfg(void);
void App_TimerLED2Cfg(uint16_t u16Period, uint16_t u16CHxACompare);
//TIM0  1ms 定时器 
extern void App_AdvTimerInit(uint16_t u16Period,  uint16_t u16CHB_PWMDuty);
extern void App_AdvTimerPortInit(void);
extern void App_Timer0PortCfg(void);
extern void App_Timer0Cfg(uint16_t u16Period);
extern void App_Timer0PortCfg(void);
extern void App_Timer0Cfg(uint16_t u16Period);
extern void Run_to_position(void);
extern void Run_to_position_B(void);
extern void Action_deal(void);
extern void Return_Properties(void);//回应app查询属性
extern void Set_Properties (void);//回应app控制指令
extern uint8_t properties_up(void);//设备状态上报
extern void BootLoardDeal(void);
extern void App_portCfg(void);
extern void App_UartCfg(void);
extern void App_LpUartCfg(void);
extern void App_LpUartPortCfg(void);
extern void r_uart0_service(void);
extern unsigned char Queue_Read_Byte(void);
extern void r_uart0_receive_input(unsigned char value);
extern unsigned char get_queue_total_data(void);
extern void r_uart0_send_bytes(unsigned char *src_p,unsigned char dataNum);
extern void App_ClockCfg(void);
extern void r_uart0_protocol_init(void);
extern void _150ms_work(void);
extern void UpMCUversion(void);
extern void r_uart1_service(void);
extern void r_uart1_protocol_init(void);
extern void App_TimerLED2Cfg(uint16_t u16Period, uint16_t u16CHxACompare);
extern void r_uart1_send_bytes(unsigned char *src_p,unsigned char dataNum);
extern void App_Timer1PortCfg(void);//Timer3 Port端口配
extern void App_Timer1Cfg(void);
extern void App_Timer3PortCfg(void);
extern void App_Timer3Cfg(void);
extern void variable_init(void);
extern void _500ms_work(void);
extern void function_select(void);
extern void App_WdtInit(void);
extern void SetupBzhx(uint8_t MBz,uint8_t flag);
extern uint8_t CmdBzhx(void);
extern void App_AdvTimerInit(uint16_t u16Period,  uint16_t u16CHB_PWMDuty);
extern void App_AdvTimerPortInit(void);
extern void App_GpioInit(void);
extern void App_SPIInit(void);
extern void App_UserKeyInit(void); 
extern void App_LedInit(void);
extern void r_uart1_receive_input(unsigned char value);
extern void light_led_work(void);//照明灯做指示灯动效
/****************FLASH*************************FLASH*********************FLASH**********************FLASH**********************FLASH***************/
extern void read_data(void);
extern void Cmd_Light(void);
extern void ModuleResetReq(void);//重置模块
extern uint16_t Read_FLASH_MUIData(uint32_t Addr, uint8_t *pBuf, uint16_t LEN);//读取多字节
extern uint8_t Read_FLASH_Data(uint32_t Addr);
extern void match_data_del(void);
extern void match_data_clear(void);
extern void save_data(void);
extern void save_current_position(void);
extern void App_LvdInit(void);
extern void App_LvdPortInit(void);
extern void _220AC_Power_down_save(void);//掉电保存
extern void limit_protect(void);//
extern 	void limit_protect_B(void);//
extern void data_handle(void);
extern uint8_t Flash_WriteStruct(uint32_t u32StartAddr, void *pData, uint16_t size);
extern unsigned char getBit(unsigned char source[], int n) ;
extern void RF_DEC(unsigned char *source) ;
extern void RLC(unsigned char *source, char c, char n);
extern void Keelop_KEY(unsigned long  key_write);
extern void PowerOn_ModelSet(void);
extern void led_work(void);
extern void variable_init(void);
extern void App_TimermotorPortCfg(void);
extern void App_TimermotorCfg(uint16_t u16Period, uint16_t u16CHxACompare, uint16_t u16CHxBCompare);
extern void alarm_signal_test_and_beep_B(void);
extern void alarm_signal_test_and_beep(void);

/*****************电机*******************//*****************电机*******************//*****************电机*******************//*****************电机*******************//*****************电机*******************/

extern  void App_MORInit(void);
extern  void App_Init(void);
extern void Cmd_Motor_Down(void);
extern void Cmd_Motor_Stop(void);
extern void Cmd_Motor_Stop_B(void);
extern void light_work(uint8_t light_state);
extern void Cmd_Motor_Down_B(void);
extern void motor_work(uint8_t motor_state);
extern void motor_work_B(uint8_t motor_state);
extern void motor_stop(void);
extern void motor_stop_B(void);
extern void motor_up(void);
extern void motor_up_B(void);
extern void motor_down(void);
extern void motor_down_B(void);
extern void Delay1ms(uint32_t nTime);
extern void Cmd_Motor_Up_B(void);
extern void Cmd_Motor_Up(void);
extern void Cmd_Motor_Up_Longpress(void);
extern void Cmd_Motor_Up_Longpress_B(void);
extern void Motor_speed(void);
extern void Motor_speed_B(void);
/*****************ADC*************//*****************ADC*************//*****************ADC*************//*****************ADC*************//*****************ADC*************/

extern void App_AdcJqrCfg(void);
extern void App_AdcPortInit(void);
extern void App_AdcInit(void);
extern uint16_t motor_current_to_advalue(float  circle);
extern uint16_t motor_current_to_advalue_B(float  circle);
extern void Ovreweight_current_correct(void);//超重按整机校正，两杆均匀分布挂重，各挂20kg

extern void motor_stop(void);
extern void motor_stop_B(void);
extern void motor_up(void);
extern void motor_up_B(void);
extern void motor_down(void);
extern void motor_down_B(void);

extern void motor_work(uint8_t motor_state);
extern void motor_work_B(uint8_t motor_state);
extern void light_work(uint8_t light_state);
#endif

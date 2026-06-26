#include "ALL_Var.h"
uint8_t u8RxData;
void App_LpUartCfg(void);
void App_LpUartPortCfg(void);
void r_uart1_protocol_init(void);
void r_uart1_receive_input(unsigned char value);
unsigned char Queue_Read_Byte_ars(void);
static uint8_t ars_uart_data_unpack(uint8_t data);
void r_uart1_service(void);
unsigned char get_queue_total_data_ars(void);
uint8_t calculate_low8_checksum(const uint8_t *data, uint32_t length);
void ASR_Date_handled(void);
static uint16_t UART1_RX_Count = 0,Frame1_length=0;
uint8_t *queue_in_ars;
uint8_t *queue_out_ars;
char  uart1_queue_buf[QUEUE_BUF_DEEP];
uint8_t uart1_rx_buf[50]; 
uint8_t uart1_tx_buf[50]; 
void r_uart1_send_bytes(unsigned char *src_p,unsigned char dataNum)
{
	uint8_t i=0;
	for(i=0;i<dataNum;i++)
	{
	   
         LPUart_SendDataIt(M0P_LPUART1, *src_p++);	     //调用库函数，通过UART0发送一个字母。
	       while(LPUart_GetStatus(M0P_LPUART1, LPUartTxe)==FALSE)
				 {
		       		 ;
				 }
		send_busy_asr=1;
	}
}

void r_uart1_service(void)
{ 
  if(get_queue_total_data_ars() > 0)
  {
    if(ars_uart_data_unpack(Queue_Read_Byte_ars()))//收到一包数据
    {
			   
          //ASR_Date_handled();
          memset((char *)uart1_rx_buf,0,sizeof(uart1_rx_buf));
	        UART1_RX_Count = 0;
    }
  }
 }

unsigned char get_queue_total_data_ars(void)
{  
if(queue_in_ars != queue_out_ars)
    return 1;
  else
    return 0;
}

void r_uart1_receive_input(unsigned char value)
{
  if((queue_in_ars > queue_out_ars) && ((queue_in_ars - queue_out_ars) >= sizeof(uart1_queue_buf)))
  {
	// the data queue is full
  }
  else if((queue_in_ars < queue_out_ars) && ((queue_out_ars  - queue_in_ars) == 0))
  {
    //Data queue is full
  }
  else
  {
    //Queue is not full
    if(queue_in_ars >= (unsigned char *)(uart1_queue_buf + sizeof(uart1_queue_buf)))
    {
      queue_in_ars = (unsigned char *)(uart1_queue_buf);
    }  
    *queue_in_ars ++ = value;
  }
}



unsigned char Queue_Read_Byte_ars(void)
{
  unsigned char value;
  
  if(queue_out_ars != queue_in_ars)
  {
    //Data is not empty
    if(queue_out_ars >= (unsigned char *)(uart1_queue_buf + sizeof(uart1_queue_buf)))
    {
      //Data has reached the end
      queue_out_ars = (unsigned char *)(uart1_queue_buf);
    }    
    value = *queue_out_ars ++;   
  } 
  return value;
}



void r_uart1_protocol_init(void)
{
  queue_in_ars = (unsigned char *)uart1_queue_buf;
  queue_out_ars = (unsigned char *)uart1_queue_buf;
}
#define MAX_FRAME_LENGTH 256  // 最大帧长度
#define HEADER_HIGH 0x55       // 帧头高字节
#define HEADER_LOW 0xAA        // 帧头低字节
#define VERSION 0x01           // 协议版本号

static uint8_t ars_uart_data_unpack(uint8_t data)
{
    uint8_t ret = FALSE;
	  uint8_t CRC = 0;
  if(Time.Uart_Receive_Interval_time>=RECEIVE_DATA_PACKET_INTERVAL_TIME)//间隔20ms判定为下一包数据
     {
     	UART1_RX_Count=0;
     }
      Time.Uart_Receive_Interval_time=0;
      uart1_rx_buf[UART1_RX_Count]=data;
 if(uart1_rx_buf[UART1_RX_Count-6]==HEADER_HIGH&&uart1_rx_buf[UART1_RX_Count-5]==HEADER_LOW)//帧头
		 {
		    CRC=calculate_low8_checksum(uart1_rx_buf,UART1_RX_Count);//校验和
  	if(uart1_rx_buf[UART1_RX_Count]==CRC)
		{
		  Frame1_length=UART1_RX_Count;
	    UART1_RX_Count=0;
    	ret = TRUE;
		}
}
    UART1_RX_Count++;
    if(UART1_RX_Count >= sizeof(uart1_rx_buf)) 
    {
	    UART1_RX_Count=0;
  	  Flag.Uart1_Err = 1;
	    ret = TRUE;
    }
    return ret;
}

/**
 * @brief 计算数据的低8位校验和
 * @param data 指向要计算校验和的数据缓冲区
 * @param length 数据长度（字节数）
 * @return 计算得到的8位校验和
 */
uint8_t calculate_low8_checksum(const uint8_t *data, uint32_t length) {
    uint8_t checksum = 0;
    
    for (uint32_t i = 0; i < length; i++) {
        checksum += data[i];  // 简单累加所有字节
    }
    
    return checksum;  // 只返回低8位
}
//固定帧头	版本号	数据长度	数据属性/方向	数据	校验和（低八位）

typedef enum {
    ASR_VOLUME = 0xA1,
    ASR_VOICE = 0xA2,
    ASR_MOTOR_UP = 0xA3,
    ASR_MOTOR_DOWN = 0xA4,
    ASR_MOTOR_STOP = 0xA5,
    ASR_UPPER_LIMIT = 0xA6,
    ASR_DOWN_LIMIT = 0xA7,
    ASR_LIGHT = 0xA8,
    ASR_MOONLIGHT = 0xA9,
    ASR_COLOR_TEMPERATURE_SET = 0xAA,
    ASR_LIGHT_SET = 0xAB,
    ASR_STERILIZATION = 0xAC,
    ASR_WIND = 0xAD
} ASR_Command;

ASR_Command ASR_command;

#define voiceOn           0X01
#define voiceOFF          0X02
//上升
#define clothesHangerUp1   0X01
#define clothesHangerUp2   0X02
#define clothesHangerUp3   0X03
#define clothesHangerUp4   0X04
#define clothesHangerUp5   0X05
#define clothesHangerUp6   0X06
#define clothesHangerUp7   0X07
#define clothesHangerUp8   0X08
#define clothesHangerUp9   0X09
#define clothesHangerUp0   0X0A
#define clothesHangerUpA   0X0B
#define clothesHangerUpB   0X0C
#define clothesHangerUpC   0X0D
//下降
#define clothesHangerDown1  0X01
#define clothesHangerDown2  0X02
#define clothesHangerDown3  0X03
#define clothesHangerDown4  0X04
#define clothesHangerDown5  0X05
#define clothesHangerDown6  0X06
#define clothesHangerDown7  0X07
#define clothesHangerDown8  0X08
#define clothesHangerDown9  0X09
#define clothesHangerStop1  0X02
#define clothesHangerStop2  0X03
#define clothesHangerStop3  0X01

//uint8_t           ASR_BACK[8]={0x55 ,0xAA ,0x01 ,0x07 ,0xD2, 0x01, 0xDA};  
//uint8_t           ASR_BACK_CLOSE[8]={0x55 ,0xAA ,0x01 ,0x07 ,0xD2, 0x02, 0xDB};  
//uint8_t           Upper_limit_tip[8]={0x55 ,0xAA ,0x01 ,0x07 ,0xD3, 0x05, 0xDF};  //已到上限位，继续上升请说衣架隐藏  
//static uint8_t    Down_limit_tip[8]={0x55 ,0xAA ,0x01 ,0x07 ,0xD4, 0x03, 0xDE};    //已到下限位
//                     
//uint8_t           Upper_limit_tip1[8]={0x55 ,0xAA ,0x01 ,0x07 ,0xD3, 0x03, 0xDD};  //已到A上限位，继续上升请说衣架隐藏                                         55 AA 01 07 D3 03 DD
//uint8_t           Upper_limit_tip2[8]={0x55 ,0xAA ,0x01 ,0x07 ,0xD3, 0x04, 0xDE};  //已到B上限位，继续上升请说衣架隐藏 
//static uint8_t    Uppering_asr[8]={0x55, 0xAA, 0x01, 0x07, 0xD3, 0x01, 0xDB};   //正在上升   
//static uint8_t    Downing_asr[8]={0x55, 0xAA, 0x01, 0x07, 0xD4, 0x01, 0xDC};   //正在下降 
//static uint8_t    Open_light_asr[8]={0x55 ,0xAA ,0x01, 0x07 ,0xD8 ,0x01,0xE0};   //开灯
//static uint8_t    Open_Nightlight_asr[8]={0x55 ,0xAA ,0x01, 0x07 ,0xD9 ,0x01,0xE1};   //夜灯开灯
//static uint8_t    brightness_asr[8]={0x55, 0xAA ,0x01 ,0x07 ,0xDB, 0x01, 0xE3};  // 灯光已调整
//static uint8_t    Close_light_asr[8]={0x55 ,0xAA ,0x01 ,0x07 ,0xD9, 0x02 ,0xE2};   //关
//static uint8_t    MAX_LIGHT_asr[8]={0x55, 0xAA, 0x01, 0x07 ,0xDB, 0x02 ,0xE4}; //已最大亮度                                              //55 AA 01 07 DB 02 E4 MAX_LIGHT
//static uint8_t    MIX_LIGHT_asr[8]={0x55, 0xAA, 0x01, 0x07 ,0xDB, 0x03 ,0xE5};  //已最小亮度                            //55 AA 01 07 DB 03 E5
//static uint8_t    LIGHT_SET_asr[8]={0x55, 0xAA, 0x01, 0x07 ,0xDA, 0x01 ,0xE2};//已调整最大值
//static uint8_t    STOP_asr[8]={0x55, 0xAA, 0x01, 0x07 ,0xD5, 0x01 ,0xDD};//已停止
//static uint8_t    Hide_asr[8]={0x55, 0xAA, 0x01, 0x07 ,0xD3, 0x02 ,0xDC};
//uint8_t           Confirm_asr[8]={0x55, 0xAA, 0x01, 0x07 ,0xD3, 0x02 ,0xDC};// 请确认晾杆清空，再次操作
//static uint8_t    Confirm_asr1[8]={0x55, 0xAA, 0x01, 0x07 ,0xD3, 0x06 ,0xE0};//请确认晾杆一清空，再次操作
//static uint8_t    Confirm_asr2[8]={0x55, 0xAA, 0x01, 0x07 ,0xD3, 0x07 ,0xE1};// 请确认晾杆二清空，再次操作
//static uint8_t    upper_limit_set1[8]={0x55, 0xAA, 0x01, 0x07 ,0xD6, 0x01 ,0xDE};///已设置衣架一上限位
//static uint8_t    upper_limit_set2[8]={0x55, 0xAA, 0x01, 0x07 ,0xD6, 0x02 ,0xDF};//已设置衣架e二上限位
//static uint8_t    upper_limit_Cancel[8]={0x55, 0xAA, 0x01, 0x07 ,0xD6, 0x03 ,0xE0};//已取消衣架e二上限位
//static uint8_t    upper_limit_Cance2[8]={0x55, 0xAA, 0x01, 0x07 ,0xD6, 0x04 ,0xE1};//已取消衣架e二上限位
//static uint8_t    Down_limit_set1[8]={0x55, 0xAA, 0x01, 0x07 ,0xD7, 0x01 ,0xDF};///已设置衣架一上限位
//static uint8_t    Down_limit_set2[8]={0x55, 0xAA, 0x01, 0x07 ,0xD7, 0x02 ,0xE0};//已设置衣架e二上限位
//static uint8_t    Down_limit_Cancel[8]={0x55, 0xAA, 0x01, 0x07 ,0xD7, 0x03 ,0xE1};///已取消衣架一上限位
//static uint8_t    Down_limit_Cance2[8]={0x55, 0xAA, 0x01, 0x07 ,0xD7, 0x04 ,0xE2};//已取消衣架e二上限位


//uint8_t           Obs1[8]={0x55, 0xAA, 0x01, 0x07 ,0xD4, 0x06 ,0xE1};// 遇阻
//uint8_t           Obs2[8]={0x55, 0xAA, 0x01, 0x07 ,0xD4, 0x07 ,0xE2};// 遇阻
//uint8_t           OVER_WIGHT1[8]={0x55, 0xAA, 0x01, 0x07 ,0xD4, 0x08 ,0xE3};// 超重
//uint8_t           OVER_WIGHT2[8]={0x55, 0xAA, 0x01, 0x07 ,0xD4, 0x09 ,0xE4};// 超重

void ASR_Date_handled  (void)
{
			uint8_t  *data = uart1_rx_buf;
			uint8_t  CommandType = data[4];
			uint8_t  Action=data[5];
      static uint8_t  hide_type=0;
	if(data[0]==HEADER_HIGH&&data[1]==HEADER_LOW)
	{
		switch(CommandType)//指令类型
		{
						case ASR_VOLUME:             
						break;
						case ASR_VOICE: 
				 	switch(Action)
									{
									case 0x01: //关闭语音
									{
								//		r_uart1_send_bytes(ASR_BACK,7);		
									}
						break;
									case 0x02: //关闭语音
									{
										//r_uart1_send_bytes(ASR_BACK_CLOSE,7);
									}
						break;
								}
						break;
						case ASR_MOTOR_UP:     //电机上升
						{							
									switch(Action)//电机上升命令
									{
									case clothesHangerUp1: //电机  上升
									{
										
								    if(Flag.ELE_LIM==1&&Flag.B_ELE_LIM==1)
												{
								   //  			r_uart1_send_bytes(Upper_limit_tip,7);
												
												}else{
												
														   Flag.No_judge_electronic_limit=0;
														if(Device_State_Data.Motor_State_Byte!=MOTOR_UP_STATE)
														{
												  		  Cmd_Motor_Up();
														}
														  Flag.B_No_judge_electronic_limit=0;
														if(Device_State_Data.B_Motor_State_Byte!=MOTOR_UP_STATE)
														{
														    Cmd_Motor_Up_B();
														}
												//	r_uart1_send_bytes(Uppering_asr,7);
												}

									}             
									break;
									case clothesHangerUp2:  //电机A上升      
                   {
												if(Flag.ELE_LIM==1)
												{
								     //			r_uart1_send_bytes(Upper_limit_tip1,7);
												}else
												{
														Flag.No_judge_electronic_limit=0;
														if(Device_State_Data.Motor_State_Byte!=MOTOR_UP_STATE)
														{
														Cmd_Motor_Up();
											 	}
													//r_uart1_send_bytes(Uppering_asr,7);
												}
								    } 
									break;
									case clothesHangerUp3: //电机B上升 
                  {	
														if(Flag.B_ELE_LIM==1)
														{
													  //  	r_uart1_send_bytes(Upper_limit_tip2,7);
														}else
														{
														    Flag.B_No_judge_electronic_limit=0;
														if(Device_State_Data.B_Motor_State_Byte!=MOTOR_UP_STATE)
														{
														    Cmd_Motor_Up_B();
														}
												    	//	r_uart1_send_bytes(Uppering_asr,7);
														}
									} 
									break;
									case clothesHangerUp4:  //电机高一点 
									{
													if(	Flag.ELE_LIM==0)
													{
													Device_State_Data.set_position=Device_State_Data.Motor_Position-5;
													Run_to_position();	
													}
													if(	Flag.B_ELE_LIM==0)
													{
													Device_State_Data.B_set_position=Device_State_Data.B_Motor_Position-5;
													Run_to_position_B();
													}																								
									}                 
									break;
									case clothesHangerUp5  :  //电机A高一点    
                                       if(	Flag.ELE_LIM==0)
																			 {
																					Device_State_Data.set_position=Device_State_Data.Motor_Position-5;
																					if(Device_State_Data.Motor_Position<10)
																					Device_State_Data.Motor_Position=0;
																					Run_to_position();	
																			 }else
																			 {
																		   	// r_uart1_send_bytes(Upper_limit_tip,7);
																			 }											 
									break;
									case clothesHangerUp6  :   //电机B高一点   
												     if(	Flag.B_ELE_LIM==0)
																			{
																					Device_State_Data.B_set_position=Device_State_Data.B_Motor_Position-5;
																			if(Device_State_Data.B_Motor_Position<10)
																					Device_State_Data.B_Motor_Position=0;
																					Run_to_position_B();	
																			}else
																			{
																		    //	r_uart1_send_bytes(Upper_limit_tip,7);
																			}	
									break;

									case clothesHangerUp7  :    //电机升一半   
													if(	Flag.ELE_LIM==0)
													{
															Device_State_Data.set_position=50;
															if(Device_State_Data.Motor_Position<10)
															Device_State_Data.Motor_Position=0;
															Run_to_position();	
													}else
													{
														//	r_uart1_send_bytes(Upper_limit_tip,7);
													}	
													if(	Flag.B_ELE_LIM==0)
													{
																	Device_State_Data.B_set_position=50;
															if(Device_State_Data.B_Motor_Position<10)
																 Device_State_Data.B_Motor_Position=0;
																 Run_to_position_B();	
													}else
													{
														//r_uart1_send_bytes(Upper_limit_tip,7);
												}										
									break;
									case clothesHangerUp8  :    //电机A升一半  
													if(	Flag.ELE_LIM==0)
													{
																Device_State_Data.set_position=50;
																if(Device_State_Data.Motor_Position<10)
																Device_State_Data.Motor_Position=0;
																Run_to_position();	
													}else
													{
																//	r_uart1_send_bytes(Upper_limit_tip,7);
													}		
									break;
								case clothesHangerUp9  :    //电机B升一半 

															if(	Flag.B_ELE_LIM==0)
															{
																	Device_State_Data.B_set_position=50;
																	if(Device_State_Data.B_Motor_Position<10)
																	Device_State_Data.B_Motor_Position=0;
																	Run_to_position_B();	
															}else
															{
														 //  	r_uart1_send_bytes(Upper_limit_tip,7);
															}	
							 break;
														case clothesHangerUp0  :    //电机隐藏  
															
									{	
														
									static uint8_t asr_cnt = 0;
									static uint32_t art_timecnt = 0;

									if (asr_cnt == 0) {
									// 第一次按下
									asr_cnt++;
									art_timecnt = Time.key_convergent;
								//	r_uart1_send_bytes(Confirm_asr,7);
									return;
									} else {
									// 第二次按下
									if (Time.key_convergent - art_timecnt>10000) {
									// 时间间隔超过200，仍然发送Confirm_asr
								//	r_uart1_send_bytes(Confirm_asr, 7);
									art_timecnt = Time.key_convergent;  // 更新时间记录
									// 保持cnt_eve=1，等待下一次按键
									asr_cnt = 1;
									return;
									} else {
										if (Device_State_Data.convergent_all == CONVERGENT_OFF)
													{

														Device_State_Data.convergent_all=CONVERGENT_ON;
														if(Device_State_Data.convergent==CONVERGENT_OFF)
														{
															Cmd_Motor_Up_Longpress();
														}
														if(Device_State_Data.B_convergent==CONVERGENT_OFF)
														{
															Cmd_Motor_Up_Longpress_B();
														}
													}		
														//r_uart1_send_bytes(Uppering_asr,7);
											asr_cnt = 0;  // 重置计数
						 }
									}
														
								}		break;
														case clothesHangerUpA  :    //电机A隐藏 
														{					
									static uint8_t asr_cnt1 = 0;
									static uint32_t art_timecnt1 = 0;

									if (asr_cnt1 == 0) {
									// 第一次按下
									asr_cnt1++;
									art_timecnt1 = Time.key_convergent;
								//	r_uart1_send_bytes(Confirm_asr,7);
									return;
									} else {
									// 第二次按下
									if (Time.key_convergent - art_timecnt1>10000) {
									// 时间间隔超过200，仍然发送Confirm_asr
								//	r_uart1_send_bytes(Confirm_asr, 7);
									art_timecnt1 = Time.key_convergent;  // 更新时间记录
									// 保持cnt_eve=1，等待下一次按键
									asr_cnt1 = 1;
									return;
									} else {
								
											if(Device_State_Data.convergent==CONVERGENT_OFF)
											{
											Cmd_Motor_Up_Longpress();
											} 
								//			r_uart1_send_bytes(Uppering_asr,7);
											asr_cnt1 = 0;  // 重置计数
									}
									}
			
								}		break;
									case clothesHangerUpB  :    //电机B隐藏  

												{					
									static uint8_t asr_cnt2 = 0;
									static uint32_t art_timecnt2 = 0;

									if (asr_cnt2 == 0) {
									// 第一次按下
									asr_cnt2++;
									art_timecnt2 = Time.key_convergent;
									//r_uart1_send_bytes(Confirm_asr,7);
									return;
									} else {
									// 第二次按下
									if (Time.key_convergent - art_timecnt2>10000) {
									// 时间间隔超过200，仍然发送Confirm_asr
								//	r_uart1_send_bytes(Confirm_asr, 7);
									art_timecnt2 = Time.key_convergent;  // 更新时间记录
									// 保持cnt_eve=1，等待下一次按键
									asr_cnt2 = 1;
									return;
									} else {

											if(Device_State_Data.B_convergent==CONVERGENT_OFF)
											{
										  	Cmd_Motor_Up_Longpress_B();
											}  
									//		r_uart1_send_bytes(Uppering_asr,7);
											asr_cnt2 = 0;  // 重置计数
										 }
			             }
            }
														
														
								   	break;
									case clothesHangerUpC  :    //确定隐藏  
									switch(hide_type)
									{
									case 1:  //确定隐藏   
									if (Device_State_Data.convergent_all == CONVERGENT_OFF)
									{

										Device_State_Data.convergent_all=CONVERGENT_ON;
										if(Device_State_Data.convergent==CONVERGENT_OFF)
										{
											Cmd_Motor_Up_Longpress();
										}
										if(Device_State_Data.B_convergent==CONVERGENT_OFF)
										{
											Cmd_Motor_Up_Longpress_B();
										}
									}		
									//	r_uart1_send_bytes(Uppering_asr,7);
										hide_type=0;
									break;
									case 2:   	//确定A隐藏 
										if(Device_State_Data.convergent==CONVERGENT_OFF)
										{

											Cmd_Motor_Up_Longpress();
										} 
										hide_type=0;
									//	r_uart1_send_bytes(Uppering_asr,7);
									break;
									case 3:   //确定B隐藏 
										if(Device_State_Data.B_convergent==CONVERGENT_OFF)
										{

									   Cmd_Motor_Up_Longpress_B();
										}  
								//		r_uart1_send_bytes(Uppering_asr,7);
										hide_type=0; 
									break;
									}
								   	break;
							  	  default:    break;
					}
				}	break;
						case ASR_MOTOR_DOWN:   //电机下降
						{       
													   switch(Action)//电机下降命令
																		{
																			case clothesHangerDown1://全部下降
																					{
																							Flag.No_judge_electronic_limit=0;
																							if(Device_State_Data.Motor_State_Byte!=MOTOR_DOWN_STATE)
																							{
																						    	Cmd_Motor_Down();
																							}
																							Flag.B_No_judge_electronic_limit=0;
																							if(Device_State_Data.B_Motor_State_Byte!=MOTOR_DOWN_STATE)
																							{
																							   Cmd_Motor_Down_B();
																							}
																					//		r_uart1_send_bytes(Downing_asr,7);
																					}
																							
																			break;
																				case clothesHangerDown2:  //晾衣架A下降
																				{								
																				
																								if(Up_State_Data.Err_Byte&0x01)
																								{
																								Up_State_Data.Motor_State_Byte=MOTOR_DOWN_STATE;//app会点亮图标，如果此时是遇阻，需要上报停止
																								Err.err_bit.Encounter_Obstacle=0;
																								Time.Encounter_Obstacle_beepnum=0;
																								Up_State_Data.Err_Byte&=0xfe;//重新上报app遇阻
																								}
																								else
																								{
																								Flag.No_judge_electronic_limit=0;
																								if(Device_State_Data.Motor_State_Byte!=MOTOR_DOWN_STATE)
																								{
																								Cmd_Motor_Down();
																								}
																								}
																							//	r_uart1_send_bytes(Downing_asr,7);
																				}
																				break;
																				case clothesHangerDown3:   //晾衣架B下降   
																				{
																						if(Up_State_Data.B_Err_Byte&0x01)
																						{
																								Up_State_Data.B_Motor_State_Byte=MOTOR_DOWN_STATE;//app会点亮图标，如果此时是遇阻，需要上报停止
																								Err_B.err_bit.Encounter_Obstacle=0;
																								Time.B_Encounter_Obstacle_beepnum=0;
																								Up_State_Data.B_Err_Byte&=0xfe;//重新上报app遇阻
																						}
																						else
																						{
																								Flag.B_No_judge_electronic_limit=0;
																						if(Device_State_Data.B_Motor_State_Byte!=MOTOR_DOWN_STATE)
																						{
																						  Cmd_Motor_Down_B();
																						}
																						}  
	                                      //  r_uart1_send_bytes(Downing_asr,7);																						
 
																				}																					
																				break;
																				

																				case clothesHangerDown4:     //晾衣架低一点下降   

																						Device_State_Data.set_position=Device_State_Data.Motor_Position+10;
																						Run_to_position();	
																						Device_State_Data.B_set_position=Device_State_Data.B_Motor_Position+10;
																						Run_to_position_B();	
                                           // r_uart1_send_bytes(Downing_asr,7);	
																				break;


																				case clothesHangerDown5:      //晾衣架A低一点下降   
																					if(Flag.ELE_LIM_DOWN==1&&	Flag.A_Up_limit==0)
																											{//SetupBzhx(LONG_BEEP,Flash_Data.Beep_open);
																											  return ;
																											}				
																				    Device_State_Data.set_position=Device_State_Data.Motor_Position+10;
																				 	  if(Device_State_Data.Motor_Position>100)
																				    Device_State_Data.Motor_Position=100;
																		   	  	Run_to_position();	
																				break;
																				case clothesHangerDown6:      //晾衣架B低一点下降   

																				Device_State_Data.B_set_position=Device_State_Data.B_Motor_Position+10;
																				Run_to_position_B();	
																				break;
																				case clothesHangerDown7:        //晾衣架一半   

																				Device_State_Data.set_position=Device_State_Data.set_position_all;
																				Run_to_position();	
																				Device_State_Data.B_set_position=Device_State_Data.set_position_all;
																				Run_to_position_B();	

																				break;
																				case clothesHangerDown8:       //晾衣架A一半   

																				Device_State_Data.set_position=30;
																				Run_to_position();	

																				break;
																				case clothesHangerDown9:       //晾衣架B一半   

																				Device_State_Data.set_position=Device_State_Data.set_position_all;
																				Run_to_position();	

																				break;
																						}	

					}	break;
					
		       	case ASR_MOTOR_STOP:  //电机暂停
					  {       
							switch(Action)//电机暂停命令
							{
								case clothesHangerStop1:     // 暂停
							{
								Cmd_Motor_Stop();
								//r_uart1_send_bytes(STOP_asr,7);
							}            
							break;
							case clothesHangerStop2:     //暂停
							{
									//r_uart1_send_bytes(STOP_asr,7);
								   Cmd_Motor_Stop_B();
							}            
							break;
              		case clothesHangerStop3:     //暂停
							{
								Cmd_Motor_Stop();
								Cmd_Motor_Stop_B();
							//	r_uart1_send_bytes(STOP_asr,7);
							}            
							break;

							}
										
				 }	break;
 
				    case ASR_LIGHT://灯光命令
				    {		
									switch(Action)
									{										
										case 0x01:	//开灯	
											
											if (Device_State_Data.Light_State_Byte == LIGHT_OFF)
											{
														Time.Light_key_space_time=Light_KEY_SPACE_TIME;
														Cmd_Light();
											}		
									//	r_uart1_send_bytes(Open_light_asr,7);		
										break;	
										case 0x02:  //打开白灯	
													
													if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
													{
															Device_State_Data.Light_colour_temperature=5700;
															Device_State_Data.Light_State_Byte=LIGHT_ON;
															SetupBzhx(LIGHT_ON_BEEP,Flash_Data.Beep_open);
															colour_temperature_temp=(Device_State_Data.Light_colour_temperature-3000)/5;//步长5  0~LIGHT_MAX_STEP	
															Flash_Data.Light_CT_value=Device_State_Data.Light_colour_temperature;
													}
													else
													{
											

													}	
											//		r_uart1_send_bytes(Open_light_asr,7);	
										break;		
                    case 0x03:  //打开黄色灯
													
/*														if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
														{
																		Device_State_Data.Light_colour_temperature=3000;
																		Device_State_Data.Light_State_Byte=LIGHT_ON;
																		SetupBzhx(LIGHT_ON_BEEP,Flash_Data.Beep_open);
																		colour_temperature_temp=(Device_State_Data.Light_colour_temperature-3000)/5;//步长5  0~LIGHT_MAX_STEP	
																		Flash_Data.Light_CT_value=Device_State_Data.Light_colour_temperature;
														}
														else
														{
															
															
																
														}	
													
														r_uart1_send_bytes(Open_light_asr,7);	
										*/
										break;		  

                    case 0x04:  //打开冷色	
														
											/*				if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
															{
															      Device_State_Data.Light_State_Byte=LIGHT_ON;
														      	SetupBzhx(LIGHT_ON_BEEP,Flash_Data.Beep_open);
																	  colour_temperature_temp=(Device_State_Data.Light_colour_temperature-3000)/5;//步长5  0~LIGHT_MAX_STEP	
															}
															else
															{
														      	Device_State_Data.Light_colour_temperature=4000;
															}	
														
														  	Flash_Data.Light_CT_value=Device_State_Data.Light_colour_temperature;
												r_uart1_send_bytes(Open_light_asr,7);	
										*/
										break;	
	
									}
						} break;
						case ASR_UPPER_LIMIT://上限位命令
					  {		
						switch(Action)
						{										
							case 0x01:	//衣架A上限位
								
									/*			if(Ell_Data.Motor_Current_Position<=UP_POSITION_VALUE)
														{
															Device_State_Data.electronic_up_limit=0;
														}
														else
														{	
															Device_State_Data.electronic_up_limit=Device_State_Data.Motor_Position;
														}
														Flash_Data.Electronic_Up_Limit=Device_State_Data.electronic_up_limit;	
														if(Flash_Data.Electronic_Up_Limit==0)
														{
															set_up_limit_value=0;//电子上限位对应霍尔脉冲数
														}
														else
														{
															set_up_limit_value=Flash_Data.Electronic_Up_Limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;
														}
														if(Ell_Data.Motor_Current_Position<set_up_limit_value)
														{
															Device_State_Data.Motor_State_Byte=MOTOR_UP_STOP_STATE;
														}									
							    	 r_uart1_send_bytes(upper_limit_set1,7);
										 */
							break;	
							case 0x02:			//衣架B上限位	
						/* if(Ell_Data.B_Motor_Current_Position<=UP_POSITION_VALUE)
								{
									Device_State_Data.B_electronic_up_limit=0;
								}
								else
								{	
									Device_State_Data.B_electronic_up_limit=Device_State_Data.B_Motor_Position;
								}
								Flash_Data.B_electronic_up_limit=Device_State_Data.B_electronic_up_limit;	
								if(Flash_Data.B_electronic_up_limit==0)
								{
									B_set_up_limit_value=0;//电子上限位对应霍尔脉冲数
								}
								else
								{
									B_set_up_limit_value=Flash_Data.B_electronic_up_limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;
								}
								if(Ell_Data.B_Motor_Current_Position<B_set_up_limit_value)
								{
									Device_State_Data.B_Motor_State_Byte=MOTOR_UP_STOP_STATE;
								}	
					    	r_uart1_send_bytes(upper_limit_set2,7);
						*/	break;			
							case 0x03:		//取消衣架A上限位		
								
						//	r_uart1_send_bytes(uart1_rx_buf,7);
							break;	
							case 0x04:		//取消衣架B上限位	
								
						//	r_uart1_send_bytes(uart1_rx_buf,7);
							break;			
						}
					} break;	
						case ASR_DOWN_LIMIT://下限位命令
						/*	{		
								switch(Action)
								{										
								case 0x01:			//衣架A下限位
							if(Ell_Data.Motor_Current_Position>=DOWN_POSITION_VALUE)
							{
								
							  Device_State_Data.electronic_down_limit=100;
								
							}
							else
							{
								
						  	Device_State_Data.electronic_down_limit=Device_State_Data.Motor_Position;
								
							}
							
							Flash_Data.Electronic_Down_Limit=Device_State_Data.electronic_down_limit;
							
							set_down_limit_value=Flash_Data.Electronic_Down_Limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;//电子下限位对应霍尔脉冲数		
							Device_State_Data.Motor_State_Byte=MOTOR_DOWN_STOP_STATE;	
							
						  	r_uart1_send_bytes(Down_limit_set1,7);
									*/
								break;	
								case 0x02:		//衣架B下限位
				/*			if(Ell_Data.B_Motor_Current_Position>=DOWN_POSITION_VALUE)
							{
							  Device_State_Data.B_electronic_down_limit=100;
							}
							else
							{
							  Device_State_Data.B_electronic_down_limit=Device_State_Data.B_Motor_Position;
							}
							Flash_Data.B_electronic_down_limit=Device_State_Data.B_electronic_down_limit;
							
							B_set_down_limit_value=Flash_Data.B_electronic_down_limit*ONE_PERCENT_PULSE+UP_POSITION_VALUE;//电子下限位对应霍尔脉冲数	
							
							Device_State_Data.B_Motor_State_Byte=MOTOR_DOWN_STOP_STATE;	
							
							r_uart1_send_bytes(Down_limit_set2,7);
								
								break;	
								case 0x03:		//取消衣架B下限位

								break;
								case 0x04:		//取消衣架B下限位

								break;	
								}
							
						} 	*/break;	

            case ASR_COLOR_TEMPERATURE_SET://色温
						{	
							
							switch(Action)
						{										
							case 0x01:	//切白光
						            
									/*						if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
													{
													
													}
													else
													{
											   	     Device_State_Data.Light_colour_temperature=5700;    //5700  白色 3000 黄色  4000冷色
															SetupBzhx(LIGHT_ON_BEEP,Flash_Data.Beep_open);
														
													}	
														r_uart1_send_bytes(Open_light_asr,7);	
								*/
							
							break;
							case 0x02:	//切黄光
							
	/*	if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
													{
													
													}
													else
													{
												      Device_State_Data.Light_colour_temperature=3000;    //5700  白色 3000 黄色  4000冷色
															SetupBzhx(LIGHT_ON_BEEP,Flash_Data.Beep_open);
													}	
										           r_uart1_send_bytes(Open_light_asr,7);	
							break;
							case 0x03:	//切冷光
										   				if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
													{
													
													}
													else
													{
															 SetupBzhx(LIGHT_ON_BEEP,Flash_Data.Beep_open);
												       Device_State_Data.Light_colour_temperature=4000;    //5700  白色 3000 黄色  4000冷色
													}	
											r_uart1_send_bytes(Open_light_asr,7);	
											*/
							break;
						}


            }break;	
	          case ASR_MOONLIGHT://夜灯和关灯
						{	
	                 switch(Action)
									{										
													case 0x01:	//夜灯	打开
																	Device_State_Data.nightlight_switch=LIGHT_ON;
													   //     r_uart1_send_bytes(Open_Nightlight_asr,7);	
																	Flag.Brightness_time_flag=1;
													
													break;
													case 0x02:	//夜灯关闭	
																if(Device_State_Data.Light_State_Byte==LIGHT_ON)
															{
																		Device_State_Data.Light_State_Byte=LIGHT_OFF;
																		SetupBzhx(LIGHT_ON_BEEP,Flash_Data.Beep_open);
															}
													  //       r_uart1_send_bytes(Close_light_asr,7);	
													break;
									}
            }break;	
						case ASR_LIGHT_SET://亮度 命令   
            switch(Action)
									{				
                      	case 0x02:	
                          
														if(Device_State_Data.Light_Bright>=100)
														{
														//     r_uart1_send_bytes(MAX_LIGHT_asr,7);		
																 Device_State_Data.Light_Bright=100;
														}else
														{
														
														    Device_State_Data.Light_Bright=Device_State_Data.Light_Bright+5;
                              //  r_uart1_send_bytes(LIGHT_SET_asr,7);		
														
														}
													
															if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
															{
																	Device_State_Data.Light_State_Byte=LIGHT_ON;
																	SetupBzhx(LIGHT_ON_BEEP,Flash_Data.Beep_open);
															}
															else
															{
														 
																
																
															}	
																Flash_Data.Max_Light_Value=Device_State_Data.Light_Bright;
													break;										
													  case 0x01:	
												    
																if(Device_State_Data.Light_Bright<=5)
																{
																
																		Device_State_Data.Light_Bright=5;
																//		r_uart1_send_bytes(	MAX_LIGHT_asr
																		//		 ,7);		
																}else{
																
																	   Device_State_Data.Light_Bright=Device_State_Data.Light_Bright-5;
															
																//	   r_uart1_send_bytes(LIGHT_SET_asr,7);		
																}			 
														
															if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
															{
															  Device_State_Data.Light_State_Byte=LIGHT_ON;
														  	SetupBzhx(LIGHT_ON_BEEP,Flash_Data.Beep_open);
															}
															else
															{
														 
																
																
															}	
															   Flash_Data.Max_Light_Value=Device_State_Data.Light_Bright;
															
													break;	
															
															 case 0x04:	
																
																	   Device_State_Data.Light_Bright=10;
															
																	//   r_uart1_send_bytes(LIGHT_SET_asr,7);		
																		 
														
															if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
															{
															  Device_State_Data.Light_State_Byte=LIGHT_ON;
														  	SetupBzhx(LIGHT_ON_BEEP,Flash_Data.Beep_open);
															}
															else
															{
														 
																
																
															}	
															   Flash_Data.Max_Light_Value=Device_State_Data.Light_Bright;
															
													break;	
															
															case 0x03:	
																
																	   Device_State_Data.Light_Bright=100;
																//	   r_uart1_send_bytes(LIGHT_SET_asr,7);		
															if(Device_State_Data.Light_State_Byte==LIGHT_OFF)
															{
															  Device_State_Data.Light_State_Byte=LIGHT_ON;
														  	SetupBzhx(LIGHT_ON_BEEP,Flash_Data.Beep_open);
															}
															else
															{
														 
																
																
															}	
															   Flash_Data.Max_Light_Value=Device_State_Data.Light_Bright;
													break;	
															
															
															
															
														}
						break;
	          default:                    break;
		}
	}

}
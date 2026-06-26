#ifndef __433_TX_H
#define __433_TX_H

#include <stdint.h>
#include <stdbool.h>


// 433发射模块控制IO（推挽输出）
#define TX_PIN_PORT    GpioPortD
#define TX_PIN_PIN     GpioPin0
#define TX_PIN_HIGH()  Gpio_SetIO(TX_PIN_PORT, TX_PIN_PIN)
#define TX_PIN_LOW()   Gpio_ClrIO(TX_PIN_PORT, TX_PIN_PIN)

// 微秒级延时函数（必须由硬件实现，如SysTick、定时器延时）
// 注意：延时精度直接影响发码正确性，误差需控制在±10us内

#define DELAY_US(x)    delay10us(x)

// -------------------------- 协议时序宏定义（严格按协议） --------------------------
#define T0H_US         40U    // 逻辑0 高电平时间 0.4ms = 400us
#define T0L_US         120U   // 逻辑0 低电平时间 1.2ms = 1200us
#define T1H_US         120U   // 逻辑1 高电平时间 1.2ms = 1200us
#define T1L_US         40U    // 逻辑1 低电平时间 0.4ms = 400us
#define STOP_H_US      40U    // 结束位 高电平时间 0.4ms = 400us
#define STOP_L_US      40U    // 结束位 低电平时间 0.4ms = 400us
#define FRAME_INTERVAL 1200U  // 帧间隔时间 12ms = 12000us

// -------------------------- 数据结构定义 --------------------------
typedef struct {
    uint8_t byte0;  // 滚动地址码BYTE0
    uint8_t byte1;  // 滚动地址码BYTE1
    uint8_t byte2;  // 按键码BYTE2
} RemoteCode_t;

// -------------------------- 函数声明 --------------------------
void Remote433_TxInit(void);
void Remote433_SendSingleFrame(const RemoteCode_t *code);
void Remote433_SendKey(uint8_t key_value, uint8_t repeat_cnt);

#endif // __433_REMOTE_TX_H
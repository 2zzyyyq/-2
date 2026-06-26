#include "433_tx.h"
#include "ALL_Var.h"
// -------------------------- 内部函数声明 --------------------------
static void Remote433_SendBit(uint8_t bit_val);
static void Remote433_SendStopBit(void);

 

// -------------------------- 初始化函数 --------------------------
void Remote433_TxInit(void)
{
  
 
	   stc_gpio_cfg_t stcGpioCfg;
    
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    ///< 端口方向配置->输入
    stcGpioCfg.enDir = GpioDirOut;

	///< 端口上下拉配置->上拉
    stcGpioCfg.enPu = GpioPuEnable;
    stcGpioCfg.enPd = GpioPdDisable;
    ///< 端口开漏输出配置->开漏输出关闭
    stcGpioCfg.enOD = GpioOdDisable;
    ///< 端口输入/输出值寄存器总线控制模式配置->AHB
    stcGpioCfg.enCtrlMode = GpioAHB;
 
	
		Gpio_Init(TX_PIN_PORT, TX_PIN_PIN, &stcGpioCfg); 
		
		
    TX_PIN_LOW(); // 初始状态拉低，避免误发射
}

// -------------------------- 发送单个bit（核心时序控制） --------------------------
static void Remote433_SendBit(uint8_t bit_val)
{
    if (bit_val) {
        // 逻辑1：高1.2ms + 低0.4ms
        TX_PIN_HIGH();
        DELAY_US(T1H_US);
        TX_PIN_LOW();
        DELAY_US(T1L_US);
    } else {
        // 逻辑0：高0.4ms + 低1.2ms
        TX_PIN_HIGH();
        DELAY_US(T0H_US);
        TX_PIN_LOW();
        DELAY_US(T0L_US);
    }
}

// -------------------------- 发送结束位 --------------------------
static void Remote433_SendStopBit(void)
{
    TX_PIN_HIGH();
    DELAY_US(STOP_H_US);
    TX_PIN_LOW();
    DELAY_US(STOP_L_US);
}

// -------------------------- 发送单帧完整数据 --------------------------
void Remote433_SendSingleFrame(const RemoteCode_t *code)
{
    if (code == NULL) return;
    
    uint8_t i;
    uint8_t data_byte;
    
    // 1. 发送BYTE0（地址码，高位在前/低位在前？按协议波形默认高位在前，可根据实际修改）
    data_byte = code->byte0;
    for (i = 0; i < 8; i++) {
        // 从最高位到最低位依次发送
        Remote433_SendBit((data_byte >> (7 - i)) & 0x01);
    }
    
    // 2. 发送BYTE1（地址码）
    data_byte = code->byte1;
    for (i = 0; i < 8; i++) {
        Remote433_SendBit((data_byte >> (7 - i)) & 0x01);
    }
    
    // 3. 发送BYTE2（按键码）
    data_byte = code->byte2;
    for (i = 0; i < 8; i++) {
        Remote433_SendBit((data_byte >> (7 - i)) & 0x01);
    }
    
    // 4. 发送结束位
    Remote433_SendStopBit();
    
    // 5. 帧间隔延时（保持低电平12ms）
    DELAY_US(FRAME_INTERVAL);
}

// -------------------------- 发送按键（带重复发送，适配遥控器习惯） --------------------------
void Remote433_SendKey(uint8_t key_value, uint8_t repeat_cnt)
{
    RemoteCode_t code;
    
    // 示例：地址码固定为0x20, 0x03（对应你提供的K1键示例）
    // 实际使用中，BYTE0/BYTE1需按滚动码算法更新，这里先固定演示
    code.byte0 = 0x20;
    code.byte1 = 0x03;
    code.byte2 = key_value; // 按键码，如K1=0x01
    
    // 重复发送指定次数（遥控器通常连发3-5次，提高接收成功率）
    for (uint8_t i = 0; i < repeat_cnt; i++) {
        Remote433_SendSingleFrame(&code);
    }
}
#include "ALL_Var.h"
void App_LedInit(void);
void App_UserKeyInit(void); 
void GPIOA_ExtiConfig(uint8_t pin, uint8_t edge_type);
void App_UserKeyInit(void)
{
    stc_gpio_cfg_t stcGpioCfg;
    
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    ///< 端口方向配置->输入
    stcGpioCfg.enDir = GpioDirIn;

    stcGpioCfg.enDrv = GpioDrvL;
	///< 端口上下拉配置->上拉
    stcGpioCfg.enPu = GpioPuEnable;
    stcGpioCfg.enPd = GpioPdDisable;
    ///< 端口开漏输出配置->开漏输出关闭
    stcGpioCfg.enOD = GpioOdDisable;
    ///< 端口输入/输出值寄存器总线控制模式配置->AHB
    stcGpioCfg.enCtrlMode = GpioAHB;
    ///< GPIO IO USER KEY初始化
	
		Gpio_Init(GpioPortB, GpioPin6, &stcGpioCfg); 
		Gpio_Init(GpioPortB, GpioPin7, &stcGpioCfg); 

	
		Gpio_Init(GpioPortB, GpioPin4, &stcGpioCfg); 
		Gpio_Init(GpioPortB, GpioPin5, &stcGpioCfg); 
	
		Gpio_Init(GpioPortC, GpioPin15, &stcGpioCfg); 
	
		Gpio_Init(GpioPortB, GpioPin12, &stcGpioCfg); 
		Gpio_Init(GpioPortB, GpioPin2, &stcGpioCfg); 
    
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    ///< 端口方向配置->输入
    stcGpioCfg.enDir = GpioDirIn;

    stcGpioCfg.enDrv = GpioDrvL;
	///< 端口上下拉配置-> 无
    stcGpioCfg.enPu = GpioPuDisable;
    stcGpioCfg.enPd = GpioPdDisable;
    ///< 端口开漏输出配置->开漏输出关闭
    stcGpioCfg.enOD = GpioOdDisable;
    ///< 端口输入/输出值寄存器总线控制模式配置->AHB
    stcGpioCfg.enCtrlMode = GpioAHB;

		Gpio_Init(HOLL_DIR_PORT, HOLL_DIR_PIN, &stcGpioCfg); 
		Gpio_Init(HOLL_NUM_PORT, HOLL_NUM_PIN, &stcGpioCfg); 

		Gpio_Init(B_HOLL_NUM_PORT, B_HOLL_NUM_PIN, &stcGpioCfg); 
		Gpio_Init(B_HOLL_DIR_PORT, B_HOLL_DIR_PIN, &stcGpioCfg);

		Gpio_EnableIrq(HOLL_NUM_PORT, HOLL_NUM_PIN, GpioIrqFalling);
		Gpio_EnableIrq(B_HOLL_NUM_PORT, B_HOLL_NUM_PIN, GpioIrqFalling);

		Gpio_EnableIrq(HOLL_NUM_PORT, HOLL_NUM_PIN, GpioIrqRising);
		Gpio_EnableIrq(B_HOLL_NUM_PORT, B_HOLL_NUM_PIN, GpioIrqRising);

		Gpio_EnableIrq(GpioPortB, GpioPin12, GpioIrqFalling);
		Gpio_EnableIrq(GpioPortB, GpioPin12, GpioIrqRising);

		EnableNvic(PORTA_IRQn, IrqLevel1, TRUE);
		EnableNvic(PORTB_IRQn, IrqLevel1, TRUE);
		EnableNvic(PORTC_IRQn, IrqLevel1, TRUE);
 
}

void App_LedInit(void)
{
				stc_gpio_cfg_t stcGpioCfg;

				///< 打开GPIO外设时钟门控
				Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 

				///< 端口方向配置->输出(其它参数与以上（输入）配置参数一致)
				stcGpioCfg.enDir = GpioDirOut;
				///< 端口上下拉配置->下拉
				stcGpioCfg.enPu = GpioPuDisable;
				stcGpioCfg.enPd = GpioPdEnable;


				///< GPIO IO LED端口初始化
				Gpio_Init(GpioPortB, GpioPin1, &stcGpioCfg);
				Gpio_Init(GpioPortB, GpioPin3, &stcGpioCfg);
				Gpio_Init(GpioPortA, GpioPin15, &stcGpioCfg);

				Gpio_Init(GpioPortD, GpioPin6, &stcGpioCfg);
				Gpio_Init(GpioPortD, GpioPin7, &stcGpioCfg);

				Gpio_Init(GpioPortB, GpioPin13, &stcGpioCfg); 
				Gpio_Init(GpioPortC, GpioPin13, &stcGpioCfg);
			
  
       	Gpio_Init(GpioPortB, GpioPin10, &stcGpioCfg); 
				Gpio_Init(GpioPortB, GpioPin11, &stcGpioCfg);
				
				Gpio_ClrIO(GpioPortB, GpioPin1);
				Gpio_ClrIO(GpioPortB, GpioPin3);
				Gpio_ClrIO(GpioPortB, GpioPin10); 
				Gpio_ClrIO(GpioPortB, GpioPin11);
				Gpio_ClrIO(GpioPortB, GpioPin13);
				Gpio_ClrIO(GpioPortD, GpioPin6);
				Gpio_ClrIO(GpioPortD, GpioPin7);
		   	Gpio_ClrIO(GpioPortC, GpioPin13);
				Gpio_ClrIO(GpioPortA, GpioPin15);
		
}

// 端口A中断配置函数
void GPIOA_ExtiConfig(uint8_t pin, uint8_t edge_type)
{
    // 1. 配置引脚为数字端口 (PAADS[pin]=0)
    M0P_GPIO->PAADS &= ~(1 << pin);

    // 2. 配置引脚为输入 (PADIR[pin]=1)
    M0P_GPIO->PADIR |= (1 << pin);

    // 3. 配置中断触发方式
    switch(edge_type) {
    case 0: // 上升沿触发
        M0P_GPIO->PARIE |= (1 << pin);  // 使能上升沿中断
        M0P_GPIO->PAFIE &= ~(1 << pin); // 禁用下降沿中断
        break;
    case 1: // 下降沿触发
        M0P_GPIO->PAFIE |= (1 << pin);  // 使能下降沿中断
        M0P_GPIO->PARIE &= ~(1 << pin); // 禁用上升沿中断
        break;
    case 2: // 双边沿触发
        M0P_GPIO->PARIE |= (1 << pin);  // 使能上升沿中断
        M0P_GPIO->PAFIE |= (1 << pin);  // 使能下降沿中断
        break;
    default:
        // 默认配置为双边沿
        M0P_GPIO->PARIE |= (1 << pin);
        M0P_GPIO->PAFIE |= (1 << pin);
    }

}

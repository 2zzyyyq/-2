#include "ALL_Var.h"
//串口引脚配置
uint8_t  send_busy_asr;
void App_portCfg(void)
{
    stc_gpio_cfg_t stcGpioCfg;

    DDL_ZERO_STRUCT(stcGpioCfg);

    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE); //使能GPIO模块时钟
    ///<TX
		stcGpioCfg.enDir		= GpioDirOut;
		stcGpioCfg.enPu			= GpioPuEnable;
		stcGpioCfg.enCtrlMode	= GpioAHB;
		stcGpioCfg.enDrv		= GpioDrvH;
		Gpio_Init(GpioPortB, GpioPin8, &stcGpioCfg);
		Gpio_SetAfMode(GpioPortB, GpioPin8, GpioAf7);        

		///<RX
		stcGpioCfg.enDir = GpioDirIn;
		stcGpioCfg.enDrv		= GpioDrvL;

		Gpio_Init(GpioPortB, GpioPin9, &stcGpioCfg);
		Gpio_SetAfMode(GpioPortB, GpioPin9, GpioAf7);          
}

//串口配置
void App_UartCfg(void)
{
		App_portCfg();                        //串口引脚配置  115200
	  r_uart0_protocol_init();              //wifi数组初始化  将头和尾指针指向实体
    stc_uart_cfg_t    stcCfg;

    DDL_ZERO_STRUCT(stcCfg);

    ///< 开启外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralUart0,TRUE);///<使能uart1模块时钟

    ///<UART Init
    stcCfg.enRunMode        = UartMskMode1;          ///<模式1
    stcCfg.enStopBit        = UartMsk1bit;           ///<1bit停止位
    stcCfg.enMmdorCk        = UartMskDataOrAddr;           ///<偶检验
    stcCfg.stcBaud.u32Baud  = 115200;                  ///<波特率115200
    stcCfg.stcBaud.enClkDiv = UartMsk8Or16Div;       ///<通道采样分频配置
    stcCfg.stcBaud.u32Pclk  = Sysctrl_GetPClkFreq(); ///<获得外设时钟（PCLK）频率值
    Uart_Init(M0P_UART0, &stcCfg);                   ///<串口初始化

    ///<UART中断使能
    Uart_ClrStatus(M0P_UART0,UartRC);                ///<清接收请求
    Uart_ClrStatus(M0P_UART0,UartTC);                ///<清接收请求
    Uart_EnableIrq(M0P_UART0,UartRxIrq);             ///<使能串口接收中断
    Uart_EnableIrq(M0P_UART0,UartTxIrq);             ///<使能串口接收中断
    EnableNvic(UART0_IRQn, IrqLevel1, TRUE);       ///<系统中断使能

}


///< LPUART配置
void App_LpUartCfg(void)
{
		 App_LpUartPortCfg();

		r_uart1_protocol_init();              //语音数组初始化  将头和尾指针指向实体
    stc_lpuart_cfg_t  stcCfg;

    DDL_ZERO_STRUCT(stcCfg);
    
    ///<外设模块时钟使能
    Sysctrl_SetPeripheralGate(SysctrlPeripheralLpUart1,TRUE);    
    
    ///<LPUART 初始化
    stcCfg.enStopBit = LPUart1bit;                   ///<1停止位    
    stcCfg.enMmdorCk = LPUartEven;                   ///<偶校验
    stcCfg.stcBaud.enSclkSel = LPUartMskPclk;        ///<传输时钟源
    stcCfg.stcBaud.u32Sclk = Sysctrl_GetPClkFreq();  ///<PCLK获取
    stcCfg.stcBaud.enSclkDiv = LPUartMsk4Or8Div;     ///<采样分频
    stcCfg.stcBaud.u32Baud = 115200;                   ///<波特率
    stcCfg.enRunMode = LPUartMskMode1;               ///<工作模式
    LPUart_Init(M0P_LPUART1, &stcCfg);
     
    ///<LPUART 中断使能
    LPUart_ClrStatus(M0P_LPUART1,LPUartRC);          ///<清接收中断请求
    LPUart_ClrStatus(M0P_LPUART1,LPUartTC);          ///<清发送中断请求
    LPUart_EnableIrq(M0P_LPUART1,LPUartRxIrq);      ///<禁止接收中断
    LPUart_EnableIrq(M0P_LPUART1,LPUartTxIrq);       ///<使能发送中断
    EnableNvic(LPUART1_IRQn,IrqLevel3,TRUE);         ///<系统中断使能

}

///< 端口配置
void App_LpUartPortCfg(void)
{
    stc_gpio_cfg_t stcGpioCfg;
    
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
    
    ///<TX
    stcGpioCfg.enDir =  GpioDirOut;
    Gpio_Init(GpioPortA,GpioPin0,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin0,GpioAf2); //配置PA00为LPUART1_TX
    
    //<RX
    stcGpioCfg.enDir =  GpioDirIn;
    Gpio_Init(GpioPortA,GpioPin1,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin1,GpioAf2); //配置PA01为LPUART1_RX   
}

































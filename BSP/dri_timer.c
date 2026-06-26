#include "ALL_Var.h"

//PB00	TIM5_CHB	BUZ_PWM	蜂鸣器
//PB01		BUZ_PWR
void App_AdvTimerInit(uint16_t u16Period,  uint16_t u16CHB_PWMDuty);
void App_AdvTimerPortInit(void);
void App_Timer0PortCfg(void);
void App_Timer0Cfg(uint16_t u16Period);

///< AdvTimer端口初始化
void App_AdvTimerPortInit(void)
{
    stc_gpio_cfg_t         stcTIM5Port;
    
    DDL_ZERO_STRUCT(stcTIM5Port);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //端口外设时钟使能
    
    stcTIM5Port.enDir  = GpioDirOut;
   
    Gpio_Init(GpioPortB, GpioPin0, &stcTIM5Port);
    Gpio_SetAfMode(GpioPortB,GpioPin0,GpioAf4);
  
}


///< AdvTimer初始化
void App_AdvTimerInit(uint16_t u16Period,  uint16_t u16CHB_PWMDuty)
{
    App_AdvTimerPortInit();               //蜂鸣器定时器初始化
    en_adt_compare_t          enAdtCompareB;

    stc_adt_basecnt_cfg_t     stcAdtBaseCntCfg;
  
    stc_adt_CHxX_port_cfg_t   stcAdtTIM5BCfg;
    
    
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtTIM5BCfg);
  

    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE);    //ADT外设时钟使能
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;                 //锯齿波模式
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0Div2;
    
    Adt_Init(M0P_ADTIM5, &stcAdtBaseCntCfg);                      //ADT载波、计数模式、时钟配置


    Adt_SetPeriod(M0P_ADTIM5, u16Period);                         //周期设置
  
    enAdtCompareB = AdtCompareB;
    Adt_SetCompareValue(M0P_ADTIM5, enAdtCompareB, u16CHB_PWMDuty); 
    stcAdtTIM5BCfg.enCap = AdtCHxCompareOutput;            //比较输出
    stcAdtTIM5BCfg.bOutEn = TRUE;                          //CHB输出使能
    stcAdtTIM5BCfg.enPerc = AdtCHxPeriodLow;               //计数值与周期匹配时CHB电平保持不变
    stcAdtTIM5BCfg.enCmpc = AdtCHxCompareInv;             //计数值与比较值A匹配时，CHB电平翻转
    stcAdtTIM5BCfg.enStaStp = AdtCHxStateSelSS;            //CHB起始结束电平由STACB与STPCB控制
    stcAdtTIM5BCfg.enStaOut = AdtCHxPortOutLow;            //CHB起始电平为低
    stcAdtTIM5BCfg.enStpOut = AdtCHxPortOutLow;            //CHB结束电平为低
    Adt_CHxXPortCfg(M0P_ADTIM5, AdtCHxB, &stcAdtTIM5BCfg);   //端口CHB配置
   // Adt_StartCount(M0P_ADTIM5); //AdvTimer5运行
}					 
			
void App_MORInit(void)
{
    stc_gpio_cfg_t stcGpioCfg;
    
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    ///< 端口方向配置->输入
    stcGpioCfg.enDir = GpioDirIn;
    ///< 端口驱动能力配置->高驱动能力
    stcGpioCfg.enDrv = GpioDrvL;
    ///< 端口上下拉配置->无
    stcGpioCfg.enPu = GpioPuEnable;
    stcGpioCfg.enPd = GpioPdDisable;
    ///< 端口开漏输出配置->开漏输出关闭
    stcGpioCfg.enOD = GpioOdDisable;
    ///< 端口输入/输出值寄存器总线控制模式配置->AHB
    stcGpioCfg.enCtrlMode = GpioAHB;
    ///< GPIO IO USER KEY初始化
    Gpio_Init(GpioPortB, GpioPin6, &stcGpioCfg); 

    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    
    ///< 端口方向配置->输出(其它参数与以上（输入）配置参数一致)
    stcGpioCfg.enDir = GpioDirOut;
    ///< 端口上下拉配置->下拉
    stcGpioCfg.enPu = GpioPuDisable;
    stcGpioCfg.enPd = GpioPdEnable;

    Gpio_ClrIO(GpioPortD, GpioPin7);

    ///< GPIO IO LED端口初始化
    Gpio_Init(GpioPortD, GpioPin7, &stcGpioCfg);

	
    ///< 打开并配置USER KEY为下降沿中断
  //   Gpio_EnableIrq(STK_USER_PORT, STK_USER_PIN, GpioIrqFalling);
	//	 EnableNvic(PORTA_IRQn, IrqLevel3, TRUE);
}


/*******************************************************************************
 * TIM0中断服务函数
 ******************************************************************************/

//端口配置初始化
void App_TimermotorPortCfg(void);
void App_TimermotorCfg(uint16_t u16Period, uint16_t u16CHxACompare, uint16_t u16CHxBCompare);
//端口配置初始化
void App_TimermotorPortCfg(void)
{
	    	App_MORInit();                        //电机使能 
	
				LED_LIGHT_OFF;
	
	#ifdef  NO_PWM_MOTOR													
	      stc_gpio_cfg_t stcGpioCfg;

				///< 打开GPIO外设时钟门控
				Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 

				///< 端口方向配置->输出(其它参数与以上（输入）配置参数一致)
				stcGpioCfg.enDir = GpioDirOut;
				///< 端口上下拉配置->下拉
				stcGpioCfg.enPu = GpioPuDisable;
				stcGpioCfg.enPd = GpioPdEnable;
				///< GPIO IO LED端口初始化
				Gpio_Init(GpioPortB, GpioPin15, &stcGpioCfg);
				Gpio_Init(GpioPortB, GpioPin14, &stcGpioCfg);
	
				Gpio_ClrIO(GpioPortB, GpioPin14);
				Gpio_ClrIO(GpioPortB, GpioPin15);
	#else  
  	App_TimermotorCfg(3200,0,0);    //  电机PWM 32MH/3200=10kz  
		
    stc_gpio_cfg_t             stcTIM0Port;
    //结构体初始化清零
    DDL_ZERO_STRUCT(stcTIM0Port);

    stcTIM0Port.enDir  = GpioDirOut;
    
    Gpio_Init(GpioPortB, GpioPin14, &stcTIM0Port);
		
    Gpio_SetAfMode(GpioPortB,GpioPin14,GpioAf4);            //PB14设置为TIM0_CHA
    
    Gpio_Init(GpioPortB, GpioPin15, &stcTIM0Port);
	
    Gpio_SetAfMode(GpioPortB,GpioPin15,GpioAf3);            //PA15设置为TIM0_CHB
   
	#endif
       
}


void App_TimermotorCfg(uint16_t u16Period, uint16_t u16CHxACompare, uint16_t u16CHxBCompare)
{
	#ifdef  NO_PWM_MOTOR			
   
	#else 
	  uint16_t                   u16CntValue;
  
    stc_bt_mode23_cfg_t        stcBtBaseCfg;
    stc_bt_m23_compare_cfg_t   stcBtPortCmpCfg;
   
    //结构体初始化清零
    DDL_ZERO_STRUCT(stcBtBaseCfg);
    DDL_ZERO_STRUCT(stcBtPortCmpCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBaseTim, TRUE);   //Base Timer外设时钟使能
        
    stcBtBaseCfg.enWorkMode    = BtWorkMode2;             
    stcBtBaseCfg.enCT          = BtTimer;                  //定时器功能，计数时钟为内部PCLK
    stcBtBaseCfg.enPRS         = BtPCLKDiv1;               //PCLK
    stcBtBaseCfg.enCntDir     =  BtCntUp;                  //向上计数，在三角波模式时只读
    stcBtBaseCfg.enPWMTypeSel  = BtIndependentPWM;         //独立输出PWM     //BtComplementaryPWM;
    stcBtBaseCfg.enPWM2sSel    = BtSinglePointCmp;         //单点比较功能
    stcBtBaseCfg.bOneShot      = FALSE;                    //循环计数
    stcBtBaseCfg.bURSSel       = FALSE;                    //上下溢更新
    
    Bt_Mode23_Init(TIM0, &stcBtBaseCfg);                   //TIM0 的模式23功能初始化
    
    Bt_M23_ARRSet(TIM0, u16Period, TRUE);                  //设置重载值,并使能缓存
    
    Bt_M23_CCR_Set(TIM0, BtCCR0A, u16CHxACompare);         //设置比较值A

    Bt_M23_CCR_Set(TIM0, BtCCR0B, u16CHxBCompare);         //设置比较值B
    
    stcBtPortCmpCfg.enCH0ACmpCtrl   = BtPWMMode1;          //OCREFA输出控制OCMA:PWM模式2
    stcBtPortCmpCfg.enCH0APolarity  = BtPortOpposite;      //正常输出
    stcBtPortCmpCfg.bCh0ACmpBufEn   = TRUE;                //A通道缓存控制
    stcBtPortCmpCfg.enCh0ACmpIntSel = BtCmpIntNone;        //A通道比较控制:无
    
    stcBtPortCmpCfg.enCH0BCmpCtrl   = BtPWMMode1;          //OCREFB输出控制OCMB:PWM模式2
    stcBtPortCmpCfg.enCH0BPolarity  = BtPortOpposite;      //正常输出
    stcBtPortCmpCfg.bCH0BCmpBufEn   = TRUE;                //B通道缓存控制使能
    stcBtPortCmpCfg.enCH0BCmpIntSel = BtCmpIntNone;        //B通道比较控制:无
    
    Bt_M23_PortOutput_Cfg(TIM0, &stcBtPortCmpCfg);         //比较输出端口配置
        
    u16CntValue = 0;
    Bt_M23_Cnt16Set(TIM0, u16CntValue);                    //设置计数初值
    
    Bt_ClearAllIntFlag(TIM0);                              //清中断标志
 

    Bt_M23_EnPWM_Output(TIM0, TRUE, FALSE);    //TIM0 端口输出使能
    
    Bt_M23_Run(TIM0);                          //TIM0 运行
	#endif
}



//Timer0配置初始化
void App_Timer0Cfg(uint16_t u16Period)
{
    
    stc_bt_mode0_cfg_t        stcBtBaseCfg;
    
    DDL_ZERO_STRUCT(stcBtBaseCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBaseTim, TRUE); //Base Timer外设时钟使能
    
    stcBtBaseCfg.enWorkMode = BtWorkMode0;                  //定时器模式
    stcBtBaseCfg.enCT       = BtTimer;                      //定时器功能，计数时钟为内部PCLK
    stcBtBaseCfg.enPRS      = BtPCLKDiv1;                   //PCLK
    stcBtBaseCfg.enCntMode  = Bt16bitArrMode;               //自动重载16位计数器/定时器
    stcBtBaseCfg.bEnTog     = FALSE;
    stcBtBaseCfg.bEnGate    = FALSE;
    stcBtBaseCfg.enGateP    = BtGatePositive;
    Bt_Mode0_Init(TIM2, &stcBtBaseCfg);                     //TIM2 的模式0功能初始化
    
 
    Bt_M0_ARRSet(TIM2, 32000);                        //设置重载值(ARR = 0x10000 - 周期)
    
 
    Bt_M0_Cnt16Set(TIM2, 0);                      //设置计数初值
    
    Bt_ClearIntFlag(TIM2,BtUevIrq);                         //清中断标志   
    Bt_Mode0_EnableIrq(TIM2);                               //使能TIM2中断(模式0时只有一个中断)
    EnableNvic(TIM2_IRQn, IrqLevel1, TRUE);                 //TIM2中断使能
		Bt_M0_Run(TIM2);  
}

//Timer3 配置
void App_TimerLEDCfg(uint16_t u16Period, uint16_t u16CHxACompare)
{
    uint16_t                     u16CntValue;
 
    stc_tim3_mode23_cfg_t        stcTim3BaseCfg;
    stc_tim3_m23_compare_cfg_t   stcTim3PortCmpCfg;
    
    //结构体初始化清零
    DDL_ZERO_STRUCT(stcTim3BaseCfg);
    DDL_ZERO_STRUCT(stcTim3PortCmpCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralTim3, TRUE);   //Timer3外设时钟使能
        
    stcTim3BaseCfg.enWorkMode    = Tim3WorkMode2;             //锯齿波模式
    stcTim3BaseCfg.enCT          = Tim3Timer;                 //定时器功能，计数时钟为内部PCLK
    stcTim3BaseCfg.enPRS         = Tim3PCLKDiv1;              //PCLK
    stcTim3BaseCfg.enCntDir      = Tim3CntUp;                 //向上计数，在三角波模式时只读
    stcTim3BaseCfg.enPWMTypeSel  = Tim3IndependentPWM;        //独立输出PWM
    stcTim3BaseCfg.enPWM2sSel    = Tim3SinglePointCmp;        //单点比较功能
    stcTim3BaseCfg.bOneShot      = FALSE;                     //循环计数
    stcTim3BaseCfg.bURSSel       = FALSE;                     //上下溢更新
    
    Tim3_Mode23_Init(&stcTim3BaseCfg);                        //TIM3 的模式23功能初始化
    
    Tim3_M23_ARRSet(u16Period, TRUE);                       //设置重载值,并使能缓存
 
    Tim3_M23_CCR_Set(Tim3CCR1A, u16CHxACompare);            //设置CH1比较值A

    stcTim3PortCmpCfg.enCHxACmpCtrl   = Tim3PWMMode2;         //OCREFA输出控制OCMA:PWM模式2
    stcTim3PortCmpCfg.enCHxAPolarity  = Tim3PortOpposite;     //正常输出
    stcTim3PortCmpCfg.bCHxACmpBufEn   = TRUE;                 //A通道缓存控制
    stcTim3PortCmpCfg.enCHxACmpIntSel = Tim3CmpIntNone;       //A通道比较中断控制:无

		Tim3_M23_PortOutput_Cfg(Tim3CH1, &stcTim3PortCmpCfg);     //比较输出端口配置


		u16CntValue = 0;

		Tim3_M23_Cnt16Set(u16CntValue);                           //设置计数初值
		Tim3_ClearAllIntFlag();                                   //清中断标志

		Tim3_M23_EnPWM_Output(TRUE, FALSE);        //端口输出使能

		Tim3_M23_Run();                            //运行。
}
void App_TimerLED2Cfg(uint16_t u16Period, uint16_t u16CHxACompare)
{

    en_adt_compare_t          enAdtCompareB;

    stc_adt_basecnt_cfg_t     stcAdtBaseCntCfg;
  
    stc_adt_CHxX_port_cfg_t   stcAdtTIM6BCfg;
    
    
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtTIM6BCfg);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE);    //ADT外设时钟使能
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;                 //锯齿波模式
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0;
    Adt_Init(M0P_ADTIM6, &stcAdtBaseCntCfg);                      //ADT载波、计数模式、AdtClkPClk0时钟配置
    Adt_SetPeriod(M0P_ADTIM6, u16Period);                         //周期设置
    enAdtCompareB = AdtCompareA;
    Adt_SetCompareValue(M0P_ADTIM6, enAdtCompareB, u16CHxACompare); 
    stcAdtTIM6BCfg.enCap = AdtCHxCompareOutput;               //比较输出
    stcAdtTIM6BCfg.bOutEn = TRUE;                            //CHB输出使能
    stcAdtTIM6BCfg.enPerc = AdtCHxPeriodHigh;                //计数值与周期匹配时CHA电平保持不变
    stcAdtTIM6BCfg.enCmpc = AdtCHxCompareInv;                //计数值与比较值A匹配时，CHA电平翻转
    stcAdtTIM6BCfg.enStaStp = AdtCHxStateSelSS;              //CHB起始结束电平由STACA与STPCA控制
    stcAdtTIM6BCfg.enStaOut = AdtCHxPortOutLow;            //CHB起始电平为低
    stcAdtTIM6BCfg.enStpOut = AdtCHxPortOutLow;            //CHB结束电平为低
    Adt_CHxXPortCfg(M0P_ADTIM6, AdtCHxA, &stcAdtTIM6BCfg);   //端口CHA配置

  
}


void App_Timer1PortCfg(void)
{
    stc_gpio_cfg_t            stcTIM1Port;
    //结构体初始化清零
    DDL_ZERO_STRUCT(stcTIM1Port);
  //  Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //端口外设时钟使能
	  SetBit((uint32_t)(&(M0P_SYSCTRL->PERI_CLKEN)), SysctrlPeripheralGpio, TRUE);
    stcTIM1Port.enDir  = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin6, &stcTIM1Port);
    Gpio_SetAfMode(GpioPortA,GpioPin6,GpioAf4);    
   	App_Timer1Cfg();                      //Timer1配置初始化  捕获设置  高电平读取
}


//433 配置
void App_Timer1Cfg(void)
{
   // uint16_t                   u16ArrValue;
  //  uint16_t                   u16CntValue;
    stc_bt_mode23_cfg_t        stcBtBaseCfg;
    stc_bt_m23_input_cfg_t     stcBtPortCapCfg;
    stc_gpio_cfg_t             stcTIM1Port;
    
    //结构体初始化清零
    DDL_ZERO_STRUCT(stcBtBaseCfg);
    DDL_ZERO_STRUCT(stcTIM1Port);
    DDL_ZERO_STRUCT(stcBtPortCapCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBaseTim, TRUE); //Base Timer外设时钟使能
    
    stcBtBaseCfg.enWorkMode    = BtWorkMode2;              //锯齿波模式
    stcBtBaseCfg.enCT          = BtTimer;                  //定时器功能，计数时钟为内部PCLK
    stcBtBaseCfg.enPRS         = BtPCLKDiv32;              //PCLK/64
    stcBtBaseCfg.enCntDir      = BtCntUp;                  //向上计数，在三角波模式时只读
    
    Bt_Mode23_Init(TIM1, &stcBtBaseCfg);                   //TIM1 的模式2功能初始化
    
    stcBtPortCapCfg.enCh0ACmpCap   = BtCHxCapMode;         //CHB通道设置为捕获模式
    stcBtPortCapCfg.enCH0ACapSel   = BtCHxCapFallRise;     //CHB通道上升沿下降沿捕获都使能
    stcBtPortCapCfg.enCH0AInFlt    = BtFltPCLKDiv16Cnt3;   //PCLK/16 3个连续有效
    stcBtPortCapCfg.enCH0APolarity = BtPortPositive;       //正常输入输出
    
    Bt_M23_PortInput_Cfg(TIM1, &stcBtPortCapCfg);          //端口输入初始化配置
    
  //  u16ArrValue = 0xFFFF;
    Bt_M23_ARRSet(TIM1, 0xFFFF, TRUE);                //设置重载值,并使能缓存
    
   // u16CntValue = 0;
    Bt_M23_Cnt16Set(TIM1, 0);                    //设置计数初值
    
    Bt_ClearAllIntFlag(TIM1);                              //清中断标志
    Bt_Mode23_EnableIrq(TIM1,BtUevIrq);                    //使能TIM0 UEV更新中断
    Bt_Mode23_EnableIrq(TIM1,BtCA0Irq);                    //使能TIM0 CB0比较/捕获中断
    EnableNvic(TIM1_IRQn, IrqLevel3, TRUE);                //TIM0中断使能
    Bt_M23_Run(TIM1);  	
}



//Timer3 Port端口配置
void App_TimerLEDPortCfg(void)
{//PB11   PB10
		 stc_gpio_cfg_t               stcTIM3Port;
		 stc_gpio_cfg_t               stcTIM6Port;
    //结构体初始化清零
     DDL_ZERO_STRUCT(stcTIM3Port);
     DDL_ZERO_STRUCT(stcTIM6Port);
     stcTIM3Port.enDir  = GpioDirOut;
     stcTIM6Port.enDir  = GpioDirOut;
     Gpio_Init(GpioPortB, GpioPin10, &stcTIM3Port);
     Gpio_SetAfMode(GpioPortB,GpioPin10,GpioAf5);               //PB10设置为TIM3_CH1A
     Gpio_Init(GpioPortB, GpioPin11, &stcTIM6Port);
     Gpio_SetAfMode(GpioPortB,GpioPin11,GpioAf5);               //PB11设置为TIM6_CA
	
	
		 App_TimerLEDCfg(1600,0);//PWM     20KHZ                    占空比= (pre/3200)     //最大亮度对应脉宽数值
		 App_TimerLED2Cfg(1600,0); 
}















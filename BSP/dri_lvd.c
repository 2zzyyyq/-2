#include "ALL_Var.h"
 //Wdt_Feed();//喂狗
void App_WdtInit(void);
void App_LvdInit(void);
void App_LvdPortInit(void);
void App_WdtInit(void)
{
///< 开启WDT外设时钟
	
Sysctrl_SetPeripheralGate(SysctrlPeripheralWdt,TRUE);
	
///< WDT 初始化
Wdt_Init(WdtIntEn, WdtT1s64);

EnableNvic(WDT_IRQn, IrqLevel3, TRUE);
///< 启动 WDT
Wdt_Start();
	
}



///< LVD相关端口初始化
 void App_LvdPortInit(void)
{
//    stc_gpio_cfg_t stcGpioCfg;
//    DDL_ZERO_STRUCT(stcGpioCfg);

//    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);    //开LVD时钟
    

}

 void App_LvdInit(void)
{
    stc_lvd_cfg_t stcLvdCfg;

    DDL_ZERO_STRUCT(stcLvdCfg);     //变量清0
 
	  SetBit((uint32_t)(&(M0P_SYSCTRL->PERI_CLKEN)), SysctrlPeripheralVcLvd, TRUE);//开LVD时钟
    stcLvdCfg.enAct        = LvdActMskInt;              ///< 配置触发产生中断
    stcLvdCfg.enInputSrc   = LvdInputSrcMskVCC;        ///< 配置LVD输入源
    stcLvdCfg.enThreshold  = LvdMskTH3_3V;              ///< 配置LVD基准电压
    stcLvdCfg.enFilter     = LvdFilterMskEnable;        ///< 滤波使能
    stcLvdCfg.enFilterTime = LvdFilterMsk7_2ms;        ///< 滤波时间设置
    stcLvdCfg.enIrqType    = LvdIrqMskRise;             ///< 中断触发类型
    Lvd_Init(&stcLvdCfg);
   
    M0P_LVD->CR_f.IE = TRUE; ///< 中断开启
    M0P_LVD->IFR_f.INTF = 0u; 
	
		EnableNvic(LVD_IRQn, IrqLevel3, TRUE);              ///< NVIC 中断使能
    ///< LVD 模块使能
    M0P_LVD->CR_f.LVDEN = 1u;
}















#include "ddl.h"
#include "ALL_Var.h"
#include "sysctrl.h"
#include "flash.h"
//时钟配置初始化
void App_ClockCfg(void);
void App_ClockCfg(void)
{
	 en_flash_waitcycle_t      enWaitCycle;
	 stc_sysctrl_pll_cfg_t     stcPLLCfg;

    //结构体初始化清零
    DDL_ZERO_STRUCT(stcPLLCfg);
    
    enWaitCycle = FlashWaitCycle1;
    Flash_WaitCycle(enWaitCycle);

    
    stcPLLCfg.enInFreq    = SysctrlPllInFreq4_6MHz;     //RCH 4MHz
    stcPLLCfg.enOutFreq   = SysctrlPllOutFreq24_36MHz;  //PLL 输出32MHz
    stcPLLCfg.enPllClkSrc = SysctrlPllRch;              //输入时钟源选择RCH
    stcPLLCfg.enPllMul    = SysctrlPllMul8;            //4MHz x 8 = 32MHz
    Sysctrl_SetPLLFreq(&stcPLLCfg);
    Sysctrl_SetPLLStableTime(SysctrlPllStableCycle16384);
    Sysctrl_ClkSourceEnable(SysctrlClkPLL, TRUE);
    
    Sysctrl_SysClkSwitch(SysctrlClkPLL);  ///< 时钟切换

}
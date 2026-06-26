#include "ALL_Var.h"

void App_AdcJqrCfg(void);
///< ADC采样端口初始化
void App_AdcPortInit(void);
///< ADC模块初始化
void App_AdcInit(void);
///< ADC采样端口初始化
void App_AdcPortInit(void)
{    
    ///< 开启ADC/BGR GPIO外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    Gpio_SetAnalogMode(GpioPortA, GpioPin2);        //PA02 (AIN2)
    Gpio_SetAnalogMode(GpioPortA, GpioPin3);        //PA03 (AIN3)
		App_AdcInit();	                      //< ADC模块初始化

		App_AdcJqrCfg();                      //< ADC插队扫描功能配置
}

void App_AdcInit(void)
{
    stc_adc_cfg_t              stcAdcCfg;

    DDL_ZERO_STRUCT(stcAdcCfg);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE); 
    Bgr_BgrEnable();        ///< 开启BGR
    ///< ADC 初始化配置
    stcAdcCfg.enAdcMode         = AdcScanMode;              ///<采样模式-扫描
    stcAdcCfg.enAdcClkDiv       = AdcMskClkDiv4;            ///<采样分频-1
    stcAdcCfg.enAdcSampCycleSel = AdcMskSampCycle12Clk;      ///<采样周期数-8
    stcAdcCfg.enAdcRefVolSel    = AdcMskRefVolSelInBgr1p5;      ///<参考电压选择-VCC
    stcAdcCfg.enAdcOpBuf        = AdcMskBufDisable;         ///<OP BUF配置-关
    stcAdcCfg.enInRef           = AdcMskInRefEnable;       ///<内部参考电压使能-关
    stcAdcCfg.enAdcAlign        = AdcAlignRight;               ///<转换结果对齐方式-右
    Adc_Init(&stcAdcCfg);
}
///< ADC插队扫描功能配置
void App_AdcJqrCfg(void)
{
    stc_adc_jqr_cfg_t          stcAdcJqrCfg;
    
    DDL_ZERO_STRUCT(stcAdcJqrCfg);
    
    ///< 插队扫描模式功能及通道配置
    ///< 注意：扫描模式下，当配置转换次数为n时，转换通道的配置范围必须为[JQRCH(0)MUX,JQRCH(n-1)MUX]
    stcAdcJqrCfg.bJqrDmaTrig = FALSE;
    stcAdcJqrCfg.u8JqrCnt    = 3;
    Adc_JqrModeCfg(&stcAdcJqrCfg);

    Adc_CfgJqrChannel(AdcJQRCH0MUX, AdcExInputCH2);
	
	
    Adc_CfgJqrChannel(AdcJQRCH1MUX, AdcExInputCH3);

    
    ///< ADC 中断使能
    Adc_EnableIrq();
    EnableNvic(ADC_IRQn, IrqLevel3, TRUE);

    ///< 启动插队扫描采样
    Adc_JQR_Start();
}

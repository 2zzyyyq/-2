#include "ALL_Var.h"
 
void Lvd_IRQHandler(void)
{
  Lvd_ClearIrq();
 __disable_irq();
	MOTOR_DN_IO(0);//停止电机
	MOTOR_UP_IO(0);
	B_MOTOR_DN_IO(0);
	B_MOTOR_UP_IO(0);
	BuzVcc_OFF;
	BUZ_OFF;
	LED_LIGHT_OFF;
	LED24V_IO2(0);
	LED24V_IO(0);
	Flag.Low_power=1;
	FDL_Block0_Save_Times=FDL_ONCE_MAX_WRITE_NUM-1;	
	Flash_WriteStruct(FDL_Block0+FDL_Block0_Save_Times*sizeof(Flash_Data),(uint8_t*)&Flash_Data,sizeof(Flash_Data));//写入一次	
	FDL_Block1_Save_Times=FDL_BLOCK1_ONCE_MAX_WRITE_NUM-1;
	Flash_WriteStruct(FDL_Block1+FDL_Block1_Save_Times*sizeof(Ell_Data),(uint8_t*)&Ell_Data,sizeof(Ell_Data));//块1写入一次	
	__enable_irq(); // 重新启用中断
	while(1)
	{
	
	
	};
}

///< WDT 中断服务程序
void Wdt_IRQHandler(void)
{
    if(Wdt_GetIrqStatus())
    {
        Wdt_IrqClr();       ///<清除 wdt 中断标记
			if(!Flag.Low_power)
			{
				Flash_Data.Quite_updata=0;
				BootLoardDeal();
				__NVIC_SystemReset();
			}
    }
}
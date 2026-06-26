#include "ALL_Var.h"
///< ADC中断服务函数
void Adc_IRQHandler(void)
{    
if(TRUE == Adc_GetIrqStatus(AdcMskIrqJqr))
    {
     Adc_ClrIrqStatus(AdcMskIrqJqr);
		 Adc_JQR_Stop();
		 Motor_Current.AD_sum+=(uint16_t)(Adc_GetJqrResult(AdcJQRCH1MUX)); 
	 
		 Flag.AD19_Change_start=0;
		 Motor_Current.AD_sumple_num++;
		 if(Motor_Current.AD_sumple_num>=300)
		 {
		 	Motor_Current.AD_sumple_num=0;
			Motor_Current.ADvalue=Motor_Current.AD_sum/300;	
			Motor_Current.AD_sum=0;	
			if((Time.Overweight_Current_time>10)&&(Flag.Overweight_current_correct_start))//校正启动1s后
			{
				Overweight_Correct.Sample_num++;
				Overweight_Correct.Sample_sum+=Motor_Current.ADvalue;
			}
		 }
	 	Motor_Current_B.AD_sum+=(uint16_t)(Adc_GetJqrResult(AdcJQRCH0MUX)); 

		Flag.AD19_Change_start=1;
		Motor_Current_B.AD_sumple_num++;
		if(Motor_Current_B.AD_sumple_num>=300)
		{
		 	Motor_Current_B.AD_sumple_num=0;
			Motor_Current_B.ADvalue=Motor_Current_B.AD_sum/300;	
			Motor_Current_B.AD_sum=0;	
			if((Time.B_Overweight_Current_time>10)&&(Flag.B_Overweight_current_correct_start))//校正启动1s后
			{
				B_Overweight_Correct.Sample_num++;
				B_Overweight_Correct.Sample_sum+=Motor_Current_B.ADvalue;
			}
		}
			Adc_JQR_Start();
}
}

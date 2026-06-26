#include "ALL_Var.h"
#include "adt.h"
//PB00	TIM5_CHB	BUZ_PWM	蜂鸣器
//PB01		BUZ_PWR
const tBuzHx Bzx0[] = {{FREQ_NO,    0,  0}, {FREQ_NO,   0,   0}, {FREQ_NO,    0,  0}, {FREQ_NO, 0, 0}};
const tBuzHx Bzx1[] = {{FREQ_1288, 10, 10}, {FREQ_NO,   0,   0}, {FREQ_NO,    0,  0}, {FREQ_NO, 0, 0}}; //¶ÌÃù
const tBuzHx Bzx2[] = {{FREQ_1288, 50, 50}, {FREQ_NO,   0,   0}, {FREQ_NO,    0,  0}, {FREQ_NO, 0, 0}}; //³¤Ãù
const tBuzHx Bzx3[] = {{FREQ_1238, 12,  6}, {FREQ_1288, 12,  6}, {FREQ_1342, 80, 20}, {FREQ_NO, 0, 0}}; //Éýµ÷
const tBuzHx Bzx4[] = {{FREQ_1342, 12,  6}, {FREQ_1288, 12,  6}, {FREQ_1238, 80, 20}, {FREQ_NO, 0, 0}}; //½µµ÷
const tBuzHx Bzx5[] = {{FREQ_1288, 30, 10}, {FREQ_1288, 30, 10}, {FREQ_NO,    0,  0}, {FREQ_NO, 0, 0}}; //±¨¾¯"di-di"
const tBuzHx Bzx6[] = {{FREQ_1288, 30, 10}, {FREQ_1288, 30, 10}, {FREQ_1288, 30, 10}, {FREQ_NO, 0, 0}}; //±¨¾¯"di-di-di"

const tBuzHx Bzx7[] = {{FREQ_2300, 12,  6}, {FREQ_2600, 12,  6}, {FREQ_2900, 80, 20}, {FREQ_NO, 0, 0}}; //Éýµ÷1
const tBuzHx Bzx8[] = {{FREQ_2900, 12,  6}, {FREQ_2600, 12,  6}, {FREQ_2300, 80, 20}, {FREQ_NO, 0, 0}}; //½µµ÷1

const tBuzHx Bzx9[] = {{FREQ_1288, 30, 10}, {FREQ_1288, 30, 10}, {FREQ_1288, 30, 10}, {FREQ_NULL, 130, 0},
		       {FREQ_1288, 30, 10}, {FREQ_1288, 30, 10}, {FREQ_1288, 30, 10}, {FREQ_NO, 130, 0}}; //±¨¾¯"di-di-di","di-di-di"
		       
const tBuzHx Bzx10[] = {{FREQ_1288, 30, 10}, {FREQ_1288, 30, 10}, {FREQ_1288, 30, 10}, {FREQ_NULL, 130, 0},
		       {FREQ_1288, 30, 10}, {FREQ_1288, 30, 10}, {FREQ_1288, 30, 10}, {FREQ_NULL, 130, 0},
		       {FREQ_1288, 30, 10}, {FREQ_1288, 30, 10}, {FREQ_1288, 30, 10}, {FREQ_NO, 130, 0}}; //±¨¾¯"di-di-di","di-di-di","di-di-di"
uint8_t    g_BuzFrzSta = 0;
uint8_t    g_BuzFrzValue = 0;
uint8_t beep_step=0,beep_step_temp = 0;
tBuzHx  Bzx[12];
tBuzHx *Buz = (tBuzHx *)Bzx; 
void SetupBzhx(uint8_t MBz,uint8_t flag);
					
/***********************************************************************************************************************
* Function Name: SetupBzhx
* Description  : initializes the Buzzer application.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/ 
void SetupBzhx(uint8_t MBz,uint8_t flag)
{	 	
   uint8_t i;	
	if(!Flag.Beep_open_now)
	{
		BuzVcc_OFF; 
		BUZ_OFF;
		g_BuzFrzValue = 0;
		g_BuzFrzSta = 0;
		beep_step=0;
		Buz = (tBuzHx *)Bzx; 
		if(flag)
			{
				Flag.Beep_open_now=1;
				switch(MBz)
				{
				case SHORT_BEEP:
					 for (i = 0; i < 4; i++) Bzx[i] = Bzx1[i];
					 beep_step_temp=1;
					 break;
				case LONG_BEEP:
					 for (i = 0; i < 4; i++) Bzx[i] = Bzx2[i]; 
					 beep_step_temp=1;
					 break;      
				case LONG_BEEP_THREE:
					 for (i = 0; i < 4; i++) Bzx[i] = Bzx6[i];
						beep_step_temp=3;
					 break;	 
				case OVER_WIGHT_ALARM_BEEP://³¬ÖØ
					 for (i = 0; i < 8; i++) Bzx[i] = Bzx9[i]; 
						 beep_step_temp=7;
					 break;	
				case MOTOR_ERR_ALARM_BEEP://µç» ¹ÊÕÏ
					 for (i = 0; i < 12; i++) Bzx[i] = Bzx10[i]; 
						 beep_step_temp=11;
					 break;	
				default:
					 for (i = 0; i < 4; i++) Bzx[i] = Bzx0[i]; 
					 Flag.Beep_open_now=0;
					 break;     
				} 
			}	
		}	
	}

/***********************************************************************************************************************
* Function Name: CmdBzhx
* Description  : Buzzer application.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
uint8_t CmdBzhx(void)
{		
    if (g_BuzFrzValue != Buz->BzFre)
    {
        if (Buz->BzFre == FREQ_NULL)
        {
            //App_AdvTimerInit(0, 0);    // 蜂鸣器 PWM控制输出50% 频率控制 10kz 
            Adt_StopCount(M0P_ADTIM5);
				  	Adt_ClearCount(M0P_ADTIM5);
        }
        else
        {
            
						Adt_SetPeriod(M0P_ADTIM5, Buz->BzFre);                         //周期设置
						Adt_SetCompareValue(M0P_ADTIM5, AdtCompareB,  Buz->BzFre / 2); 
					
        }
        g_BuzFrzValue = Buz->BzFre;
    }
    
    if (Buz->BzFre != FREQ_NO)
    {
        if (Buz->T_BzV != 0)
        {
            Buz->T_BzV--;
            if (Buz->BzFre != FREQ_NULL)
            {
                BuzVcc_ON;
            }
        }
        else
        {
            BuzVcc_OFF;
        }
        
        if (Buz->T_BzO != 0)
        {
            Buz->T_BzO--;
            if (g_BuzFrzSta == 0)
            {
                g_BuzFrzSta = 1;
                BUZ_ON;
            }
        }
        else
        {
            BUZ_OFF;
            g_BuzFrzSta = 0;
            Buz++;
            beep_step++;
            if (beep_step == beep_step_temp)
            {
                beep_step = 0;
                Flag.Beep_open_now = 0;
            }
        }
    }
    
    return 0;
}

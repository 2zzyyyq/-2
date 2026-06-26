/**
  ******************************************************************************
  * Copyright (C), 2020, 杭州凯撒大帝技术有限公司
  * @file       keelop_crc.c
  * @brief      编码采用 keeloq 加密
  * @version    版本，如：V1.0.0 
  * @author     作者，如：John（汪）
  * @date       日期，如：2021.03.16
  *
  * 修改历史：  初始版本
  * 版本        修改日期        修改者      修改内容
  *
  ******************************************************************************
 **/
#include "ALL_Var.h"
unsigned char key_fixed[8]={0X19,0X28,0X37,0X46,0X50,0XAF,0XBE,0XCD}; 
unsigned char _key[8]; 
unsigned char NLF[2][2][2][2][2];
static void KeeLoq_Init(void); 
unsigned char getBit(unsigned char source[], int n) ;
void RF_DEC(unsigned char *source) ;
void RLC(unsigned char *source, char c, char n);
void Keelop_KEY(unsigned long  key_write);
/**********************************************************************************
 * 函数名：unsigned char getBit(unsigned char source[],int n)
 * 描述  ：
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用 
 **********************************************************************************/
unsigned char getBit(unsigned char source[], int n) 
{ 
    unsigned char temp0 = (unsigned char) 1<<(n % 8); 
    unsigned char temp1 = source[n / 8] & temp0; 
    if(temp1 != 0) 
    {
		return 1;
	}
    else
    {
		return 0;
	}
}
/**********************************************************************************
 * 函数名：void RLC(unsigned char *source,char c,char n)
 * 描述  ：
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用 
 **********************************************************************************/
void RLC(unsigned char *source, char c, char n)
{
	int i=0;
	unsigned char temp;
	for(i = 0; i < n; i++)
	{ 
		temp = source[i];
		if(c != 0)
		{
			source[i] = (source[i] <<1 ) | 0x01;
		}
		else
		{
			source[i] = (source[i] << 1) & 0xfe; 
		}

		if((temp & 0x80) != 0)
		{
			c = 1;
		}
		else
		{
			c = 0;
		}
	} 
}
/**********************************************************************************
 * 函数名：void RF_DEC(unsigned char *source) 
 * 描述  ：
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用 
 **********************************************************************************/
void RF_DEC(unsigned char *source) 
{
	int i=0;
	unsigned char c=0;
	unsigned char nlf, y15, y31, k, result;
	
	KeeLoq_Init(); 
	
	for (i = 528; i > 0; i--) 
	{ 
		nlf = NLF[getBit(source, 30)][getBit(source, 25)][getBit(source, 19)]
		         [getBit(source, 8)][getBit(source, 0)];
		y15 = getBit(source, 15);
		y31 = getBit(source, 31);
		k = getBit(_key, (i - 1) % 64);
		result = nlf^y15^y31^k;
		if (result != 0) 
		{
			c = 1;
		}
		else 
		{
			c = 0;
		} 
		RLC(source, c, 4);  
	}
}
/**********************************************************************************
 * 函数名：static void KeeLoq_Init(void) 
 * 描述  ：
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用 
 **********************************************************************************/
static void KeeLoq_Init(void) 
{ 
    NLF[0][0][0][0][0] = 0; 
    NLF[0][0][0][0][1] = 1; 
    NLF[0][0][0][1][0] = 1; 
    NLF[0][0][0][1][1] = 1;       
    NLF[0][0][1][0][0] = 0;        
    NLF[0][0][1][0][1] = 1; 
    NLF[0][0][1][1][0] = 0; 
    NLF[0][0][1][1][1] = 0;

    NLF[0][1][0][0][0] = 0; 
    NLF[0][1][0][0][1] = 0; 
    NLF[0][1][0][1][0] = 1; 
    NLF[0][1][0][1][1] = 0; 
    NLF[0][1][1][0][0] = 1; 
    NLF[0][1][1][0][1] = 1; 
    NLF[0][1][1][1][0] = 1; 
    NLF[0][1][1][1][1] = 0;

    NLF[1][0][0][0][0] = 0; 
    NLF[1][0][0][0][1] = 0; 
    NLF[1][0][0][1][0] = 1; 
    NLF[1][0][0][1][1] = 1; 
    NLF[1][0][1][0][0] = 1; 
    NLF[1][0][1][0][1] = 0; 
    NLF[1][0][1][1][0] = 1; 
    NLF[1][0][1][1][1] = 0;

    NLF[1][1][0][0][0] = 0; 
    NLF[1][1][0][0][1] = 1; 
    NLF[1][1][0][1][0] = 0; 
    NLF[1][1][0][1][1] = 1; 
    NLF[1][1][1][0][0] = 1; 
    NLF[1][1][1][0][1] = 1; 
    NLF[1][1][1][1][0] = 0; 
    NLF[1][1][1][1][1] = 0; 
}
/**********************************************************************************
 * 函数名：void Keelop_KEY(uint32_t key_write)
 * 描述  ：
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用 
 **********************************************************************************/
void Keelop_KEY(unsigned long  key_write)
{
	unsigned char i;
	if (key_write == 0)
	{
		for (i = 0; i < 8; i++)
		    _key[i] = key_fixed[i]; 
	}
	else
	{
		_key[3] = (unsigned char)key_write;
		_key[2] = (unsigned char)(key_write >> 8);
		_key[1] = (unsigned char)(key_write >> 16);
		_key[0] = (unsigned char)(key_write >> 24);
		for (i = 0; i < 4; i++)//8
		    _key[i + 4] = _key[i] << 1;
	}
}

/***************************** (END OF FILE) *********************************/

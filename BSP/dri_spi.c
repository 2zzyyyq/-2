#include "spi.h"
#include "ALL_Var.h"

  void App_GpioInit(void);
  void App_SPIInit(void);



/**
******************************************************************************
    ** \brief  主函数
    **
  ** @param  无
    ** \retval 无
    **
******************************************************************************/
//int32_t main(void)
//{
//    uint16_t tmp;
//    volatile uint8_t tmp1;

//    tmp = 0;

//    ///< 端口初始化
//    App_GpioInit();
//    ///< SPI初始化
//    App_SPIInit();

//    ///< USER 按下启动通信
//    while(TRUE == Gpio_GetInputIO(STK_USER_PORT, STK_USER_PIN));

//   ///< 片选，开始通讯
//   Spi_SetCS(M0P_SPI0, FALSE);

//    ///< 主机向从机发送数据
//    while(tx_cnt<10)                                        //主机发送数据给从机
//    {
//        while(Spi_GetStatus(M0P_SPI0, SpiTxe) == FALSE);    //等待发送缓冲器空
//        Spi_SendData(M0P_SPI0, tx_buf[tx_cnt++]);
//    }
//Spi_SetCS(M0P_SPI0, TRUE);
//    ///< 主机接收从机数据
//    while(rx_cnt<10)                                        //接收从机的数据
//    {
//        while(Spi_GetStatus(M0P_SPI0, SpiTxe) == FALSE);    //等待发送缓冲器空
//        Spi_SendData(M0P_SPI0, 0);                          //发送空数据
//        delay10us(20);
//        while(Spi_GetStatus(M0P_SPI0, SpiRxne) == FALSE);
//        rx_buf[rx_cnt++] = Spi_ReceiveData(M0P_SPI0);
//    }

//    ///< 结束通信
   

//    ///< 判断发送的数据与接收的数据是否相等
//    for(tmp = 0; tmp<10; )
//    {
//        if(rx_buf[tmp] == tx_buf[tmp])
//            tmp++;
//        else
//            break;
//    }
//    if(tmp == 10)                                    //如果接收到的数据与发送的数据相等则点亮板上LED
//        Gpio_WriteOutputIO(STK_LED_PORT, STK_LED_PIN, TRUE);

//    while(1);
//}


/**
 ******************************************************************************
 ** \brief  初始化外部GPIO引脚
 **
 ** \return 无
 ******************************************************************************/
 void App_GpioInit(void)
{
    stc_gpio_cfg_t GpioInitStruct;
    DDL_ZERO_STRUCT(GpioInitStruct);

    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);

    //SPI0引脚配置:主机
    GpioInitStruct.enDrv = GpioDrvH;
    GpioInitStruct.enDir = GpioDirOut;

    Gpio_Init(GpioPortA, GpioPin7,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortA, GpioPin7,GpioAf1);        //配置引脚PA7作为SPI0_MOSI

    GpioInitStruct.enDrv = GpioDrvH;
    GpioInitStruct.enDir = GpioDirOut;
    Gpio_Init(GpioPortA, GpioPin5,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortA, GpioPin5,GpioAf1);         //配置引脚PA05作为SPI0_SCK
		


			GpioInitStruct.enDrv = GpioDrvH;
			GpioInitStruct.enDir = GpioDirOut;
			Gpio_Init(GpioPortA, GpioPin4,&GpioInitStruct);
			Gpio_SetAfMode(GpioPortA, GpioPin4,GpioAf1);         //配置引脚PA04作为SPI0_CS
		
	
		
		
		
		
//    GpioInitStruct.enDir = GpioDirIn;

//    Gpio_Init(GpioPortA, GpioPin6,&GpioInitStruct);
//    Gpio_SetAfMode(GpioPortA, GpioPin6,GpioAf1);         //配置引脚PA06作为SPI0_MISO


}

/**
 ******************************************************************************
 ** \brief  初始化SPI
 **
 ** \return 无
 ******************************************************************************/
 void App_SPIInit(void)
{
    stc_spi_cfg_t  SpiInitStruct;

    Sysctrl_SetPeripheralGate(SysctrlPeripheralSpi0,TRUE);

    //SPI0模块配置：主机
    SpiInitStruct.enSpiMode = SpiMskMaster;   //配置位主机模式
    SpiInitStruct.enPclkDiv = SpiClkMskDiv128;  //波特率：fsys/128
    SpiInitStruct.enCPHA    = SpiMskCphafirst;//第一边沿采样
    SpiInitStruct.enCPOL    = SpiMskcpollow;  //极性为低
    Spi_Init(M0P_SPI0, &SpiInitStruct);
}
















































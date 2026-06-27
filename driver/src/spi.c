
#include "spi.h"

/**
 ******************************************************************************
 ** \addtogroup SpiGroup
 *****************************************************************************/
//@{

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 *****************************************************************************/

#define IS_VALID_STAT(x)            (   SpiIf == (x)||\
                                        SpiSserr == (x)||\
                                        SpiBusy == (x)||\
                                        SpiMdf == (x)||\
                                        SpiTxe == (x)||\
                                        SpiRxne == (x))


/******************************************************************************/
/* Local function prototypes ('static')                                       */
/******************************************************************************/

/******************************************************************************/
/* Local variable prototypes ('static')                                       */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief  SPI 请求状态获取
 **
 ** \param [in]SPIx 通道， enStatus 获取请求
 **
 ** \retval 请求状态
 ** 
 ******************************************************************************/
boolean_t Spi_GetStatus(M0P_SPI_TypeDef* SPIx, en_spi_status_t enStatus)
{    
    ASSERT(IS_VALID_STAT(enStatus));
    
    if(SPIx->STAT&enStatus)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
    
}
/**
 ******************************************************************************
 ** \brief  SPI中断清除
 **
 ** \param [in]SPIx 通道选择
 **
 ** \retval 请求状态
 ** 
 ******************************************************************************/
en_result_t Spi_ClearStatus(M0P_SPI_TypeDef* SPIx)
{    
    SPIx->ICLR = 0;
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief  SPI 中断使能函数
 **
 ** \param [in] SPIx 通道
 **
 ** \retval Ok成功
 **
 ******************************************************************************/
en_result_t Spi_IrqEnable(M0P_SPI_TypeDef* SPIx)
{ 
    SPIx->CR2 |= 0x4u;
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief  SPI 中断禁止函数
 **
 ** \param [in] enCh通道
 **
 ** \retval Ok成功
 **
 ******************************************************************************/
en_result_t Spi_IrqDisable(M0P_SPI_TypeDef* SPIx)
{ 
    SPIx->CR2 &= ~0x4u;
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief  SPI 功能使能函数
 **
 ** \param [in] SPIx 通道，enFunc功能
 **
 ** \retval Ok初始化成功
 **
 ******************************************************************************/
en_result_t Spi_FuncEnable(M0P_SPI_TypeDef* SPIx, en_spi_func_t enFunc)
{    
    SPIx->CR2 |= enFunc;
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief  SPI 功能禁止函数
 **
 ** \param [in] SPIx 通道，enFunc功能
 **
 ** \retval Ok初始化成功
 **
 ******************************************************************************/
en_result_t Spi_FuncDisable(M0P_SPI_TypeDef* SPIx, en_spi_func_t enFunc)
{    
    SPIx->CR2 &= ~(uint32_t)enFunc;
    
    return Ok;
}

/**
 ******************************************************************************
 ** \brief  SPI 总体初始化函数
 **
 ** \param [in] SPIx 通道
 ** \param [in] pstcSpiCfg 初始化结构
 **
 ** \retval Ok初始化成功
 ** \retval ErrorInvalidParameter 初始化错误
 ******************************************************************************/
en_result_t Spi_Init(M0P_SPI_TypeDef* SPIx, stc_spi_cfg_t *pstcSpiCfg)
{
    ASSERT(NULL != pstcSpiCfg);

    SPIx->CR = 0;
    SPIx->CR =  (uint32_t)pstcSpiCfg->enSpiMode |
                (uint32_t)pstcSpiCfg->enPclkDiv |
                (uint32_t)pstcSpiCfg->enCPOL    |
                (uint32_t)pstcSpiCfg->enCPHA    |
                (uint32_t)0x40;
    
    SPIx->STAT = 0x00;

    return Ok;
}

/**
 ******************************************************************************
 ** \brief  SPI 配置主发送的电平
 **
 ** \param [in] SPIx 通道选择，bFlag高低电平
 **
 ** \retval 无
 ** 
 ******************************************************************************/
void Spi_SetCS(M0P_SPI_TypeDef* SPIx, boolean_t bFlag) 
{    
    SPIx->SSN = bFlag;    
} 
/**
 ******************************************************************************
 ** \brief  SPI 发送一字节函数
 **
 ** \param [in] SPIx 通道选择，u8Data发送字节
 **
 ** \retval Ok发送成功
 ** 
 ******************************************************************************/
en_result_t Spi_SendData(M0P_SPI_TypeDef* SPIx, uint8_t u8Data)
{  
    SPIx->DATA = u8Data;
   
    return Ok;
}

/**
 ******************************************************************************
 ** \brief  SPI 接收一字节函数
 **
 ** \param [in] SPIx接收通道
 **
 ** \retval 接收一字节数据
 ** 
 ******************************************************************************/
uint8_t Spi_ReceiveData(M0P_SPI_TypeDef* SPIx)
{
    return SPIx->DATA;
}

//@} // SpiGroup
/******************************************************************************
 * EOF (not truncated)
 *****************************************************************************/


#include "ALL_Var.h"
volatile uint8_t * gp_uart0_tx_address;         /* uart0 send buffer address */
volatile uint16_t  g_uart0_tx_count;            /* uart0 send data number */
volatile uint8_t * gp_uart0_rx_address;         /* uart0 receive buffer address */
volatile uint16_t  g_uart0_rx_count;            /* uart0 receive data number */
volatile uint16_t  g_uart0_rx_length;           /* uart0 receive data length */
uint8_t send_busy=0;
/***串口配置***/
//UART1pc中断函数
void Uart0_IRQHandler(void)
{
	
  volatile  uint8_t rx_data;

	
    if(Uart_GetStatus(M0P_UART0, UartRC))         //UART1数据接收
    {
          Uart_ClrStatus(M0P_UART0, UartRC);        //清中断状态位
          rx_data = Uart_ReceiveData(M0P_UART0);   //接收数据字节
          r_uart0_receive_input(rx_data);
// if (g_uart0_rx_length > g_uart0_rx_count)
//    {
//         *gp_uart0_rx_address = rx_data;
//          gp_uart0_rx_address++;
//          g_uart0_rx_count++;
//    }

    }

    if(Uart_GetStatus(M0P_UART0, UartTC))         //UART1数据发送
    {
      
        Uart_ClrStatus(M0P_UART0, UartTC);        //清中断状态位
        
  if (g_uart0_tx_count > 0U)
    {
      
		    	Uart_SendDataPoll(M0P_UART0,*gp_uart0_tx_address);         //调用库函数，通过UART0发送一个字母。
          gp_uart0_tx_address++;
          g_uart0_tx_count--;
    }
    else
    {
      send_busy=0;
    }
        
    }

}
volatile uint8_t * gp_uart1_tx_address;         /* uart0 send buffer address */
volatile uint16_t  g_uart1_tx_count;            /* uart0 send data number */
volatile uint8_t * gp_uart1_rx_address;         /* uart0 receive buffer address */
volatile uint16_t  g_uart1_rx_count;            /* uart0 receive data number */
volatile uint16_t  g_uart1_rx_length;           /* uart0 receive data length */
//void LpUart1_IRQHandler(void)
//{

//		 uint8_t rx_data;
// 

//    if(LPUart_GetStatus(M0P_LPUART1, LPUartRC))         //UART1数据接收
//    {
//            LPUart_ClrStatus(M0P_LPUART1, LPUartRC);      //清中断状态位
//            rx_data = LPUart_ReceiveData(M0P_LPUART1);   //接收数据字节
//            r_uart1_receive_input(rx_data);
// if (g_uart1_rx_length > g_uart1_rx_count)
//    {
//         *gp_uart1_rx_address = rx_data;
//          gp_uart1_rx_address++;
//          g_uart1_rx_count++;
//    }
//     

//    }

//    if(LPUart_GetStatus(M0P_LPUART1, LPUartTC))         //UART1数据发送
//    {
//      
//        LPUart_ClrStatus(M0P_LPUART1, LPUartTC);         //清中断状态位
//        
//  if (g_uart1_tx_count > 0U)
//    {
//      
//            LPUart_SendData(M0P_LPUART1, *gp_uart1_tx_address);		  //调用库函数，通过UART0发送一个字母。
//            gp_uart1_tx_address++;
//            g_uart1_tx_count--;
//    }
//    else
//    {
//     send_busy_asr=0;
//    }
//        
//    }

//}














































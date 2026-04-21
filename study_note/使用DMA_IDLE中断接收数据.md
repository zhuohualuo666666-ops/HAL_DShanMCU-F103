<!-- 使用DMA_IDLE中断接收数据 -->

一、基本概念
·IDLE：空闲状态
·IDLE在串口中的应用：传输大量数据时，用来表示传输已完成
·IDLE实际工作效果：使能IDLE中断后不会立刻产生，
接收完一个字节的数据（包含起始位，数据位，停止位）后一个字节的时间内没有检测到起始位，
判定发送方数据已发送完毕，产生一个IDLE中断


·使用DMA+IDLE的方式接收1000个字节，等待完成，
完成的情况有三种：
1.接收到1000个字节
2.收到IDLE中断
3.错误


二、函数解析：
1.查询方式：
函数HAL_UARTEx_ReceiveToIdle部分代码：

if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE))
        {
        ·
        ·
        ·
          return HAL_OK;
        }

if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE))
      {

      }

如果遇到空闲状态，返回HAL_OK，未遇到空闲状态，将数据保存到环形缓冲区中。


2.中断方式：
函数HAL_UARTEx_ReceiveToIdle_IT部分代码：
status =  UART_Start_Receive_IT(huart, pData, Size);
    if (status == HAL_OK)
    {
      if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
      {
        __HAL_UART_CLEAR_IDLEFLAG(huart);
        ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);//使能IDLE中断
      }
      else
      {
        status = HAL_ERROR;
      }
    }
使能串口接收中断后，会使能IDLE中断，发生IDLE中断后，在HAL_UART_IRQHandler内部
使用的是DMA+IDLE方式会暂停DMA，使用的是IDLE中断方式会清除IDLE中断标志位，
接着调用HAL_UARTEx_RxEventCallback这个回调函数
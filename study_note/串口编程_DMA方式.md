<!-- 串口编程_DMA方式 -->

串口发送DMA方式流程：
1.调用函数HAL_UART_Transmit_DMA用于启动DMA串口发送
2.等待（多任务系统阻塞，逻辑死循环等待）
3.DMA中断调用串口发送回调函数函数HAL_UART_TxCpltCallback


中断处理函数HAL_DMA_IRQHandler内部如下：
 if(hdma->XferCpltCallback != NULL)
    {
      /* Transfer complete callback */
      hdma->XferCpltCallback(hdma);
    }

XferCpltCallback在HAL_UART_Transmit_DMA函数中被设置为UART_DMATransmitCplt：
huart->hdmatx->XferCpltCallback = UART_DMATransmitCplt;

UART_DMATransmitCplt中调用了HAL_UART_TxCpltCallback串口发送回调函数：
HAL_UART_TxCpltCallback(huart);

因此，在已经配置好中断发送参数和句柄的情况下，
使用DMA发送时，将HAL_UART_Transmit_IT替换为HAL_UART_Transmit_DMA即可
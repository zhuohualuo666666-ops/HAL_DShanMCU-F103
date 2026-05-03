<!-- _SPI_HAL库编程 -->

一、三种编程方式：
1.查询方式：
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t 
*pRxData, uint16_t Size, uint32_t Timeout);

2.中断方式：
HAL_StatusTypeDef HAL_SPI_TransmitReceive_IT(SPI_HandleTypeDef *hspi, uint8_t *pTxData, 
uint8_t *pRxData, uint16_t Size);

3.DMA方式：
HAL_StatusTypeDef HAL_SPI_TransmitReceive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pTxData, 
uint8_t *pRxData, uint16_t Size); 

hspi：SPI控制器型号
pTxData：要发送数据保存位置
pRxData：要接受的数据保存位置
Size：发送/接收的字节数
Timeout：超时时间

二、函数内部：
1.HAL_SPI_TransmitReceive_IT函数部分代码：
·
·
·
/* Set the transaction information */
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pTxBuffPtr  = (uint8_t *)pTxData;
  hspi->TxXferSize  = Size;
  hspi->TxXferCount = Size;       //发送数据大小
  hspi->pRxBuffPtr  = (uint8_t *)pRxData;
  hspi->RxXferSize  = Size;
  hspi->RxXferCount = Size;       //接收数据大小

  /* Set the function for IT treatment */
  if (hspi->Init.DataSize > SPI_DATASIZE_8BIT)
  {
    hspi->RxISR     = SPI_2linesRxISR_16BIT;
    hspi->TxISR     = SPI_2linesTxISR_16BIT;
  }
  else
  {
    hspi->RxISR     = SPI_2linesRxISR_8BIT;//接收中断处理函数
    hspi->TxISR     = SPI_2linesTxISR_8BIT;//发送中断处理函数
  }
  ·
  ·
  ·

  其中，命名2lines表示用到了mosi和miso两条线，这个程序用于启动SPI

  2.SPI1_IRQHandler中断处理函数中HAL_SPI_IRQHandler函数具体实现部分代码：
  ·
  ·
  ·
   /* SPI in mode Receiver ----------------------------------------------------*/
  if ((SPI_CHECK_FLAG(itflag, SPI_FLAG_OVR) == RESET) &&
      (SPI_CHECK_FLAG(itflag, SPI_FLAG_RXNE) != RESET) && (SPI_CHECK_IT_SOURCE(itsource, SPI_IT_RXNE) != RESET))
  {
    hspi->RxISR(hspi);
    return;
  }

  /* SPI in mode Transmitter -------------------------------------------------*/
  if ((SPI_CHECK_FLAG(itflag, SPI_FLAG_TXE) != RESET) && (SPI_CHECK_IT_SOURCE(itsource, SPI_IT_TXE) != RESET))
  {
    hspi->TxISR(hspi);
    return;
  }
  ·
  ·
  ·

  我们选择传输的数据格式是8bit，结合第一个函数可知，使用中断实现SPI传输，发生中断时会调用TxISR和RxISR两个函数，
  即SPI_2linesRxISR_8BIT和SPI_2linesTxISR_8BIT

  3.SPI_2linesRxISR_8BIT函数内部实现：                                                                              
  /* Receive data in 8bit mode */
  *hspi->pRxBuffPtr = *((__IO uint8_t *)&hspi->Instance->DR);
  hspi->pRxBuffPtr++;
  hspi->RxXferCount--;

  /* Check end of the reception */
  if (hspi->RxXferCount == 0U)
  {
#if (USE_SPI_CRC != 0U)
    if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
    {
      hspi->RxISR =  SPI_2linesRxISR_8BITCRC;
      return;
    }
#endif /* USE_SPI_CRC */

    /* Disable RXNE  and ERR interrupt */
    __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_RXNE | SPI_IT_ERR));

    if (hspi->TxXferCount == 0U)
    {
      SPI_CloseRxTx_ISR(hspi);
    }
  }
具体流程：读入数据-->如果接收完毕，关闭中断-->如果发送完毕，调用 SPI_CloseRxTx_ISR(hspi)

4. SPI_2linesTxISR_8BIT函数内部实现：    
 *(__IO uint8_t *)&hspi->Instance->DR = (*hspi->pTxBuffPtr);
  hspi->pTxBuffPtr++;
  hspi->TxXferCount--;

  /* Check the end of the transmission */
  if (hspi->TxXferCount == 0U)
  {
#if (USE_SPI_CRC != 0U)
    if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
    {
      /* Set CRC Next Bit to send CRC */
      SET_BIT(hspi->Instance->CR1, SPI_CR1_CRCNEXT);
      /* Disable TXE interrupt */
      __HAL_SPI_DISABLE_IT(hspi, SPI_IT_TXE);
      return;
    }
#endif /* USE_SPI_CRC */

    /* Disable TXE interrupt */
    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_TXE);

    if (hspi->RxXferCount == 0U)
    {
      SPI_CloseRxTx_ISR(hspi);
    }
  }

具体流程：写入数据-->如果发送完毕，关闭中断-->如果接收完毕，调用 SPI_CloseRxTx_ISR(hspi)

5.SPI_CloseRxTx_ISR函数内部实现部分代码：
·
·
·
/* USE_SPI_CRC */
    if (hspi->ErrorCode == HAL_SPI_ERROR_NONE)
    {
      if (hspi->State == HAL_SPI_STATE_BUSY_RX)
      {
        hspi->State = HAL_SPI_STATE_READY;
        /* Call user Rx complete callback */
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
        hspi->RxCpltCallback(hspi);
#else
        HAL_SPI_RxCpltCallback(hspi);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
      }
      else
      {
        hspi->State = HAL_SPI_STATE_READY;
        /* Call user TxRx complete callback */
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
        hspi->TxRxCpltCallback(hspi);
#else
        HAL_SPI_TxRxCpltCallback(hspi);
·
·
·

最终调用了 HAL_SPI_TxRxCpltCallback(hspi)这个发送、接收完成回调函数


DMA方式与上面的中断类似，最终调用XferCpltCallback回调函数





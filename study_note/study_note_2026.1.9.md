<!-- 2026.1.6  STM32HAL库中断代码理解——基于案例0601_key_isr -->

该案例配置PB14引脚作为GPIO中断，
GPIO配置如下：

  /*Configure GPIO pin : PB14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;                 //配置引脚号14
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;//配置为中断模式，双边沿触发
  GPIO_InitStruct.Pull = GPIO_NOPULL;                //无上拉下拉
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

这是个初始化程序，HAL_GPIO_Init这个函数虽然取名叫GPIO，但既涉及GPIO模块，又涉及EXTI模块，
其中的EXTI模块源码配置如下：


·
·
·
/*--------------------- EXTI Mode Configuration ------------------------*/
      /* Configure the External Interrupt or event for the current IO */
      if ((GPIO_Init->Mode & EXTI_MODE) == EXTI_MODE)
      {
        /* Enable AFIO Clock */
        __HAL_RCC_AFIO_CLK_ENABLE();                                          //使能AFIO  
        temp = AFIO->EXTICR[position >> 2u];                                  //配置AFIO_EXTICR4，选中EXTI14这条线
        CLEAR_BIT(temp, (0x0Fu) << (4u * (position & 0x03u)));
        SET_BIT(temp, (GPIO_GET_INDEX(GPIOx)) << (4u * (position & 0x03u)));
        AFIO->EXTICR[position >> 2u] = temp;


        /* Enable or disable the rising trigger */
        if ((GPIO_Init->Mode & RISING_EDGE) == RISING_EDGE)
        {
          SET_BIT(EXTI->RTSR, iocurrent);
        }
        else
        {
          CLEAR_BIT(EXTI->RTSR, iocurrent);                                  //使能上升沿触发
        }

·
·
·

在这段代码里配置了EXTI模块



配置NVIC的函数在HAL_Init();里被调用：

  /* Set Interrupt Group Priority */
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
这一段配置抢占优先级和子优先级的分组

在 MX_NVIC_Init();里被调用：

static void MX_NVIC_Init(void)
{
  /* EXTI15_10_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}


这一段配置具体的优先级和使能通道

注：在cubemx里可以调整初始化序列的顺序，勾选“中断使能函数在外设初始化完成后调用”，
HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0)和 HAL_NVIC_EnableIRQ(EXTI15_10_IRQn)会出现在MX_NVIC_Init里，
不勾选的话，这两行会出现在MX_GPIO_Init中，在配置GPIO时配置中断。


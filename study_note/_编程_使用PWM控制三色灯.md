<!-- _编程_使用PWM控制三色灯 -->

三色灯由三个引脚控制三种颜色，每种颜色亮度可由生成的不同占空比的PWM波形控制，
每个PWM波形的占空比由一个字节控制，因此有0-255这256个级别。

在cubemx中，将定时器2的channel1，channel2，channel3配置为PWM生成模式，
STM32主频设置为72MHz，根据网上资料查询可知，三色灯调光频率建议设置为2KHz以上，即PWM波形频率2KHz，
因此可将分频系数设置为17，自动重装值设置为1999，
每个channel的模式设置为模式1，极性选择低电平有效（三色灯第四个引脚接了高电平）。


以下是驱动程序：
extern TIM_HandleTypeDef htim2;

#define COLOR_LED_R TIM_CHANNEL_3 //红色
#define COLOR_LED_G TIM_CHANNEL_1 //绿色
#define COLOR_LED_B TIM_CHANNEL_2 //蓝色

void ColorLED_Init(void)//启动定时器及三个通道
{
	//MX_TIM2_Init();
	HAL_TIM_PWM_Start(&htim2, COLOR_LED_R);
	HAL_TIM_PWM_Start(&htim2, COLOR_LED_G);
	HAL_TIM_PWM_Start(&htim2, COLOR_LED_B);
}

/* 0x00RRGGBB */
void ColorLED_SetColor(uint32_t color)
{
  TIM_OC_InitTypeDef sConfigOC_R = {0};
	TIM_OC_InitTypeDef sConfigOC_G = {0};
	TIM_OC_InitTypeDef sConfigOC_B = {0};

  sConfigOC_R.OCMode = TIM_OCMODE_PWM1; //模式1
  sConfigOC_R.Pulse = ((color >> 16) & 0xff)*1999/255;  /* 自动重装值最大值1999，即周期是1999；Pulse的值代表脉冲宽度；color的值范围0-255，表示256个级别，在这个式子里，占空比=color/255*/
  sConfigOC_R.OCPolarity = TIM_OCPOLARITY_LOW;//低电平有效
  sConfigOC_R.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC_R, COLOR_LED_R) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC_G.OCMode = TIM_OCMODE_PWM1;
  sConfigOC_G.Pulse = ((color >> 8) & 0xff)*1999/255;  /* 最大值1999 */
  sConfigOC_G.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC_G.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC_G, COLOR_LED_G) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC_B.OCMode = TIM_OCMODE_PWM1;
  sConfigOC_B.Pulse = ((color >> 0) & 0xff)*1999/255;  /* 最大值1999 */
  sConfigOC_B.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC_B.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC_B, COLOR_LED_B) != HAL_OK)
  {
    Error_Handler();
  }
	
}
<!-- _编程_timer高精度计时 -->

extern TIM_HandleTypeDef        htim4;
void udelay(int us)
{
	uint32_t told = __HAL_TIM_GET_COUNTER(&htim4); // timer4的计数值
	uint32_t tnow;	
	uint32_t load =  __HAL_TIM_GET_AUTORELOAD(&htim4); // timer4的auto-reload值
	/* LOAD+1个时钟对应1ms
	 * n us对应 n*(load+1)/1000个时钟
   */
	uint32_t ticks = us*(load+1)/1000;
	uint32_t cnt = 0;
	
	while (1)
	{
		tnow = __HAL_TIM_GET_COUNTER(&htim4);
		if (tnow >= told)
			cnt += tnow - told;
		else
			cnt += load + 1 - told + tnow;
		told = tnow;
		if (cnt >= ticks)
			break;
	}	
}

整体思路与systick类似：
访问VAL寄存器，用told获取当前值；
访问LOAD寄存器，用load获取重装值；
ticks存储计算出的延时特定微秒需要经过的的时钟数；

在循环中用tnow获取当前值，
cnt用来存储经过的时间。
因为定时器计数器选择向上计数模式，因此未发生溢出时数值上tnow大于told
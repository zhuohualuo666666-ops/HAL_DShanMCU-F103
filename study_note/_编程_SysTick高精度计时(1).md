<!-- _编程_SysTick高精度计时（1） -->

SysTick定时器中存在4个寄存器：
1.CTRL：控制和状态寄存器 
2.LOAD：重装载值寄存器
3.VAL：当前值寄存器
4.CALIB：校准值寄存器



在systick中断频率为1000Hz时，
一开始LOAD中的值被放入VAL中，VAL每接受到一个时钟，该值减1，
减到0时产生systick中断，表示时间过去1ms，
即1ms对应LOAD+1个计数，
那么1000us对应LOAD+1个计数，
则1us对应(LOAD+1)/1000个计数，
如果想要延迟 n 微秒，
就需要知道有 n*(LOAD+1)/1000个计数


以下是延迟n微秒的程序：
void udelay(int us)
{
	uint32_t told = SysTick->VAL;
	uint32_t tnow;
	
	uint32_t load = SysTick->LOAD;
	
	/* LOAD+1个时钟对应1ms
	 * n us对应 n*(load+1)/1000个时钟
   */
	uint32_t ticks = us*(load+1)/1000;
	
	uint32_t cnt = 0;
	
	while (1)
	{
		tnow = SysTick->VAL;
		if (told >= tnow)
			cnt += told - tnow;
		else
			cnt += told + load + 1 - tnow;
		
		told = tnow;
		if (cnt >= ticks)
			break;
	}	
}

整体思路：
访问VAL寄存器，用told获取当前值；
访问LOAD寄存器，用load获取重装值；
ticks存储计算出的延时特定微秒需要经过的的时钟数；

在循环中用tnow获取当前值，
cnt用来存储经过的时间：
如果当前值没有减到0，即未发生溢出，那么cnt=told - tnow，即过去值与当前值之差；
如果当前值减到0，即发生溢出，那么cnt += told + 1 + load - tnow，
可以拆为两部分看，told+1是told与0之间经过的计数值，load-tnow是重装后经过的计数值。
如果实际经过的计数值大于等于需要的计数值，退出循环表示已经延时特定微秒，功能已实现。
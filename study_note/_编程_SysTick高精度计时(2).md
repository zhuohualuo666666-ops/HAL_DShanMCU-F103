<!-- _编程_SysTick高精度计时(2) -->

获得系统时间(单位ns)程序：
uint64_t system_get_ns(void)
{
	uint64_t ns = HAL_GetTick(); 
	ns = ns*1000000;

	uint32_t tnow = SysTick->VAL;	
	uint32_t load = SysTick->LOAD;
	
	uint64_t cnt;
	
	cnt = load+1-tnow; /* 没有考虑tnow等于0的情况 */
	
	ns += cnt * 1000000 / (load+1) ;
	return ns;
}

思路：
ns存储获取的系统时间（单位是毫秒），这个值乘上1000000将单位换算成纳秒，
tnow存储获取的当前值，load存储获取的重装值，
cnt用来存储计算出的经过的时钟数，
前半部分提到，1us对应(LOAD+1)/1000个时钟数，
那么1ns对应(LOAD+1)/1000000个时钟数，
即cnt= n*(LOAD+1)/1000000，
则n=cnt * 1000000 / (load+1)
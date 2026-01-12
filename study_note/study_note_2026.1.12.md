<!-- 2026.1.12  案例0603_key_timer消抖理解 -->

该案例目的：按键会抖动，会导致多次 GPIO 中断，只想在“抖动彻底结束后”认定一次有效按键

直接使用delay函数延时消抖存在的问题：
1.阻塞中断
2.影响系统实时性
3.执行效率低

解决方案代码理解：
1.引入结构体
struct soft_timer {
    uint32_t timeout;        // 什么时候执行
    void *args;              // 执行时用到的数据
    void (*func)(void *);    // 到时间后执行的函数
};

结构体里三个参数，
第一个为超时时间，uint32_t类型足够大，避免隐式类型转换，避免符号位比较 bug；
第二个为要传的数据，采用void *类型，目的是解耦，将驱动层和业务层分开，不关注具体要传的值得类型；
第三个为执行的函数，采用void *类型，目的也是解耦；

2.
int g_key_cnt=0;

void key_timeout_func(void *args);

struct soft_timer key_timer={~0,NULL,key_timeout_func};

void key_timeout_func(void *args)
{
	g_key_cnt++;
	key_timer.timeout=~0;
}

定义一个中断数量统计变量g_key_cnt；
定义一个soft_timer类型的结构体，超时时间初始设为最大，暂时无参数传递，执行函数名称key_timeout_func；
定义key_timeout_func函数具体功能：中断数量统计变量增加，设置超时时间为最大（与下面的check_timer函数有关）；

3.编写一个超时时间设置函数和检测函数
void mod_timer(struct soft_timer *pTimer,uint32_t timeout)
{
	pTimer->timeout=HAL_GetTick()+timeout;
}

void check_timer(void)
{
	if(key_timer.timeout<=HAL_GetTick())
	{
		key_timer.func(key_timer.args);
	}
}


mod_timer函数里将timeout的值设置为当前systick的值加上自己想要延长的值；
check_timer函数检测当前的systick值是否超过了设置的超时时间，超过了说明到了，调用执行函数，
该函数在SysTick_Handler中被调用，每1ms执行一次



4.
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
 {
	 if(GPIO_Pin==GPIO_PIN_14)
	 {
	  mod_timer(&key_timer,10);
	 }
 }

 中断回调函数里判断是否按下按键，按下了将超时时间设置为当前时间的10ms之后，
 如果中间抖动导致不断产生中断，就不断地将超时时间设置为10ms后，
 直到抖动消除10ms后systick调用一次check_timer函数检测到超时时间小于当前时间，
 调用执行函数key_timeout_func，增加一次中断统计

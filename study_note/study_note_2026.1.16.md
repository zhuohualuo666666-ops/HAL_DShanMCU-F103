<!-- 2026.1.16  案例0604_key_circle_buffer_apply的理解 -->

整体思路：
按下按键时，PB14引脚发生边沿跳变，触发中断，调用中断回调函数HAL_GPIO_EXTI_Callback。
这个库里weak函数里我们在main函数里进行了编写，其中当检测到PB14按下时，调用mod_timer函数，该函数会通过检测到边沿跳变时将超时时间向后推迟10ms的方式实现按键消抖，
SysTick_Handler里每过1ms会调用check_timer函数进行当前时间与超时时间的比较，到时间后会调用执行函数key_timeout_func，
在这个函数中，会将g_key_cnt的值增加一次，同时把按键状态key_val的值写入环形缓冲区g_data_buf[100]。
假如我连续按了5次，那么g_key_cnt这个值在中端中会累加，最后显示的值是10。
若是把 OLED_PrintString(0,0,"CNT     :");前一行的// HAL_Delay(10000);注释取消掉，
并且我再按的过程中程序正好执行到这一行，那么这个环形缓冲区前十个位置就会就会写上0x1和0x81的交替数据，
直到程序执行到    if(circle_buf_read(&g_key_bufs,&key_val)==0)这一行时，缓冲区里的数据才会被读走。
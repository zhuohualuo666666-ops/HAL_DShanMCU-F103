<!-- GPIO中断编程 -->

编写中断程序cubemx配置步骤：
1.GPIO引脚配置为GPIO_EXIT模式
2.配置该GPIO引脚触发方式
3.配置引脚上下拉模式
4.NVIC标签下勾选enable，使能外部中断控制器EXTI interrupt发送信号给NVIC的通道
5.配置中断优先级
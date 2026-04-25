<!-- _STM32_I2C硬件结构 -->

1.软件模拟I2C编程主要内容：
发送起始信号的函数：void SendStart();
发送停止信号的函数：void SendStop();
发送一个字节的函数：void SendByte();
等待应答函数：uint8_t WaitAck();
读取一个字节的函数：uint8_t ReadByte();
延时函数

2.硬件I2C:
发送数据：将数据写入数据寄存器（Data register,简称DR），硬件会将数据从数据寄存器（Data register）中移动到数据移位寄存器(Data shift register)，再发送出去
读取数据：判断状态寄存器(Status register,简称SR),从数据寄存器中读取数据

SCL时钟控制：基准时钟T与时钟控制寄存器（Clock control Register，简称CCR）中的某个值相乘得到具体某个电平维持的时间

TXE=1：发送缓冲区空
BTF=1A:字节传输完成，移位寄存器也空

·配置流程(见STM32F103手册图273)（从左到右）：
S,EV5,Address,A,EV6,EV8_1，EV8 & Data1,A，EV8 & Data2,A，···A,EV8_2,P

（EV8与Data1长度相同，表示时间上相同；EV8比Data2及后面的Data长度要短，起始位置想用，表示时间上要短）

S：开始信号
EV5：SB（I2C_SR1的第0位）=1，表示start信号已经产生。（这个信号通过读取SR1寄存器来清除，下一步将从机地址写入数据寄存器DR）
Address：从机地址
A：应答（读取I2C_SR1的第10位AF位，0表示没有错误得到了应答，1表示应答错误，这个过程在下一步EV6中判断ADDR是否等于1时进行）
EV6：ADDR=1（这个信号通过读取SR1寄存器，再读取SR2来清除），表示地址发送成功和从机已应答
EV8_1：表示TXE=1,移位寄存器空，数据寄存器空,将第一个数据Data1写入DR
EV8：表示TXE=1,移位寄存器非空，数据寄存器空
EV8_2：表示TXE=1,BTF = 1,数据寄存器和数据移位寄存器都空，请求停止发送
P：停止信号



具体配置方法：

产生start信号：需要设置I2C_CR1(控制寄存器1)的第8位（见STM32F103手册26.6.1）为1；
判断start信号是否产生：读取I2C_SR1的第0位SB位是否为1
判断地址是否发送成功：读取I2C_SR1的第10位AF位是否为0

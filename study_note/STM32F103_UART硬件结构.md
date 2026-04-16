<!-- STM32F103_UART硬件结构 -->

1.设置波特率，数据位、校验位、停止位位数

2.发送：
数据写入TDR发送数据寄存器，
TDR将数据放入TSR发送移位寄存器，
逐位发送

TXE位：transmit data register empty
为1：TDR里数据已发送至TSR
为0：还没发送


TC：transmission complete
为1：数据位已经全部发送出去
为0：还没发完

3.接收：
RSR接收移位寄存器接收到数据，
RSR将数据给RDR接收数据寄存器，
根据RXNE的值判断有没有数据

RXNE:read data register not empty
为1：表示有数据，可以从RDR读



其他芯片常用硬件流程：使用FIFO暂存多个数据
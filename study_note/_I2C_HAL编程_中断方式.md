<!-- _I2C_HAL编程_中断方式 -->

和串口中断编程方式类似，I2C的中断编程思路是：
中断回调函数中改变“发送完成标志”的值，在编写的等待完成函数中用while循环阻塞。
具体实现如下：

static volatile int g_i2c1_tx_complete = 0;
static volatile int g_i2c1_rx_complete = 0;
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if (hi2c == &hi2c1)
	{
		g_i2c1_tx_complete = 1;
	}
}

void Wait_I2C1Tx_Complete(void)
{
	while (g_i2c1_tx_complete == 0);
	g_i2c1_tx_complete = 0;
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if (hi2c == &hi2c1)
	{
		g_i2c1_rx_complete = 1;
	}
}

void Wait_I2C1Rx_Complete(void)
{
	while (g_i2c1_rx_complete == 0);
	g_i2c1_rx_complete = 0;
}

“发送完成标志”值初始为0，中断回调函数调用时改为1，从等待函数阻塞中跳出，表示发送已完成
MEM函数类似：
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if (hi2c == &hi2c1)
	{
		g_i2c1_tx_complete = 1;
	}
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if (hi2c == &hi2c1)
	{
		g_i2c1_rx_complete = 1;
	}
}
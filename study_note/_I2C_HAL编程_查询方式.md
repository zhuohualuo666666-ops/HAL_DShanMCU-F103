<!-- _I2C_HAL编程_查询方式 -->

·MPU-6050单字节写入时序(从左到右)：
S，AD+W,ACK,RA,ACK,DATA,ACK,P

S:起始信号
AD+W：设备地址+写操作
ACK：应答信号
RA:寄存器地址
DATA:数据
P：停止信号

·MPU-6050单字节读出时序(从左到右)：
S，AD+W,ACK,RA,ACK,S,AD+R,ACK,DATA,NACK,P

S:起始信号
AD+W：设备地址+写操作
ACK：应答信号
RA:寄存器地址
AD+R:设备地址+读操作
DATA:数据
NACK:拒绝应答信号
P：停止信号

·查询方式读写MPU6050
MPU6050设备地址0b110100X,最后一位0或1，表示可同时接两个该设备，
接一个时0b1101000，即0x68。
117号（0x75）寄存器用于识别设备身份，该寄存器默认值0x68。

1.发送函数：
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, 
uint8_t *pData, uint16_t Size, uint32_t Timeout); 

参数（从左到右）依次为：I2C句柄，设备地址，要传输的数据，数据大小，超时时间

2.接收函数:
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, 
uint8_t *pData, uint16_t Size, uint32_t Timeout);

参数（从左到右）依次为：I2C句柄，设备地址（需要移位），要传输的数据，数据大小，超时时间

3.记忆模式函数:
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t 
MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout); 

AL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t 
MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);

参数（从左到右）依次为：I2C句柄，设备地址，存储（寄存器）地址，存储地址的大小，要传输的数据，数据大小，超时时间

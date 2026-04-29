<!-- _OLED操作原理(2) -->

·设置地址模式函数OLED_SetMemAddrMode如下：
static void OLED_SetMemAddrMode(MEM_MODE mode)
{
    if((mode != H_ADDR_MODE) && (mode != V_ADDR_MODE) && (mode != PAGE_ADDR_MODE))      return;
    OLED_WriteCmd(0x20);
    OLED_WriteCmd(mode);
    mem_mode = mode;
}

其内部OLED_WriteCmd函数如下：
static int OLED_WriteCmd(uint8_t cmd)
{
    uint8_t tmpbuf[2];

    tmpbuf[0] = 0;
    tmpbuf[1] = cmd;
    
    return HAL_I2C_Master_Transmit(g_pHI2COLED, OELD_I2C_ADDR, tmpbuf, 2, OLED_TIMEOUT);
}

发送两个字节数据，第一个字节数据是0，表示后面一个字节是一个命令。
0x20命令表示设置内存地址模式，
mode只关注其低两位，00表示水平地址模式，01表示垂直地址模式，10表示页地址模式，在这里值是2，采用也地址模式

·清屏函数OLED_Clear如下：
void OLED_Clear(void)
{
    uint8_t i = 0;
    uint8_t buf[128] = {0};
    
    for(i=0; i<8; i++)
    {
        OLED_SetPosition(i, 0);
        OLED_WriteNBytes(&buf[0], 128);
    }
}
其内部OLED_SetPosition函数如下：
void OLED_SetPosition(uint8_t page, uint8_t col)
{
    OLED_SetPageAddr_PAGE(page);
    OLED_SetColAddr_PAGE(col);
}

其内部OLED_SetPageAddr_PAGE函数如下：
static void OLED_SetPageAddr_PAGE(uint8_t addr)
{
    if(mem_mode != PAGE_ADDR_MODE)  return;
    if(addr > 7)   return;
    OLED_WriteCmd(0xB0 + addr);
}

OLED_SetColAddr_PAGE如下：
static void OLED_SetColAddr_PAGE(uint8_t addr)
{
    if(mem_mode != PAGE_ADDR_MODE)  return;
    if(addr > 0x7F)   return;
    OLED_WriteCmd(0x00 + (addr & 0x0F));
    OLED_WriteCmd(0x10 + (addr>>4));
}

结合数据手册可知，OLED页从0-7，列从0-127，由于命令本身占据一些数据位，
导致一个字节不够表示128列，因此将列分为低4位和高4位
因此命令0xab时由两个独立的命令字节组成
a为0表示设置列地址低4位，
a为1表示设置列地址高4位，
a为B表示设置页地址，
b表示具体地址

例如表示第127列，换为十六进制为7f，那么表示时写为0x0f和0x17

<!-- 完善UART程序与stdio -->

·回调函数可能被不止一个串口使用，
因此某个串口使用特定功能时应该加上判断：
if(huart==&huart1)


·printf函数与scanf函数来源：
1.标准库
2.microlib
3.自己写

·使用microlib方式实现printf和scanf除了要勾选microlib这个库选项，还需要实现三个函数：
int fputc(int ch, FILE* stream); 
int fgetc(FILE *f);
int __backspace(FILE *stream);
其中printf依赖于fputc，scanf依赖于fgetc和__backspace

__backspace函数的用途：
scanf得到输入字符时会进行判断，若不是想要的字符会返还回去，
下次调用fgetc时得到之前的字符   

例：

int fputc(int ch, FILE* stream) 
{ 
HAL_UART_Transmit(&huart1, (const uint8_t *)&ch, 1, 10); 
return ch; 
} 

static int g_last_char; 
static int g_backspace = 0; 

int fgetc(FILE *f) 
{ 
int ch; 
if (g_backspace)    
{ 
g_backspace = 0; 
return g_last_char; 
} 
while (0 != UART1GetChar((uint8_t *)&ch)); //未读到数据阻塞在此
g_last_char = ch;   //当前数据复制一份给g_last_char，表示上次的数据
return ch;  //读到数据后返回
} 

int __backspace(FILE *stream) 
{ 
g_backspace = 1; 
return 0; 
}

返还思路： 
g_backspace初始设为0，当调用fgetc函数获取的值不是想要的值，
调用__backspace将g_backspace设为1，
下一次调用fgetc函数时，返还值为g_last_char，即上一次的值。

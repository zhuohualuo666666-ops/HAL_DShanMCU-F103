<!-- 2026.1.14  环形缓冲区理解——案例0604_key_circle_buffer -->

环形缓冲区概念打比方：
环形操场上每隔有len个篮子，里面可以放萝卜，w先出发，往篮子里放萝卜，r跟在后面捡萝卜，
当w套了r一圈时，就站在r后面一个篮子位置时，操场上除了w面前的篮子都放满了萝卜，表示满。
之所以最后剩一个篮子不放，是为了和最初没开始放萝卜的状态区分，否则外人来看w和r这俩哥们站一块，不知道篮子里有没有萝卜。




编写思路：
1.需要四个参数：读位置，写位置，长度，实际数据
可以用一个结构体打包这四个参数，即：
typedef struct circle_buf
{
    uint32_t r;
    uint32_t w;
    uint32_t len;
    uint8_t *buf;
}circle_buf,*p_circle_buf;

意思是：
typedef struct circle_buf circle_buf;
typedef struct circle_buf * p_circle_buf;

用circle_buf代指struct circle_buf这种结构体类型
用p_circle_buf代指struct circle_buf *这个指向结构体的指针

2.环形缓冲区初始化：需要设置读位置与写位置相同都为0
void circle_buf_init(p_circle_buf pCircleBuf,uint32_t len,uint8_t *buf)
{
    pCircleBuf->r=pCircleBuf->w=0;
    pCircleBuf->len=len;
    pCircleBuf->buf=buf;
}

3.读位置函数编写：

int circle_buf_read(p_circle_buf pCircleBuf,uint8_t *pVal)
{
    if(pCircleBuf->r!=pCircleBuf->w)
    {
        *pVal=pCircleBuf->buf[pCircleBuf->r];
        pCircleBuf->r++;
        if(pCircleBuf->r==pCircleBuf->len)
        pCircleBuf->r=0;
        return 0;
    }
    else
    {
        return -1;
    }
}

读位置不等于写位置表示环形缓冲区非空，有数据
从读位置读取数据，读位置加1；
如果读位置与缓冲区长度数值相同，代表要越界了
让读位置等于0


4.写位置函数的编写：

环形缓冲区空的表示方法：当前读位置=写位置;
满的表示方法：下一个写位置=读位置；

如果下一个写位置=缓冲区长度的值
让下一个写位置=0；

判断如果未满，方法：下一个写位置不等于读位置；
此时，写位置写数据，写位置加1；


int circle_buf_write(p_circle_buf pCircleBuf,uint8_t val)
{
    uint32_t next_w;
    next_w=pCircleBuf->w+1;
    if(next_w==pCircleBuf->len)
    {
        next_w=0;
    }
    if ((next_w!=pCircleBuf->r))
    {
        pCircleBuf->buf[pCircleBuf->w]=val;
        pCircleBuf->w=next_w;
        return 0;
    }
    else
    {
    return -1;
    }
    
}

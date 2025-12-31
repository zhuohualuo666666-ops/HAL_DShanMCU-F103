<!-- 2025.12.31  STM32学习笔记-HAL库源码本质 -->
想要操作某一位GPIO，通过基地址+偏移地址的方式定位
PERIPH_BASE“外设基地址”的值 0x40000000U，这是STM32所有外设的起始地址
由外设基地址加上偏移地址得到 APB1/APB2PERIPH_BASE“总线基地址”

#define APB1PERIPH_BASE        PERIPH_BASE
#define APB2PERIPH_BASE        (PERIPH_BASE + 0x00010000U)
意思是：
APB1总线地址：0x4000 0000
APB2总线地址：0x4001 0000

由总线基地址加上偏移地址得到“GPIO端口基地址"

#define GPIOA_BASE             (APB2PERIPH_BASE + 0x00000800U)
#define GPIOB_BASE             (APB2PERIPH_BASE + 0x00000C00U)
#define GPIOC_BASE             (APB2PERIPH_BASE + 0x00001000U)

意思是：
GPIOA端口及地址=APB2总线地址+0x00000800U
GPIOB端口及地址=APB2总线地址+0x00000C00U
GPIOC端口及地址=APB2总线地址+0x00001000U

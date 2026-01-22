/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "driver_oled.h"
#include "circle_buffer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
struct soft_timer{
	uint32_t timeout;
	void *args;
	void (*func)(void*);		
};

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int g_key_cnt=0;

void key_timeout_func(void *args);

struct soft_timer key_timer={~0,NULL,key_timeout_func};

static uint8_t g_data_buf[100];
static circle_buf g_key_bufs;
 void Wait_Tx_Complete(void);
void Wait_Rx_Complete(void);
void StartUART1Recv(void);
int UART1GetChar(uint8_t *pVak);

void key_timeout_func(void *args)
{
  uint8_t key_val;/* 按下�??0x1,松开�??0x81 */
	g_key_cnt++;
	key_timer.timeout=~0;

  /* read gpio */
  if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)==GPIO_PIN_RESET)
  {
    key_val=0x1;
  }
  else
  {
    key_val=0x81;
  }
  /* put key val into circle buf */
  circle_buf_write(&g_key_bufs,key_val);
}





void mod_timer(struct soft_timer *pTimer,uint32_t timeout)
{
	pTimer->timeout=HAL_GetTick()+timeout;
}

void check_timer(void)
{
	if(key_timer.timeout<=HAL_GetTick())
	{
		key_timer.func(key_timer.args);
	}
}


 void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
 {
	 if(GPIO_Pin==GPIO_PIN_14)
	 {
	  mod_timer(&key_timer,10);
	 }
 }
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  int len;
  char *str="www.jinitaimei\r\n";
  char *str2="Please enter a char:\r\n";
  char c;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  circle_buf_init(&g_key_bufs,100,g_data_buf);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  //��ʼ��OLED
  OLED_Init();
  //���� 
	OLED_Clear();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  // HAL_Delay(10000);

  OLED_PrintString(0,0,"CNT     :");	
  len=OLED_PrintString(0,2,"Key Val :");

  HAL_UART_Transmit(&huart1,str,strlen(str),1000);

  /* use  HAL_UART_Transmit_IT 使能RXNE中断*/
  StartUART1Recv();

  while (1)
  {
   /* enable txe interrupt */
    HAL_UART_Transmit_IT(&huart1,str2,strlen(str2));
    /* wait for tc */
    Wait_Tx_Complete();


    // while(HAL_UART_Receive(&huart1,&c,1,100)!=HAL_OK);
    
   while(0!=UART1GetChar(&c));
    c=c+1;
     HAL_UART_Transmit(&huart1,&c,1,1000);
     HAL_UART_Transmit(&huart1,"\r\n",1,1000);
    // OLED_PrintSignedVal(len,0,g_key_cnt);
    // uint8_t key_val=0;
    // if(circle_buf_read(&g_key_bufs,&key_val)==0)
    // {
    //   OLED_ClearLine(len,2);
    //    OLED_PrintHex(len,2,key_val,1);
    // }
		// /* set PC13 output high */
		// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);		
		// HAL_Delay(500);
		
		// /* set PC13 output low */
		// HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
		// HAL_Delay(500);
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* EXTI15_10_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc3;

UART_HandleTypeDef hlpuart1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim8;

/* USER CODE BEGIN PV */
//TEST
int A = 2000;
uint8_t B = 0;
uint16_t C = 50;
int test;
int test2;
int Maxtest=0;
int start = 0;
//TEST

//PID
typedef struct
{
// for record New / Old value to calculate dx / dt
float Output[3];
float Error[3];
float kp;
float ki;
float kd;
float T;
}PID_StructureTypeDef;
PID_StructureTypeDef Velocontrol = {0};
PID_StructureTypeDef Poscontrol = {0};
//PID

//Trajectory
float Pos_Start = 0;
float Pos_Target = 300;
float Old_Target = 300;
float Velo_Start = 0;
float Max_Velo = 500;
float Max_Acc = 500;
float t_Acc = 2;
float q_Pos,q_Velo,q_Acc,diff_Pos,t;
int state_Tra=0;
int direction = 1;
int m_Direction = 1;
//Trajectory


//JOY
uint16_t joyAnalogRead[40];
uint8_t joySW;
uint8_t state;
uint16_t joyAvg[2];
uint32_t joySum[2];
uint16_t joyX;
uint16_t joyY;
//JOY


uint8_t Rx[5];
uint8_t header = 0x45; // Header byte
uint8_t parityBit = 0; // Parity bit initialized to 0
uint8_t dataBytes[5]; // Array to hold the bytes of uint16_t and parity bit
uint16_t dataSend = 0;

int PWMDrive;
int scale = 4;
int16_t RPSspeed;
double speed;
double speed_1;
double MAXspeed;
double speed_fill;
double speed_fill_1;
double Pos;
double pulse;

//Encoder
int Count = 0;
int32_t QEIReadRaw;
typedef struct
{
// for record New / Old value to calculate dx / dt
uint32_t Position[2];
uint64_t TimeStamp[2];
float QEIPostion_1turn;
float QEIAngularVelocity;
}QEI_StructureTypeDef;
QEI_StructureTypeDef QEIdata = {0};
uint64_t _micros = 0;
enum
{
NEW,OLD,OLDER
};
//Encoder
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM5_Init(void);
static void MX_ADC3_Init(void);
/* USER CODE BEGIN PFP */
uint64_t micros();
void QEIEncoderVel_Update();
void UARTInterruptConfig();
void Joy_Averaged();
void Joy_State();
void Trajectory();
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

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_LPUART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM8_Init();
  MX_TIM5_Init();
  MX_ADC3_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim4);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL);
  HAL_TIM_Base_Start_IT(&htim5);
  HAL_ADCEx_Calibration_Start(&hadc3, ADC_SINGLE_ENDED);
  HAL_ADC_Start_DMA(&hadc3, joyAnalogRead, 40);
  _micros = 0;
  UARTInterruptConfig();

	Velocontrol.kp = 190;
	Velocontrol.ki = 0.05;
	Velocontrol.kd = 0;
	Velocontrol.T = 0.0001;

	Poscontrol.kp = 1.2;//2.015
	Poscontrol.ki = 1.890;//4.5425
	Poscontrol.kd = 0;//0.0000021
	Poscontrol.T = 0.0001;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  QEIReadRaw = __HAL_TIM_GET_COUNTER(&htim3);
	  if(Count >= 0)
	  {
		  Pos = ((QEIReadRaw+(Count*57344))*25*3.14)/8192;
	  }
	  else
	  {
		  Pos	= ((QEIReadRaw-(fabs(Count)*57344))*25*3.14)/8192;
	  }
	  static uint64_t timestamp =0;
	  static uint64_t timestamp3 =0;
	  int64_t currentTime = micros();
	  if(currentTime > timestamp)
	  {
		QEIEncoderVel_Update();
		timestamp =currentTime + 100;//us
	  }
	  Joy_State();
	  if(start == 1)
	  {
		  Trajectory();
		  if(Pos <500)
		  {
			  if(currentTime > timestamp3)
			  {
					Poscontrol.Error[NEW] = q_Pos-Pos;
					Poscontrol.Output[NEW] = ((((Poscontrol.kp*Poscontrol.T)+(Poscontrol.ki*Poscontrol.T*Poscontrol.T)+(Poscontrol.kd))*Poscontrol.Error[NEW])-(((Poscontrol.kp*Poscontrol.T)+(Poscontrol.kd))*Poscontrol.Error[OLD])+(Poscontrol.kd*Poscontrol.Error[OLDER])+(Poscontrol.Output[OLD]*Poscontrol.T))/Poscontrol.T;
					Poscontrol.Error[OLDER] = Poscontrol.Error[OLD];
					Poscontrol.Error[OLD] = Poscontrol.Error[NEW];
					Poscontrol.Output[OLDER] = Poscontrol.Output[OLD];
					Poscontrol.Output[OLD] = Poscontrol.Output[NEW];

					Velocontrol.Error[NEW] = Poscontrol.Output[NEW]+q_Velo-speed_fill;
					Velocontrol.Output[NEW] = ((((Velocontrol.kp*Velocontrol.T)+(Velocontrol.ki*Velocontrol.T*Velocontrol.T)+(Velocontrol.kd))*Velocontrol.Error[NEW])-(((Velocontrol.kp*Velocontrol.T)+(Velocontrol.kd))*Velocontrol.Error[OLD])+(Velocontrol.kd*Velocontrol.Error[OLDER])+(Velocontrol.Output[OLD]*Velocontrol.T))/Velocontrol.T;
					Velocontrol.Error[OLDER] = Velocontrol.Error[OLD];
					Velocontrol.Error[OLD] = Velocontrol.Error[NEW];
					Velocontrol.Output[OLDER] = Velocontrol.Output[OLD];
					Velocontrol.Output[OLD] = Velocontrol.Output[NEW];

//					if(fabs(Pos_Target-Pos) <= 0.1)start = 0;
			  timestamp3 =currentTime + 100;
			  }
			  if(Velocontrol.Output[NEW] > 0)
			  {
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
	  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, Velocontrol.Output[NEW]);
			  }
			  else
			  {
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 1);
	  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, fabs(Velocontrol.Output[NEW]));
			  }
	  }
		  else
		  {
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);

		  }
	  }
	  else if(start == 2)
	  {
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
		Velocontrol.Error[NEW] = 0;
		Velocontrol.Error[OLD] = 0;
		Velocontrol.Error[OLDER] = 0;
		Velocontrol.Output[NEW] = 0;
		Velocontrol.Output[OLD] = 0;
		Velocontrol.Output[OLDER] = 0;

		Poscontrol.Error[NEW] = 0;
		Poscontrol.Error[OLD] = 0;
		Poscontrol.Error[OLDER] = 0;
		Poscontrol.Output[NEW] = 0;
		Poscontrol.Output[OLD] = 0;
		Poscontrol.Output[OLDER] = 0;
		t = 0;
		static uint64_t timestamp4 =0;
		if(currentTime > timestamp4)
		{
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
			timestamp4 =currentTime + 200000;
		}

	  }
	  else
	  {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
	  }



	  //	  if(currentTime > timestamp3)
	  //	  {
	  //	  dataSend = RPSspeed;
	  //	  dataBytes[0] = header; // Header byte
	  //	  dataBytes[1] = (uint8_t)(dataSend & 0xFF); // Lower byte
	  //	  dataBytes[2] = (uint8_t)((dataSend >> 8) & 0xFF); // Upper byte
	  //	  dataBytes[3] = 0x0A;
	  //	  HAL_UART_Transmit(&hlpuart1, dataBytes, sizeof(dataBytes), 10);
	  //	  timestamp3 =currentTime + 1000;
	  //	  }
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.GainCompensation = 0;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T8_TRGO;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */

  /** Common config
  */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc3.Init.Resolution = ADC_RESOLUTION_12B;
  hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc3.Init.GainCompensation = 0;
  hadc3.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc3.Init.LowPowerAutoWait = DISABLE;
  hadc3.Init.ContinuousConvMode = ENABLE;
  hadc3.Init.NbrOfConversion = 2;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.DMAContinuousRequests = ENABLE;
  hadc3.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc3.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc3, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 115200;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 57343;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 42499;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 169;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4294967295;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 169;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 65535;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PC0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC1 PC2 PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 LD2_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
if(htim == &htim5)
{
_micros += UINT32_MAX;
}
}

uint64_t micros()
{
return __HAL_TIM_GET_COUNTER(&htim5)+_micros;
}

void QEIEncoderVel_Update()
{
//collect data
QEIdata.TimeStamp[NEW] = micros();
QEIdata.Position[NEW] = __HAL_TIM_GET_COUNTER(&htim3);
//Postion 1 turn calculation
QEIdata.QEIPostion_1turn = QEIdata.Position[NEW] % 8192;
//calculate dx
int32_t diffPosition = QEIdata.Position[NEW] - QEIdata.Position[OLD];
//Handle Warp around
if(diffPosition > 28672)
{
diffPosition -=57344;
Count-=1;
}
if(diffPosition < -28672)
{
diffPosition +=57344;
Count+=1;
}
//calculate dt
float diffTime = (QEIdata.TimeStamp[NEW]-QEIdata.TimeStamp[OLD]) * 0.000001;
//calculate anglar velocity
QEIdata.QEIAngularVelocity = diffPosition / diffTime;
RPSspeed = ((QEIdata.QEIAngularVelocity)/8192)*60;
speed = ((QEIdata.QEIAngularVelocity)/8192)*12.5*2*3.14;
speed_fill = (0.969*speed_fill_1)+(0.0155*speed)+(0.0155*speed_1);
speed_1 = speed;
speed_fill_1 = speed_fill;
if(speed_fill>MAXspeed)
{
	MAXspeed = speed_fill;
}
//store value for next loop
QEIdata.Position[OLD] = QEIdata.Position[NEW];
QEIdata.TimeStamp[OLD]=QEIdata.TimeStamp[NEW];
}

void UARTInterruptConfig()
{
	HAL_UART_Receive_IT(&hlpuart1, Rx,4);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if(huart == &hlpuart1)
	{
		Rx[4] = '\0';
		HAL_UART_Receive_IT(&hlpuart1, Rx, 4);
	}
}

void Joy_State()
{
joySW = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);
if(state == 0)
{
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
//	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1,0);
	if (joyY > 4000)
	{
		__HAL_TIM_SET_COUNTER(&htim3, 0);
	}
}
else if(state == 1)
{
	static uint64_t timestamp4 =0;
	if(timestamp4 < HAL_GetTick())
	{
		if(joyX > 4000)
		{
			A = A-250;
		}
		if(joyX < 2000)
		{
			A = A+250;
		}
		if(A <= 0)A=0;
		timestamp4 = HAL_GetTick()+100;
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	}
	if(joyY > 4000 || joyY < 2000)
	{
		if (joyY > 4000)
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 1);
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, A);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, A);
		}
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
	}
}
else if(state == 2)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,1);
	if(joyY > 3200 || joyY < 2700)
	{
		if (joyY > 3200)
		{
			A = 10625;
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 1);
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 2125);
		}
		else
		{
			A = -10625;
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 2125);
		}
	}
	else
	{
		A = 0;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1,0);
	}
 }
	else
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1,0);
	}
	static uint64_t timestamp2 =0;
	if(timestamp2 < HAL_GetTick())
	{
		if (joySW == 0)
		{
			switch (state)
			{
			case 0:
				state = 1;
				while(joySW == 0)
				{
					joySW = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);
				}
				break;
			case 1:
				state = 2;
				while(joySW == 0)
				{
					joySW = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);
				}
				break;
			case 2:
				state = 3;
				while(joySW == 0)
				{
					joySW = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);
				}
				break;
			case 3:
				state = 0;
				while(joySW == 0)
				{
					joySW = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);
				}
				break;
			}
		}
		timestamp2 = HAL_GetTick() + 100;
	}
	Joy_Averaged();
}

void Joy_Averaged()
{
	for (int i = 0; i < 20; i++)
	{
		joySum[0] += joyAnalogRead[2*i];
		joySum[1] += joyAnalogRead[1+(2*i)];
	}

	for (int i = 0; i < 2; i++)
	{
		joyAvg[i] = joySum[i] / 20;
		joySum[i] = 0;
	}

	joyX = joyAvg[0];
	joyY = joyAvg[1];
}
void Trajectory()
{
	static uint64_t timestamp_Traject =0;
		int64_t currentTime = micros();
		  if(timestamp_Traject < currentTime)
		  {
			  switch(state_Tra)
			  	  		  {
			  	  		  case 0:
			  	  			  if(Pos_Start > Pos_Target)
			  	  			  {
			  	  				  direction = -1;
			  	  				  m_Direction = -1;
			  	  			  }
			  	  			  else
			  	  			  {
			  	  				  direction = 1;
			  	  				  m_Direction = 1;
			  	  			  }
			  	  			  	q_Pos = Pos_Start + (Velo_Start*t)+((direction*0.5*Max_Acc)*(t*t));
			  	  			  	q_Velo = Velo_Start + direction*Max_Acc*t;
			  	  			  	q_Acc = Max_Acc*direction;
			  	  			  if(fabs(q_Velo) >= Max_Velo)
			  	  				{
			  	  					state_Tra = 1;
			  	  					Pos_Start = q_Pos;
			  	  					Velo_Start = q_Velo;
			  	  					t = 0;
			  	  				}
			  	  			  if(fabs((q_Pos-Pos_Start)*2) >= fabs(Pos_Target-Pos_Start))
			  	  			  {
			  	  				state_Tra = 2;
								Pos_Start = q_Pos;
								Velo_Start = q_Velo;
								t = 0;
			  	  			  }
			  	  			  	break;
			  	  		  case 1:
			  	  			  	q_Pos = Pos_Start +(Velo_Start*t);
			  	  			  	q_Velo = Velo_Start;
			  	  			  	q_Acc = 0;
			  	  			  if(Pos_Target-q_Pos <= (q_Velo*q_Velo)/(2*Max_Acc))
			  	  				{
			  	  					state_Tra = 2;
			  	  					Pos_Start = q_Pos;
			  	  					Velo_Start = q_Velo;
			  	  					t = 0;
			  	  				}
			  	  			  	break;
			  	  		  case 2:
//			  	  			  	if(direction == -1)
//			  	  			  	{
//			  	  			  	 m_Direction = 1;
//			  	  			  	}
			  	  			  	q_Pos = Pos_Start + (Velo_Start*t)-((direction*0.5*Max_Acc)*(t*t));
			  	  			  	q_Velo = Velo_Start - direction*Max_Acc*t;
			  	  			  	q_Acc = -direction*Max_Acc;
			  	  			  if(fabs(q_Velo) <= 0)
			  	  				{
			  	  					state_Tra = 3;
			  	  					t = 0;
			  	  				}
			  	  			  	break;
			  	  		  case 3:
			  	  			  	if(Pos_Target != Old_Target)
			  	  			  	{


			  	  			  		state_Tra = 0;
			  	  			  		t = 0;
			  	  			  		Pos_Start = q_Pos;
			  	  			  		Velo_Start = q_Velo;
			  	  			  		Old_Target = Pos_Target;


			  	  			  		Velocontrol.Error[NEW] = 0;
									Velocontrol.Error[OLD] = 0;
									Velocontrol.Error[OLDER] = 0;
									Velocontrol.Output[NEW] = 0;
									Velocontrol.Output[OLD] = 0;
									Velocontrol.Output[OLDER] = 0;

									Poscontrol.Error[NEW] = 0;
									Poscontrol.Error[OLD] = 0;
									Poscontrol.Error[OLDER] = 0;
									Poscontrol.Output[NEW] = 0;
									Poscontrol.Output[OLD] = 0;
									Poscontrol.Output[OLDER] = 0;

			  	  			  	}
			  	  			  	break;
			  	  		  }
//			  error_Velo = q_Velo-speed;
//			  output = ((2*kp_Velo*T*error_Velo) - (2*kp_Velo*T*error_Velo_2) +(ki_Velo*T*T*error_Velo)+(2*ki_Velo*T*T*error_Velo_1)+(ki_Velo*T*T*error_Velo_2)+(4*kd_Velo*error_Velo)-(8*kd_Velo*error_Velo_1)-(4*kd_Velo*error_Velo_2)+(2*output_2*T))/(2*T);
//			  error_Velo_2 = error_Velo_1;
//			  error_Velo_1 = error_Velo;
//			  output_2 = output_1;
//			  output_1 = output;
		  t = t+0.0001;
		  timestamp_Traject = currentTime + 100;
		  }
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_13)
	{
		start  = 1;
	}
	if(GPIO_Pin == GPIO_PIN_4)
	{
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
//		__HAL_TIM_SET_COUNTER(&htim3,0);
		start = 2;
	}
	if(GPIO_Pin == GPIO_PIN_5)
	{
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		start = 2;
	}
}
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

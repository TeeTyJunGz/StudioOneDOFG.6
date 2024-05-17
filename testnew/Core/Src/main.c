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
#include "ModBusRTU.h"
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
ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc3;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim16;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */
//ModBus
ModbusHandleTypedef hmodbus;
u16u8_t registerFrame[200];

int Z_Moveing_Status=0b0000;
float Z_Actual_Position= 0;
float Z_Moveing_Speed= 0;
float Z_Acceleration= 0;
float X_Actual_Position= 0;

int Mode = 0;

int Vacuum_Status;
int Gripper_Movement_Status;
int Gripper_Movement_Actual_Status = 0b0000;
int Shelve_Position[5];
int Jogging_Status=0;
int Goal_Point;
int Pick_Order[5];
int Place_Order[5];
//ModBus

//TEST
int A = 2000;
uint8_t B = 0;
uint16_t C = 50;
int test;
int test2;
int Maxtest=0;
int start = 0;
int state_ALL = 2;
int state_ALL_Old = 0;
int state_ALL_Freeze = 0;
int state_Griper = 0;
int state_Pick_Place = 0;
int HOME = 0;
int ButtonTest[6];
int LeadSW[2];
int CountSetpoint;
int CountGriper;
int CountVacuum;
int CountHome;
int CountProxi;
int VacuumNF;
float MemPos[5];
int Move[10] = {5,1,4,3,1,2,3,5,2,4};
int i = 0;
int Pick = 0;
int Place = 0;
int emer;
int resetBut;
int FirstTraject = 1;
int waitforGripper = 0;
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
PID_StructureTypeDef Velocontrol2 = {0};
PID_StructureTypeDef Poscontrol2 = {0};
//PID

//Trajectory
float Pos_Start = 0;
float Pos_Target = 0;
float Old_Target = 0;
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
float speed;
float speed_1;
float MAXspeed;
float speed_fill;
float speed_fill_1;
float Pos;
float Accelation;
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
float QEIAngularAccelation;
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
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM5_Init(void);
static void MX_ADC3_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM16_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */
uint64_t micros();
void QEIEncoderVel_Update();
void QEIEncoderPos_Update();
void Joy_Averaged();
void Joy_State();
void Trajectory();
void Motor();
void reset();
void resetHome();
void PIDreset();
void PIDposition();
void PIDposition2();
void SensorRead();
void ButtonMem();
void Stopper();
//ModBus
void State_To_Mode();
void Heartbeat();
void Routine();
void Vacuum();
void GripperMovement();
void SetShelves();
void Run_Point_Mode();
void SetHome();
void Run_Jog_Mode();
void UpdatePosRoutine();
//ModBus

//MATH
void splitInteger(int number, int *digitsArray);
//
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
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM8_Init();
  MX_TIM5_Init();
  MX_ADC3_Init();
  MX_USART2_UART_Init();
  MX_TIM16_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim4);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL);
  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim5);
  HAL_ADCEx_Calibration_Start(&hadc3, ADC_SINGLE_ENDED);
  HAL_ADC_Start_DMA(&hadc3, joyAnalogRead, 40);
  _micros = 0;

	Velocontrol.kp = 162;//225;//180
	Velocontrol.ki = 0;//2;//25
	Velocontrol.kd = 0;
	Velocontrol.T = 0.0001;

	Poscontrol.kp = 1;//0.5667;//2.015,0.6
	Poscontrol.ki = 0.4;//1.29;//4.5425,1.34
	Poscontrol.kd = 0.0000007;//0.000001;//0.0000021,0.0000004
	Poscontrol.T = 0.0001;

	Velocontrol2.kp = 50;//225;//180
	Velocontrol2.ki = 1;//2;//25
	Velocontrol2.kd = 0;
	Velocontrol2.T = 0.0001;

	Poscontrol2.kp = 10;//0.5667;//2.015,0.6
	Poscontrol2.ki = 1;//1.29;//4.5425,1.34
	Poscontrol2.kd = 0.0000007;//0.000001;//0.0000021,0.0000004
	Poscontrol2.T = 0.0001;

	hmodbus.huart = &huart2;
	hmodbus.htim = &htim16;
	hmodbus.slaveAddress = 0x15;
	hmodbus.RegisterSize =200;
	Modbus_init(&hmodbus, registerFrame);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //ModBus
	  Modbus_Protocal_Worker();
	  State_To_Mode();
	  UpdatePosRoutine();
//	  Stopper();
//	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0,0);
//	  HAL_Delay(1000);
//	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_3);//idle
//	  HAL_Delay(1000);
//	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_2);//run
//	  HAL_Delay(1000);
//	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_5);//PULL/PUSH
//	  HAL_Delay(1000);
//	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);//Vacuum
//	  HAL_Delay(1000);
//	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);//PULL/PUSH
//	  HAL_Delay(1000);
	  //ModBus

	  //Timer SET
	  int64_t currentTime = micros();
	  static uint64_t timestamp =0;
	  static uint64_t timestamp5 =0;
	  //Timer SET

		//ReadSensor
		if(currentTime > timestamp)
			{
			QEIEncoderPos_Update();
			QEIEncoderVel_Update();
			timestamp =currentTime + 50;//us
			}
		SensorRead();
		//ReadSensor

	  switch (state_ALL)
	  			{
	  			case 0://HOME
	  				SetHome();
	  				break;
	  			case 1://Standby
	  				if(start == 1)//MODBUS HERE
	  				{
	  					state_ALL = 2;
	  				}
	  				if(ButtonTest[1] == 0)//MODBUS HERE
					{
						state_ALL = 4;
					}
	  				if(start == 1)//HOME Button
					{
						state_ALL = 2;
					}
	  				break;
	  			case 2://JOY SET
	  				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3,1);
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2,0);
	  				Joy_State();
	  				Vacuum();
	  				GripperMovement();
	  				if(ButtonTest[0] == 0)//HOME Button
					{
	  					Pos_Target = 0;
						state_ALL = 0;
						Mode = 0;
					}
	  				if(ButtonTest[1] == 0)
					{
						state_ALL = 4;
					}
	  				if(ButtonTest[2] == 0)
					{
						state_ALL = 5;
					}
	  				if(ButtonTest[3] == 0)
					{
	  					Trajectory();
					}
	  				if(start == 1)
	  				{
//	  					state_ALL = 3;
//	  					start = 0;
						Trajectory();
	  				}
	  				break;
	  			case 3://PID
	  				if(Mode == 1)
	  				{
	  					Trajectory();
						if(CountSetpoint < 100)
						{
							PIDposition();
						}
						else
						{
							if(state_ALL_Old == 5 || state_ALL_Old == 0 || state_ALL_Old == 7 || state_ALL_Old == 8)
							{
							HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
							__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
							reset();
							state_ALL = 6;
	//						state_ALL = 2;
							CountSetpoint = 0;
							}
							else
							{
							state_ALL = 2;
							CountSetpoint = 0;
							HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
							__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
							reset();
							}
							registerFrame[0x10].U16 = 0b0000;

						}
						if(ButtonTest[0] == 0)//HOME Button
						{
							state_ALL = 0;
							Mode = 0;
							reset();
						}
						if(fabs(Pos-Pos_Target) <= 0.8)//Limit
						{
							if(currentTime > timestamp5)
						  {
								CountSetpoint++;
								timestamp5 =currentTime + 1000;
						  }
						}
						else
						{
							CountSetpoint = 0;
						}
	  				}
	  				else
	  				{
	  					Trajectory();
						if(CountSetpoint < 100)
						{
							PIDposition();
						}
						else
						{
							if(state_ALL_Old == 5 || state_ALL_Old == 0)
							{
							HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
							__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
							reset();
//							state_ALL = 6;
							state_ALL = 2;
							CountSetpoint = 0;
							}
							else
							{
							state_ALL = 2;
							CountSetpoint = 0;
							HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
							__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
							reset();
							}

						}
						if(ButtonTest[0] == 0)//HOME Button
						{
							state_ALL = 0;
							Mode = 0;
							reset();
						}
						if(fabs(Pos-Pos_Target) <= 0.8)//Limit
						{
							if(currentTime > timestamp5)
						  {
								CountSetpoint++;
								timestamp5 =currentTime + 1000;
						  }
						}
						else
						{
							CountSetpoint = 0;
						}
	  				}

	  				break;
	  			case 4://TEST
	  				Joy_State();
	  				ButtonMem();
					break;
	  			case 5://JOG MODE
	  				if(Mode == 1)
	  				{
						switch(state_Pick_Place){
							case 0:
								registerFrame[0x10].U16 = 0b0100;
								Pos_Target = MemPos[Pick_Order[Pick]-1];
								Pick++;
								state_ALL = 3;
								state_ALL_Old = 7;
								state_Pick_Place = 1;

								PIDreset();
								break;
							case 1:
								registerFrame[0x10].U16 = 0b1000;
								Pos_Target = MemPos[Place_Order[Place]-1]+5;
								Place++;
								state_ALL = 3;
								state_ALL_Old = 8;
								state_Pick_Place = 0;
								if(Place == 5)
								{
									state_ALL = 0;
									Place = 0;
									Pick = 0;
									Mode = 0;
									registerFrame[0x10].U16 = 0b0000;
								}

								PIDreset();
								break;
	  					}
	  				}
	  				else
	  				{
	  					Pos_Target = MemPos[i];
						state_ALL = 3;
						state_ALL_Old = 5;
						i++;
						if(i == 5)
						{
						state_ALL_Old = 0;
						i = 0;
						}

						PIDreset();
	  				}
					break;
	  			case 6://Gripper
	  				switch (state_Griper)
	  				{
	  					case 0:
//	  							HAL_TIM_Encoder_Stop(&htim3, TIM_CHANNEL_ALL);
	  							HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,1);
								if(LeadSW[0] == 0 || (LeadSW[1] == 1 && LeadSW[0] == 1))
								{
									//PUSH
									HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,1);
									HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,0);
								}
								else if(LeadSW[1] == 0)
								{
									if(CountGriper > 100)
									{
										HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,0);
										HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,0);
										state_Griper = 1;
										CountGriper = 0;
										if(VacuumNF == 0)
										{
											HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,1);
											//ON_Vacuum
											VacuumNF = 1;
										}
										else
										{
											HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,0);
											//OFF_Vacuum
											VacuumNF = 0;
										}
										if(state_ALL_Old == 8)
										{
											HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,0);
											//OFF_Vacuum
										}
									}
									else
									{
										static uint32_t timeGriper = 0;
										if(timeGriper < HAL_GetTick())
										{
											CountGriper++;
											timeGriper = HAL_GetTick()+10;
										}

									}

								}
	  						break;
	  					case 1:
//	  							static uint32_t timeVacuum = 0;
//								if(timeVacuum < HAL_GetTick())
//								{
//									CountVacuum++;
//									timeVacuum = HAL_GetTick()+10;
//								}
//								if(CountVacuum < 500)
//								{
//									//ON-OFF Vacuum
//									if(state_ALL_Old == 7)
//									{
//										HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,1);
//										//ON_Vacuum
//									}
//									if(state_ALL_Old == 8)
//									{
//										HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,0);
//										//OFF_Vacuum
//									}
//									else
//									{
//										if(VacuumNF == 1)
//										{
//											HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,1);
//											//ON_Vacuum
//											VacuumNF = 0;
//										}
//										else
//										{
//											HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,0);
//											//OFF_Vacuum
//											VacuumNF = 1;
//										}
//									}
//
//								}
//								else
//								{
//									state_Griper = 2;
//									CountVacuum = 0;
//								}
	  							state_Griper = 2;
								break;
	  					case 2:
	  							if(LeadSW[1] == 0 || (LeadSW[1] == 1 && LeadSW[0] == 1))
								{
	  								//PULL
	  								HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,0);
	  								HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,1);
								}
	  							else if(LeadSW[0] == 0)
								{
	  								if(CountGriper > 100)
									{
	  									HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,0);
	  									HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,0);
	  									state_Griper = 0;
										CountGriper = 0;
										if(state_ALL_Old == 5 || state_ALL_Old == 7 || state_ALL_Old == 8)
										{
											state_ALL = 5;
										}
										else
										{
											state_ALL = 0;
											Mode = 0;
										}
//										HAL_Delay(1000);
//										HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);

									}
									else
									{
										static uint32_t timeGriper = 0;
										if(timeGriper < HAL_GetTick())
										{
											CountGriper++;
											timeGriper = HAL_GetTick()+10;
										}

									}

								}

	  						break;

	  				}


	  				break;
	  				case 7://FREEZE

	  					break;
	  			}
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
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 3;
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
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 16999;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 1999;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

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
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 169;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 1145;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OnePulse_Init(&htim16, TIM_OPMODE_SINGLE) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 19200;
  huart2.Init.WordLength = UART_WORDLENGTH_9B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_EVEN;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

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
  HAL_GPIO_WritePin(GPIOC, LAMP1_Pin|LAMP2_Pin|PUSH_Pin|Vacuum_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DirectionMotor_Pin|AnotherDMotor_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PULL_GPIO_Port, PULL_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SWJoy_Pin LeadSW1_Pin Floor4_Pin */
  GPIO_InitStruct.Pin = SWJoy_Pin|LeadSW1_Pin|Floor4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LAMP1_Pin LAMP2_Pin PULL_Pin PUSH_Pin
                           Vacuum_Pin */
  GPIO_InitStruct.Pin = LAMP1_Pin|LAMP2_Pin|PULL_Pin|PUSH_Pin
                          |Vacuum_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : DirectionMotor_Pin AnotherDMotor_Pin LD2_Pin */
  GPIO_InitStruct.Pin = DirectionMotor_Pin|AnotherDMotor_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ResetSw_Pin */
  GPIO_InitStruct.Pin = ResetSw_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ResetSw_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LeadSW2_Pin Floor1_Pin Floor5_Pin */
  GPIO_InitStruct.Pin = LeadSW2_Pin|Floor1_Pin|Floor5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : EmergencySw_Pin */
  GPIO_InitStruct.Pin = EmergencySw_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(EmergencySw_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Floor2_Pin Floor3_Pin HomeButton_Pin */
  GPIO_InitStruct.Pin = Floor2_Pin|Floor3_Pin|HomeButton_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : Lowerprox_Pin UpperProx_Pin */
  GPIO_InitStruct.Pin = Lowerprox_Pin|UpperProx_Pin;
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
void Joy_State()
{
joySW = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);
if(state == 0)
{
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
//	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1,0);
	if (joyY > 3000)
	{
		__HAL_TIM_SET_COUNTER(&htim3, 0);
	}
	static uint64_t timestamp4 =0;
	if(timestamp4 < HAL_GetTick())
	{
		if(joyX > 3000)
		{
			X_Actual_Position = X_Actual_Position-10;
		}
		if(joyX < 1600)
		{
			X_Actual_Position = X_Actual_Position+10;
		}
		if(X_Actual_Position <= 0)X_Actual_Position=0;
		timestamp4 = HAL_GetTick()+100;
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	}
}

else if(state == 1)
{
	static uint64_t timestamp4 =0;
	if(timestamp4 < HAL_GetTick())
	{
		if(joyX > 3000)
		{
			A = A-250;
		}
		if(joyX < 1600)
		{
			A = A+250;
		}
		if(A <= 0)A=0;
		timestamp4 = HAL_GetTick()+100;
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	}
	if(joyY > 3000 || joyY < 1600)
	{
		if (joyY > 3000)
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
	if(joyY > 3000 || joyY < 1600)
	{
		if (joyY > 3000)
		{
			A = 10625;
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 1);
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 10250);
		}
		else
		{
			A = -10625;
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 10250);
		}
	}
	else
	{
		A = 0;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1,0);
	}
 }
//else if(state == 3)
//{
//	static uint32_t timeDelay = 0;
//	if(timeDelay < HAL_GetTick())
//	{
//	if(joyY > 3000 || joyY < 1600)
//	{
//		if (joyY > 3000)
//		{
//			Pos_Target = Pos_Target-10;
//		}
//		else
//		{
//			Pos_Target = Pos_Target+10;
//		}
//		if(Pos_Target < 0)
//		{
//			Pos_Target = 0;
//		}
//		timeDelay = HAL_GetTick()+250;
//	}
//	}
//	else
//	{
//
////		A = 0;
////		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
////		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1,0);
//	}
//	PIDposition2();
//}
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
void ButtonMem(){
	if(Mode == 1)
		{
			if(ButtonTest[1] == 0)
			{
				registerFrame[0x23].U16 = Pos*10;
				MemPos[0] = Pos;
			}
			if(ButtonTest[2] == 0)
			{
				registerFrame[0x24].U16 = Pos*10;
				MemPos[1] = Pos;
			}
			if(ButtonTest[3] == 0)
			{
				registerFrame[0x25].U16 = Pos*10;
				MemPos[2] = Pos;
			}
			if(ButtonTest[4] == 0)
			{
				registerFrame[0x26].U16 = Pos*10;
				MemPos[3] = Pos;
			}
			if(ButtonTest[5] == 0)
			{
				registerFrame[0x27].U16 = Pos*10;
				MemPos[4] = Pos;
			}
			if(ButtonTest[0] == 0)//HOME Button
			{
				registerFrame[0x10].U16 = 0b0000;
				resetHome();
				Pos_Target = 0;
				Mode = 0;
				state_ALL = 0;
			}
		}
		else
		{
			if(ButtonTest[1] == 0)
			{
				MemPos[0] = Pos;
			}
			if(ButtonTest[2] == 0)
			{
				MemPos[1] = Pos;
			}
			if(ButtonTest[3] == 0)
			{
				MemPos[2] = Pos;
			}
			if(ButtonTest[4] == 0)
			{
				MemPos[3] = Pos;
			}
			if(ButtonTest[5] == 0)
			{
				MemPos[4] = Pos;
			}
			if(ButtonTest[0] == 0)//HOME Button
			{
				state_ALL = 0;
				Mode = 0;
				Pos_Target = 0;
				resetHome();
			}
		}
}

void reset(){
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

	Pos_Start = Pos;
	Velo_Start = 0;
	t = 0;
}
void resetHome(){
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
	Velocontrol.Error[NEW] = 0;

	Velocontrol2.Error[OLD] = 0;
	Velocontrol2.Error[OLDER] = 0;
	Velocontrol2.Output[NEW] = 0;
	Velocontrol2.Output[OLD] = 0;
	Velocontrol2.Output[OLDER] = 0;

	Poscontrol2.Error[NEW] = 0;
	Poscontrol2.Error[OLD] = 0;
	Poscontrol2.Error[OLDER] = 0;
	Poscontrol2.Output[NEW] = 0;
	Poscontrol2.Output[OLD] = 0;
	Poscontrol2.Output[OLDER] = 0;
	Pos_Start = 0;
	Velo_Start = 0;
	t = 0;
}
void PIDreset(){
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
void QEIEncoderPos_Update()
{
	QEIReadRaw = __HAL_TIM_GET_COUNTER(&htim3);
//	Pos = ((QEIReadRaw)*25*3.14)/8192;
	if(Count >= 0)
	{
//		Pos = ((QEIReadRaw+(Count*57344))*25*3.14)/8192;
		Pos = ((QEIReadRaw)*25*3.14)/8192;

	}
	else
	{
//		Pos	= ((QEIReadRaw-(fabs(Count)*57344))*25*3.14)/8192;
		Pos	= -(((65535-QEIReadRaw)*25*3.14)/8192);
	}
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
speed = ((QEIdata.QEIAngularVelocity)/8192)*12.5*2*3.14;

speed_fill = (0.969*speed_fill_1)+(0.0155*speed)+(0.0155*speed_1);
speed_1 = speed;
speed_fill_1 = speed_fill;
static uint64_t timestamp_Accelation =0;
if(timestamp_Accelation < HAL_GetTick())
{
Accelation = fabs(speed_fill-speed_fill_1)/0.01;
timestamp_Accelation = HAL_GetTick()+10;
}





if(speed_fill>MAXspeed)
{
	MAXspeed = speed_fill;
}
//store value for next loop
QEIdata.Position[OLD] = QEIdata.Position[NEW];
QEIdata.TimeStamp[OLD]=QEIdata.TimeStamp[NEW];
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
			  	  			  }
			  	  			  else
			  	  			  {
			  	  				  direction = 1;
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
			  	  			  if(fabs(Pos_Target-q_Pos) <= (q_Velo*q_Velo)/(2*Max_Acc))
			  	  				{
			  	  					state_Tra = 2;
			  	  					Pos_Start = q_Pos;
			  	  					Velo_Start = q_Velo;
			  	  					t = 0;
			  	  				}
			  	  			  	break;
			  	  		  case 2:
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
			  	  			  	if (FirstTraject == 1)
			  	  			  	{
			  	  			  	Old_Target = Pos_Target;
			  	  			  	FirstTraject = 0;
			  	  			  	}
			  	  			  	if(Pos_Target != Old_Target)
			  	  			  	{
			  	  			  		state_Tra = 0;
			  	  			  		t = 0;
			  	  			  		Pos_Start = Pos;
//			  	  			  		Pos_Start = q_Pos;
			  	  			  		Velo_Start = q_Velo;
			  	  			  		Old_Target = Pos_Target;
			  	  			  		PIDreset();

			  	  			  	}
			  	  			  	break;
			  	  		  }
		  t = t+0.0001;
		  timestamp_Traject = currentTime + 100;
		  }
}

void PIDposition()
{
	static uint64_t timestamp3 =0;
	int64_t currentTime = micros();
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
			if(Velocontrol.Output[NEW]>42500)Velocontrol.Output[NEW]=42500;
			if(Velocontrol.Output[NEW]<-42500)Velocontrol.Output[NEW]=-42500;
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
void PIDposition2()
{
	static uint64_t timestamp3 =0;
	int64_t currentTime = micros();
	if(currentTime > timestamp3)
	  {
			Poscontrol2.Error[NEW] = Pos_Target-Pos;
			Poscontrol2.Output[NEW] = ((((Poscontrol2.kp*Poscontrol2.T)+(Poscontrol2.ki*Poscontrol2.T*Poscontrol2.T)+(Poscontrol2.kd))*Poscontrol2.Error[NEW])-(((Poscontrol2.kp*Poscontrol2.T)+(Poscontrol2.kd))*Poscontrol2.Error[OLD])+(Poscontrol2.kd*Poscontrol2.Error[OLDER])+(Poscontrol2.Output[OLD]*Poscontrol2.T))/Poscontrol2.T;
			Poscontrol2.Error[OLDER] = Poscontrol2.Error[OLD];
			Poscontrol2.Error[OLD] = Poscontrol2.Error[NEW];
			Poscontrol2.Output[OLDER] = Poscontrol2.Output[OLD];
			Poscontrol2.Output[OLD] = Poscontrol2.Output[NEW];

			Velocontrol2.Error[NEW] = Poscontrol2.Output[NEW]-speed_fill;
			Velocontrol2.Output[NEW] = ((((Velocontrol2.kp*Velocontrol2.T)+(Velocontrol2.ki*Velocontrol2.T*Velocontrol2.T)+(Velocontrol2.kd))*Velocontrol2.Error[NEW])-(((Velocontrol2.kp*Velocontrol2.T)+(Velocontrol2.kd))*Velocontrol2.Error[OLD])+(Velocontrol2.kd*Velocontrol2.Error[OLDER])+(Velocontrol2.Output[OLD]*Velocontrol2.T))/Velocontrol2.T;
			if(Velocontrol2.Output[NEW]>42500)Velocontrol2.Output[NEW]=42500;
			if(Velocontrol2.Output[NEW]<-42500)Velocontrol2.Output[NEW]=-42500;
			Velocontrol2.Error[OLDER] = Velocontrol2.Error[OLD];
			Velocontrol2.Error[OLD] = Velocontrol2.Error[NEW];
			Velocontrol2.Output[OLDER] = Velocontrol2.Output[OLD];
			Velocontrol2.Output[OLD] = Velocontrol2.Output[NEW];
	//					if(fabs(Pos_Target-Pos) <= 0.1)start = 0;
			timestamp3 =currentTime + 100;
	  }
	  if(Velocontrol2.Output[NEW] > 0)
	  {
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
		  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, Velocontrol2.Output[NEW]);
	  }
	  else
	  {
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 1);
		  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, fabs(Velocontrol2.Output[NEW]));
	  }
}

void State_To_Mode(){
        if(registerFrame[0x01].U16 == 0b0001){
        	registerFrame[0x01].U16 = 0b0000;
			registerFrame[0x10].U16 = 0b0001;
        	Mode = 1;
        	state_ALL = 4;
        	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3,0);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2,1);
        }
        if(registerFrame[0x01].U16 == 0b0010){
//            Mode = 2;
        	Mode = 1;
            state_ALL = 0;
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3,0);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2,1);
        }
        if(registerFrame[0x01].U16 == 0b0100){
//            Mode = 3;
        	Mode = 1;
        	registerFrame[0x01].U16 = 0b0000;
			splitInteger(registerFrame[0x21].U16, Pick_Order);
			splitInteger(registerFrame[0x22].U16, Place_Order);
			state_ALL = 5;
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3,0);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2,1);
        }
        if(registerFrame[0x01].U16 == 0b1000){
//          Point Mode
        	Mode = 1;
        	registerFrame[0x01].U16 = 0b0000;
			registerFrame[0x10].U16 = 0b00010000;
			Goal_Point = registerFrame[0x30].U16/10;
			Pos_Target = Goal_Point;
			state_ALL = 3;
			state_ALL_Old = 2;
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3,0);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2,1);
        }
}

void Heartbeat(){
        registerFrame[0x00].U16 =22881;
    }

void Routine () {
        if (registerFrame[0x00].U16 == 18537){
//            registerFrame[0x04].U16 = Gripper_Movement_Actual_Status;
            registerFrame[0x10].U16 = 0b0001;
            registerFrame[0x10].U16 = 0b0000;
        }
}
void UpdatePosRoutine()
{
	if(LeadSW[0] == 0)
	{
		registerFrame[0x04].U16 = 0b0001;
	}
	else if(LeadSW[1] == 0)
	{
		registerFrame[0x04].U16 = 0b0010;
	}
	else
	{
		registerFrame[0x04].U16 = 0b0000;
	}
	registerFrame[0x11].U16 = fabs(Pos)*10;
	registerFrame[0x12].U16 = fabs(speed_fill)*10;
	registerFrame[0x13].U16 = Accelation*10;
	registerFrame[0x40].U16 = X_Actual_Position*10;
}
void Vacuum() {
        Vacuum_Status = registerFrame[0x02].U16;
        if(Vacuum_Status == 1)
        {
        	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,1);
        }
        else
        {
        	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,0);
        }
}
//Write Gripper Movement Status
void GripperMovement() {
        Gripper_Movement_Status = registerFrame[0x03].U16;
        if( Gripper_Movement_Status == 1)
	   {
        	if(LeadSW[1] == 0)
			{
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,0);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,0);
			}
			else
			{
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,1);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,0);
			}

	   }
        else if(Gripper_Movement_Status == 0)
        {
        	if(LeadSW[0] == 0)
        	{
        		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,0);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,0);
        	}
        	else
        	{
        		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,0);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,1);
        	}
        }
	   else
	   {
		   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,0);
		   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,0);
	   }
}
void SetHome() {
	if(Mode == 1)
		{
			registerFrame[0x01].U16 = 0b0000;
			registerFrame[0x10].U16 = 0b0010;
			test = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4);
			if(HOME == 1||HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == 1)
			{
				if(CountHome > 100)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 1);
					__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
					HAL_Delay(1000);
					HOME = 0;
					__HAL_TIM_SET_COUNTER(&htim3,0);
					Count = 0;
					QEIdata.Position[NEW] = 0;
					QEIdata.Position[OLD] = 0;
					Pos_Start = 0;
					state_ALL = 2;
					state_Tra = 0;
					CountHome = 0;
					registerFrame[0x10].U16 = 0b0000;//Reset ModBus
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3,1);
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2,0);
					resetHome();
				}
				else
				{
					CountHome++;
				}
			}
			else
			{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 1);
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 7500);
			CountHome = 0;
			}
		}
		else
		{
			test = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4);
			if(HOME == 1||HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == 1)
			{
				if(CountHome > 100)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 1);
					__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
					HAL_Delay(1000);
					HOME = 0;
					__HAL_TIM_SET_COUNTER(&htim3,0);
					Count = 0;
					QEIdata.Position[NEW] = 0;
					QEIdata.Position[OLD] = 0;
					Pos_Start = 0;
					state_ALL = 2;
					state_Tra = 0;
					CountHome = 0;
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3,1);
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2,0);
					resetHome();
				}
				else
				{
					CountHome++;
				}
			}
			else
			{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 1);
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 7500);
			CountHome = 0;
			}
		}
    }

void splitInteger(int number, int *digitsArray) {
    int temp = number;
    for (int b = 0; b < 5; b++) {
        digitsArray[4 - b] = temp % 10;  // Getting the last digit
        temp /= 10;  // Removing the last digit
    }
}
void SensorRead()
{
	ButtonTest[0] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10);
	ButtonTest[1] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);
	ButtonTest[2] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);
	ButtonTest[3] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9);
	ButtonTest[4] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);
	ButtonTest[5] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);
	LeadSW[0] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	LeadSW[1] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1);
	test = LeadSW[1] == 0 || (LeadSW[1] == 1 && LeadSW[0] == 1);
	test2 = LeadSW[0] == 0 || (LeadSW[1] == 1 && LeadSW[0] == 1);
	emer = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4); //emer
	resetBut = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2); //reset
}
//void Stopper(){
//	if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == 1 || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 1)
//	{
//		if(CountProxi > 100)
//		{
////			state_ALL_Freeze = state_ALL;
////			state_ALL = 2;
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 1);
//			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
//		}
//		else
//		{
//			CountProxi++;
//		}
//	}
//	else
//	{
//		CountProxi = 0;
//	}
//}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
if(htim == &htim5)
{
_micros += UINT32_MAX;
}
if(htim == &htim6)
{
	Heartbeat();
	Routine();
}
}
uint64_t micros()
{
return __HAL_TIM_GET_COUNTER(&htim5)+_micros;
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_13)
	{
		start  = 1;
	}
//	if(GPIO_Pin == GPIO_PIN_4)
//	{
//		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
//		HOME = 1;
//	}
//	if(GPIO_Pin == GPIO_PIN_5)
//	{
//		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
//		HOME = 2;
//	}
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

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "mdf.h"
#include "i2c.h"
#include "icache.h"
#include "lpdma.h"
#include "lptim.h"
#include "memorymap.h"
#include "rtc.h"
#include "sdmmc.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fatfs.h"
#include "linked_list.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern SD_HandleTypeDef hsd1;
static char SD_Path[4];
static FATFS SDDISKFatFs;

uint32_t byteswritten;
FRESULT FatFsResult;

MDF_DmaConfigTypeDef         DMAConfig;

/*Buffer location and size should aligned to cache line size (32 bytes) */
__attribute__((section(".sram4"))) static uint16_t RecBuff[REC_BUFF_SIZE * 2U] = {0};
//int16_t        RecBuff[REC_BUFF_SIZE * 2U];
__IO uint32_t  DmaRecHalfBuffCplt  = 0;
__IO uint32_t  DmaRecBuffCplt      = 0;
uint32_t       PlaybackStarted         = 0;

__IO int16_t UpdatePointer = -1;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static void MX_GPDMA1_DeInit(void);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void SystemPower_Config(void);
/* USER CODE BEGIN PFP */
void Error(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void MDF_DMAConfig(void);
void Error(void);

extern DMA_QListTypeDef MDFQueue;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	  FRESULT ReturnResult;
	  FIL MyFile;

	  int numberOfWrites = 128;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the System Power */
  SystemPower_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_LPDMA1_Init();
  HAL_GPIO_WritePin(GPIOA, PWR_3p3V_EN_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, SDCard_En_Pin, GPIO_PIN_SET);
  MX_ADF1_Init();
  MX_LPTIM3_Init();
  MX_SDMMC1_SD_Init();
  MX_I2C3_Init();
  MX_RTC_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */

  HAL_DBGMCU_EnableDBGStopMode();

  FATFS_LinkDriver(&SD_Driver, SD_Path);

    ReturnResult = f_mount(&SDDISKFatFs, (TCHAR const*)SD_Path, 1);
    if(ReturnResult != FR_OK)
    {
      Error_Handler();
    }

    ReturnResult = f_open(&MyFile, "NewAudio1.raw", FA_CREATE_ALWAYS | FA_WRITE);
    if(ReturnResult != FR_OK)
    {
      Error_Handler();
    }

    MX_MDFQueue_Config();

    /* Link SAI queue to DMA channel */
    HAL_DMAEx_List_LinkQ(&handle_LPDMA1_Channel0, &MDFQueue);

    /* Associate the DMA handle */
    __HAL_LINKDMA(&AdfHandle0, hdma, handle_LPDMA1_Channel0);

    MDF_DMAConfig();

    if (HAL_MDF_AcqStart_DMA(&AdfHandle0, &AdfFilterConfig0, &DMAConfig) != HAL_OK)
    {
      Error();
    }


    while (1)
      {
        __HAL_RCC_ADF1_CLKAM_ENABLE(); //ADF1 autonomous mode enable in Stop 0/1/2 mode
        __HAL_RCC_LPDMA1_CLKAM_ENABLE();

        HAL_PWREx_ConfigSRDDomain(PWR_SRD_DOMAIN_RUN);

    	HAL_SuspendTick();
    	HAL_PWREx_EnterSTOP1Mode(PWR_STOPENTRY_WFI);
    	HAL_ResumeTick();

    	__HAL_RCC_HSI_ENABLE();
		__HAL_RCC_PLL_ENABLE();
		__HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVP);

        if(DmaRecHalfBuffCplt == 1)
        {
          FatFsResult = f_write(&MyFile, &RecBuff[0], (UINT)((REC_BUFF_SIZE_DIV2 - 1) * 2), (void *)&byteswritten);
          if(FatFsResult != FR_OK)
          {
            Error();
          }
          DmaRecHalfBuffCplt  = 0;
        }

        if(DmaRecBuffCplt == 1)
        {
          FatFsResult = f_write(&MyFile, &RecBuff[REC_BUFF_SIZE_DIV2], (UINT)(REC_BUFF_SIZE_DIV2 * 2), (void *)&byteswritten);
          if(FatFsResult != FR_OK)
          {
            Error();
          }
          numberOfWrites--;
          DmaRecBuffCplt = 0;
        }

        if(numberOfWrites == 0)
        {
          HAL_MDF_AcqStop_DMA(&AdfHandle0);
          f_close(&MyFile);
          break;
        }
      }

      HAL_MDF_DeInit(&AdfHandle0);
      MX_GPDMA1_DeInit();

      __HAL_RCC_SDMMC1_CLK_DISABLE();
      __HAL_RCC_SDMMC1_CLK_ENABLE();

      MX_LPDMA1_Init();
      MX_ADF1_Init();

      ReturnResult = f_open(&MyFile, "NewAudio2.raw", FA_CREATE_ALWAYS | FA_WRITE);
      if(ReturnResult != FR_OK)
      {
        Error();
      }

      if (HAL_MDF_AcqStart_DMA(&AdfHandle0, &AdfFilterConfig0, &DMAConfig) != HAL_OK)
      {
        Error();
      }

      numberOfWrites = 50;

      while (1)
      {
        if(DmaRecHalfBuffCplt == 1)
        {
          FatFsResult = f_write(&MyFile, &RecBuff[REC_BUFF_SIZE], (UINT)(REC_BUFF_SIZE * 2U), (void *)&byteswritten);
          if(FatFsResult != FR_OK)
          {
            Error();
          }
          DmaRecHalfBuffCplt  = 0;
        }

        if(DmaRecBuffCplt == 1)
        {
          FatFsResult = f_write(&MyFile, &RecBuff[0], (UINT)(REC_BUFF_SIZE * 2U), (void *)&byteswritten);
          if(FatFsResult != FR_OK)
          {
            Error();
          }
          numberOfWrites--;
          DmaRecBuffCplt = 0;
        }

        if(numberOfWrites == 0)
        {
          HAL_MDF_AcqStop_DMA(&AdfHandle0);
          f_close(&MyFile);
          break;
        }
      }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_CRSInitTypeDef RCC_CRSInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_LSE
                              |RCC_OSCILLATORTYPE_MSI|RCC_OSCILLATORTYPE_MSIK;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_1;
  RCC_OscInitStruct.MSIKClockRange = RCC_MSIKRANGE_8;
  RCC_OscInitStruct.MSIKState = RCC_MSIK_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV2;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 14;
  RCC_OscInitStruct.PLL.PLLP = 10;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLLVCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the SYSCFG APB clock
  */
  __HAL_RCC_CRS_CLK_ENABLE();

  /** Configures CRS
  */
  RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;
  RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_LSE;
  RCC_CRSInitStruct.Polarity = RCC_CRS_SYNC_POLARITY_RISING;
  RCC_CRSInitStruct.ReloadValue = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000,32768);
  RCC_CRSInitStruct.ErrorLimitValue = 34;
  RCC_CRSInitStruct.HSI48CalibrationValue = 32;

  HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);
}

/**
  * @brief Power Configuration
  * @retval None
  */
static void SystemPower_Config(void)
{

  /*
   * Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
   */
  HAL_PWREx_DisableUCPDDeadBattery();
/* USER CODE BEGIN PWR */
/* USER CODE END PWR */
}

/* USER CODE BEGIN 4 */
static void MX_GPDMA1_DeInit(void)
{

  /* USER CODE BEGIN GPDMA1_Init 0 */

  /* USER CODE END GPDMA1_Init 0 */

  /* Peripheral clock enable */
  //__HAL_RCC_GPDMA1_CLK_DISABLE();

  /* GPDMA1 interrupt Init */

    //HAL_NVIC_DisableIRQ(GPDMA1_Channel11_IRQn);

  /* USER CODE BEGIN GPDMA1_Init 1 */

  /* USER CODE END GPDMA1_Init 1 */
  /* USER CODE BEGIN GPDMA1_Init 2 */

  /* USER CODE END GPDMA1_Init 2 */

}

void HAL_MDF_AcqHalfCpltCallback(MDF_HandleTypeDef *hmdf)
{
    DmaRecHalfBuffCplt = 1;
}

/**
  * @brief  Regular conversion complete callback.
  * @note   In interrupt mode, user has to read conversion value in this function
            using HAL_MDF_GetAcqValue.
  * @param  hmdf : MDF handle.
  * @retval None
  */
void HAL_MDF_AcqCpltCallback(MDF_HandleTypeDef *hmdf)
{
    DmaRecBuffCplt = 1;
}

static void MDF_DMAConfig(void)
{
  /* Initialize DMA configuration parameters */
  DMAConfig.Address                              = (uint32_t)&RecBuff[0];
  DMAConfig.DataLength                           = (REC_BUFF_SIZE * 2U);
  DMAConfig.MsbOnly                              = ENABLE;
}

void Error(void)
{
  while(1);
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

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mdf.c
  * @brief   This file provides code for the configuration
  *          of the MDF instances.
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
#include "mdf.h"

/* USER CODE BEGIN 0 */
#include "lpdma.h"
/* USER CODE END 0 */

MDF_HandleTypeDef AdfHandle0;
MDF_FilterConfigTypeDef AdfFilterConfig0;

/* ADF1 init function */
void MX_ADF1_Init(void)
{

  /* USER CODE BEGIN ADF1_Init 0 */

  /* USER CODE END ADF1_Init 0 */

  /* USER CODE BEGIN ADF1_Init 1 */

  /* USER CODE END ADF1_Init 1 */

  /**
    AdfHandle0 structure initialization and HAL_MDF_Init function call
  */
  AdfHandle0.Instance = ADF1_Filter0;
  AdfHandle0.Init.CommonParam.ProcClockDivider = 1;
  AdfHandle0.Init.CommonParam.OutputClock.Activation = ENABLE;
  AdfHandle0.Init.CommonParam.OutputClock.Pins = MDF_OUTPUT_CLOCK_0;
  AdfHandle0.Init.CommonParam.OutputClock.Divider = 2;
  AdfHandle0.Init.CommonParam.OutputClock.Trigger.Activation = DISABLE;
  AdfHandle0.Init.SerialInterface.Activation = ENABLE;
  AdfHandle0.Init.SerialInterface.Mode = MDF_SITF_NORMAL_SPI_MODE;
  AdfHandle0.Init.SerialInterface.ClockSource = MDF_SITF_CCK0_SOURCE;
  AdfHandle0.Init.SerialInterface.Threshold = 31;
  AdfHandle0.Init.FilterBistream = MDF_BITSTREAM0_FALLING;
  if (HAL_MDF_Init(&AdfHandle0) != HAL_OK)
  {
    Error_Handler();
  }

  /**
    AdfFilterConfig0 structure initialization

    WARNING : only structure is filled, no specific init function call for filter
  */
  AdfFilterConfig0.DataSource = MDF_DATA_SOURCE_BSMX;
  AdfFilterConfig0.Delay = 0;
  AdfFilterConfig0.CicMode = MDF_ONE_FILTER_SINC5;
  AdfFilterConfig0.DecimationRatio = 24;
  AdfFilterConfig0.Gain = 8;
  AdfFilterConfig0.ReshapeFilter.Activation = ENABLE;
  AdfFilterConfig0.ReshapeFilter.DecimationRatio = MDF_RSF_DECIMATION_RATIO_4;
  AdfFilterConfig0.HighPassFilter.Activation = ENABLE;
  AdfFilterConfig0.HighPassFilter.CutOffFrequency = MDF_HPF_CUTOFF_0_000625FPCM;
  AdfFilterConfig0.SoundActivity.Activation = DISABLE;
  AdfFilterConfig0.AcquisitionMode = MDF_MODE_ASYNC_CONT;
  AdfFilterConfig0.FifoThreshold = MDF_FIFO_THRESHOLD_NOT_EMPTY;
  AdfFilterConfig0.DiscardSamples = 0;
  /* USER CODE BEGIN ADF1_Init 2 */

  /* USER CODE END ADF1_Init 2 */

}

void HAL_MDF_MspInit(MDF_HandleTypeDef* mdfHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(IS_ADF_INSTANCE(mdfHandle->Instance))
  {
  /* USER CODE BEGIN ADF1_MspInit 0 */
  __HAL_RCC_ADF1_CLKAM_ENABLE();
  __HAL_RCC_LPDMA1_CLKAM_ENABLE();
  __HAL_RCC_LPGPIO1_CLKAM_ENABLE();
  __HAL_RCC_SRAM4_CLKAM_ENABLE();
   __HAL_RCC_MSIKSTOP_ENABLE();
  HAL_PWREx_EnableVddIO2();
  HAL_PWREx_EnableVddA();
  /* USER CODE END ADF1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADF1;
    PeriphClkInit.Adf1ClockSelection = RCC_ADF1CLKSOURCE_MSIK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* ADF1 clock enable */
    __HAL_RCC_ADF1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**ADF1 GPIO Configuration
    PB3 (JTDO/TRACESWO)     ------> ADF1_CCK0
    PB4 (NJTRST)     ------> ADF1_SDI0
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF3_ADF1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* ADF1 interrupt Init */
    HAL_NVIC_SetPriority(ADF1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADF1_IRQn);
  /* USER CODE BEGIN ADF1_MspInit 1 */

  /* USER CODE END ADF1_MspInit 1 */
  }
}

void HAL_MDF_MspDeInit(MDF_HandleTypeDef* mdfHandle)
{

  if(IS_ADF_INSTANCE(mdfHandle->Instance))
  {
  /* USER CODE BEGIN ADF1_MspDeInit 0 */

  /* USER CODE END ADF1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADF1_CLK_DISABLE();

    /**ADF1 GPIO Configuration
    PB3 (JTDO/TRACESWO)     ------> ADF1_CCK0
    PB4 (NJTRST)     ------> ADF1_SDI0
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4);

    /* ADF1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(ADF1_IRQn);
  /* USER CODE BEGIN ADF1_MspDeInit 1 */

  /* USER CODE END ADF1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

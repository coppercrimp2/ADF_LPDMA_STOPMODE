My attempt to get the STM32U595RIT6 working in Stop Mode, with ADF1 and LPDMA running and waking the micro up with Full and Half Full interrupts. The ADF peripheral stores data in a SRAM4 buffer, and the buffer is written to a SD card through the SDMMC interface. This code works if Sleep Mode is used instead of Stop Mode. But so far I cant get it to work in Stop Mode

UPDATE 6/20/2025: I finally got this code to work in STOP2 mode between SD Card writes. Successfully records at 16kHz. I followed the example that is found here:
C:\Users\rak287\STM32Cube\Repository\STM32Cube_FW_U5_V1.7.0\Projects\B-U585I-IOT02A\Examples\MDF
One of the keys to getting the code to work was to add the following code to HAL_MDF_MspInit() found in mdf.c

  /* USER CODE BEGIN ADF1_MspInit 0 */
  __HAL_RCC_ADF1_CLKAM_ENABLE();
  __HAL_RCC_LPDMA1_CLKAM_ENABLE();
  __HAL_RCC_LPGPIO1_CLKAM_ENABLE();
  __HAL_RCC_SRAM4_CLKAM_ENABLE();
   __HAL_RCC_MSIKSTOP_ENABLE();
  HAL_PWREx_EnableVddIO2();
  HAL_PWREx_EnableVddA();
  /* USER CODE END ADF1_MspInit 0 */
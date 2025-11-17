/**
  ******************************************************************************
  * @file    ltdc_app.c
  * @brief   LTDC Application Code - 2 Layer Display
  * @note    This file contains the LCD configuration and display functions
  *          for STM32H745I-DISCO board with 2-Layer support
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include "L8_320x240.h"      // Layer 1: Indexed color image
#include "RGB565_320x240.h"  // Layer 2: RGB565 image

/* External variables --------------------------------------------------------*/
extern LTDC_HandleTypeDef hltdc;

/* Reload flag for vertical blanking sync */
__IO uint32_t ReloadFlag = 0;

/* Private defines -----------------------------------------------------------*/
/* LCD RK043FN48H Timing defines */
#define RK043FN48H_HSYNC            ((uint16_t)41)   /* Horizontal synchronization */
#define RK043FN48H_HBP              ((uint16_t)13)   /* Horizontal back porch      */
#define RK043FN48H_HFP              ((uint16_t)32)   /* Horizontal front porch     */
#define RK043FN48H_VSYNC            ((uint16_t)10)   /* Vertical synchronization   */
#define RK043FN48H_VBP              ((uint16_t)2)    /* Vertical back porch        */
#define RK043FN48H_VFP              ((uint16_t)2)    /* Vertical front porch       */
#define RK043FN48H_WIDTH            ((uint16_t)480)  /* LCD width                  */
#define RK043FN48H_HEIGHT           ((uint16_t)272)  /* LCD height                 */

/* Private function prototypes -----------------------------------------------*/
static void LCD_Config_2Layers(void);

/* Public functions ----------------------------------------------------------*/

/**
  * @brief  Initialize LCD with 2 layers
  * @note   Call this function after LTDC peripheral initialization
  * @retval None
  */
void LTDC_App_Init(void)
{
  /* MPU and Cache are configured in main() before HAL_Init() */
  /* LCD_DISP (PD7) is configured and set HIGH in MX_GPIO_Init() */

  /* Small delay to ensure LCD is ready */
  HAL_Delay(10);

  /* Configure 2 layers */
  LCD_Config_2Layers();

  /* Load CLUT for Layer 1 (indexed color) */
  HAL_LTDC_ConfigCLUT(&hltdc, (uint32_t *)L8_320x240_CLUT, 256, 0);

  /* Enable CLUT for Layer 1 */
  HAL_LTDC_EnableCLUT(&hltdc, 0);

  printf("LTDC 2-Layer initialized\r\n");
}

/**
  * @brief  Configure LTDC with 2 layers
  * @note   Layer 1 (Background): L8 indexed color at (0,0)
  *         Layer 2 (Foreground): RGB565 at (160,32)
  * @retval None
  */
static void LCD_Config_2Layers(void)
{
  LTDC_LayerCfgTypeDef pLayerCfg1;
  LTDC_LayerCfgTypeDef pLayerCfg2;

  /*##-1- Layer 1 Configuration (Background - L8 Indexed Color) ##############*/

  /* Windowing configuration */
  pLayerCfg1.WindowX0 = 0;
  pLayerCfg1.WindowX1 = 320;
  pLayerCfg1.WindowY0 = 0;
  pLayerCfg1.WindowY1 = 240;

  /* Pixel Format: L8 (8-bit indexed color) */
  pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_L8;

  /* Frame buffer address in Flash */
  pLayerCfg1.FBStartAdress = (uint32_t)&L8_320x240;

  /* Alpha: fully opaque */
  pLayerCfg1.Alpha = 255;
  pLayerCfg1.Alpha0 = 0;

  /* Default background color */
  pLayerCfg1.Backcolor.Blue = 0;
  pLayerCfg1.Backcolor.Green = 0;
  pLayerCfg1.Backcolor.Red = 0;

  /* Blending factors */
  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;

  /* Image dimensions */
  pLayerCfg1.ImageWidth = 320;
  pLayerCfg1.ImageHeight = 240;

  /*##-2- Layer 2 Configuration (Foreground - RGB565) ########################*/

  /* Windowing configuration */
  pLayerCfg2.WindowX0 = 160;  // Offset to the right
  pLayerCfg2.WindowX1 = 480;
  pLayerCfg2.WindowY0 = 32;   // Offset downward
  pLayerCfg2.WindowY1 = 272;

  /* Pixel Format: RGB565 */
  pLayerCfg2.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;

  /* Frame buffer address in Flash */
  pLayerCfg2.FBStartAdress = (uint32_t)&RGB565_320x240;

  /* Alpha: 78% opaque for blending effect */
  pLayerCfg2.Alpha = 200;
  pLayerCfg2.Alpha0 = 0;

  /* Default background color */
  pLayerCfg2.Backcolor.Blue = 0;
  pLayerCfg2.Backcolor.Green = 0;
  pLayerCfg2.Backcolor.Red = 0;

  /* Blending factors */
  pLayerCfg2.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg2.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;

  /* Image dimensions */
  pLayerCfg2.ImageWidth = 320;
  pLayerCfg2.ImageHeight = 240;

  /*##-3- Configure Layers ####################################################*/

  /* Configure Layer 1 (Background) */
  if(HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg1, 0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure Layer 2 (Foreground) */
  if(HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg2, 1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  LTDC Reload Event Callback
  * @note   Called when LTDC reload completes (vertical blanking)
  * @param  hltdc: LTDC handle
  * @retval None
  */
void HAL_LTDC_ReloadEventCallback(LTDC_HandleTypeDef *hltdc)
{
  ReloadFlag = 1;
}

/**
  * @brief  Update Layer 1 position (for animation)
  * @param  x0: X start position
  * @param  y0: Y start position
  * @retval None
  */
void LTDC_SetLayer1Position(uint32_t x0, uint32_t y0)
{
  /* Set new position without immediate reload */
  HAL_LTDC_SetWindowPosition_NoReload(&hltdc, x0, y0, 0);
}

/**
  * @brief  Update Layer 2 position (for animation)
  * @param  x0: X start position
  * @param  y0: Y start position
  * @retval None
  */
void LTDC_SetLayer2Position(uint32_t x0, uint32_t y0)
{
  /* Set new position without immediate reload */
  HAL_LTDC_SetWindowPosition_NoReload(&hltdc, x0, y0, 1);
}

/**
  * @brief  Trigger LTDC reload and wait for completion
  * @note   Use this after SetLayerPosition_NoReload functions
  * @retval None
  */
void LTDC_ReloadAndWait(void)
{
  /* Trigger reload on vertical blanking */
  ReloadFlag = 0;
  HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_VERTICAL_BLANKING);

  /* Wait for reload to complete */
  while(ReloadFlag == 0)
  {
    /* Wait */
  }
}

/**
  * @brief  Set Layer 1 alpha (transparency)
  * @param  alpha: Alpha value (0-255, 0=transparent, 255=opaque)
  * @retval None
  */
void LTDC_SetLayer1Alpha(uint8_t alpha)
{
  HAL_LTDC_SetAlpha(&hltdc, alpha, 0);
}

/**
  * @brief  Set Layer 2 alpha (transparency)
  * @param  alpha: Alpha value (0-255, 0=transparent, 255=opaque)
  * @retval None
  */
void LTDC_SetLayer2Alpha(uint8_t alpha)
{
  HAL_LTDC_SetAlpha(&hltdc, alpha, 1);
}

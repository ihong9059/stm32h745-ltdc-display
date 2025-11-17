/**
  ******************************************************************************
  * @file    ltdc_app.c
  * @brief   LTDC Application Code - Display RGB565 Image
  * @note    This file contains the LCD configuration and display functions
  *          for STM32H745I-DISCO board
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "RGB565_480x272.h"

/* External variables --------------------------------------------------------*/
extern LTDC_HandleTypeDef hltdc;

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
static void LCD_Config(void);

/* Public functions ----------------------------------------------------------*/

/**
  * @brief  Initialize LCD and display image
  * @note   Call this function after LTDC peripheral initialization
  * @retval None
  */
void LTDC_App_Init(void)
{
  /* MPU and Cache are configured in main() before HAL_Init() */
  /* LCD_DISP (PD7) is configured and set HIGH in MX_GPIO_Init() */

  /* Small delay to ensure LCD is ready */
  HAL_Delay(10);

  /* Configure LCD Layer with image from Flash */
  LCD_Config();
}

/**
  * @brief  LCD Layer Configuration
  * @note   Configure LTDC Layer 1 to display RGB565 image from Flash memory
  * @retval None
  */
static void LCD_Config(void)
{
  LTDC_LayerCfgTypeDef pLayerCfg;

  /* Layer 1 Configuration */

  /* Windowing configuration */
  /* In this case all the active display area is used to display a picture */
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = RK043FN48H_WIDTH;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = RK043FN48H_HEIGHT;

  /* Pixel Format configuration: RGB565 */
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;

  /* Start Address configuration: frame buffer is located at Flash memory */
  pLayerCfg.FBStartAdress = (uint32_t)&RGB565_480x272;

  /* Alpha constant (255 == totally opaque) */
  pLayerCfg.Alpha = 255;

  /* Default Color configuration (configure A,R,G,B component values) */
  pLayerCfg.Alpha0 = 0; /* fully transparent */
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;

  /* Configure blending factors */
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;

  /* Configure the number of lines and number of pixels per line */
  pLayerCfg.ImageWidth = RK043FN48H_WIDTH;
  pLayerCfg.ImageHeight = RK043FN48H_HEIGHT;

  /* Configure the Layer */
  if(HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 1) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }
}


/**
  * @brief  Update display with new image data
  * @param  imageData: Pointer to image data (RGB565 format)
  * @param  x0: Start X position
  * @param  y0: Start Y position
  * @param  width: Image width
  * @param  height: Image height
  * @retval None
  */
void LTDC_UpdateDisplay(const uint16_t *imageData, uint16_t x0, uint16_t y0,
                         uint16_t width, uint16_t height)
{
  LTDC_LayerCfgTypeDef pLayerCfg;

  /* Configure new window parameters */
  pLayerCfg.WindowX0 = x0;
  pLayerCfg.WindowX1 = x0 + width;
  pLayerCfg.WindowY0 = y0;
  pLayerCfg.WindowY1 = y0 + height;

  /* Pixel Format configuration */
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;

  /* Frame buffer address */
  pLayerCfg.FBStartAdress = (uint32_t)imageData;

  /* Alpha and blending */
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;

  /* Image size */
  pLayerCfg.ImageWidth = width;
  pLayerCfg.ImageHeight = height;

  /* Reconfigure Layer */
  if(HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 1) != HAL_OK)
  {
    Error_Handler();
  }
}

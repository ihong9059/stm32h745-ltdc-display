/**
  ******************************************************************************
  * @file    ltdc_app.h
  * @brief   LTDC Application Header File
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LTDC_APP_H
#define __LTDC_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Exported functions prototypes ---------------------------------------------*/
void LTDC_App_Init(void);
void LTDC_UpdateDisplay(const uint16_t *imageData, uint16_t x0, uint16_t y0,
                         uint16_t width, uint16_t height);

#ifdef __cplusplus
}
#endif

#endif /* __LTDC_APP_H */

/**
  ******************************************************************************
  * @file    ltdc_app.h
  * @brief   LTDC Application Header File - 2 Layer Display
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
void LTDC_SetLayer1Position(uint32_t x0, uint32_t y0);
void LTDC_SetLayer2Position(uint32_t x0, uint32_t y0);
void LTDC_ReloadAndWait(void);
void LTDC_SetLayer1Alpha(uint8_t alpha);
void LTDC_SetLayer2Alpha(uint8_t alpha);

#ifdef __cplusplus
}
#endif

#endif /* __LTDC_APP_H */

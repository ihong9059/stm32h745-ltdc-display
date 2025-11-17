# LTDC 프로젝트 설정 가이드

## 현재 프로젝트 상태
- ✓ 프로젝트 생성 완료
- ✓ CM4/CM7 듀얼 코어 구조 생성
- ✓ 기본 GPIO 설정 완료
- ⚠️ LTDC 주변장치 설정 필요
- ⚠️ 클럭 설정 필요

## STM32CubeIDE에서 LTDC 활성화 단계

### 1단계: .ioc 파일 열기
1. STM32CubeIDE에서 프로젝트를 엽니다
2. Project Explorer에서 `ltdc.ioc` 파일을 더블클릭합니다
3. CubeMX 편집기가 열립니다

### 2단계: LTDC 주변장치 활성화

#### 2.1 LTDC 모드 설정
1. 좌측 패널에서 **Multimedia → LTDC** 클릭
2. Mode 탭에서:
   - **Display**: `Display1` 체크 ✓
   - **Layers**:
     - Layer 0: 체크 해제 ✗
     - **Layer 1**: 체크 ✓

#### 2.2 LTDC 파라미터 설정

**Configuration → Parameter Settings:**

**Display Timing (RK043FN48H LCD):**
```
Horizontal Synchronization Width: 40
Vertical Synchronization Height: 9
Accumulated Horizontal Back Porch: 53
Accumulated Vertical Back Porch: 11
Accumulated Active Width: 533
Accumulated Active Height: 283
Total Width: 565
Total Height: 285
```

**극성 설정:**
```
HSYNC Polarity: Active Low
VSYNC Polarity: Active Low
DE Polarity: Active Low
PC Polarity: IPC (Input Pixel Clock)
```

**배경색:**
```
Red: 0
Green: 0
Blue: 0
```

**Layer 1 설정:**
```
Window Horizontal Start: 0
Window Horizontal Stop: 479
Window Vertical Start: 0
Window Vertical Stop: 271
Pixel Format: RGB565
Alpha: 255
Alpha0: 0
Blending Factor 1: Constant Alpha
Blending Factor 2: Constant Alpha
Image Width: 480
Image Height: 272
```

### 3단계: 클럭 설정

1. **Clock Configuration** 탭 클릭

2. **PLL1 (System Clock):**
   ```
   Source: HSE
   /M: 5
   ×N: 160
   /P: 2 (SYSCLK = 400MHz)
   ```

3. **PLL3 (LTDC Clock):**
   ```
   Source: HSE
   /M: 5
   ×N: 160
   /R: 83 (LTDC Clock ≈ 9.63MHz)
   ```

4. **Bus Clocks:**
   ```
   AHB Prescaler: 2 (HCLK = 200MHz)
   APB1/2/3/4 Prescaler: 2 (100MHz each)
   ```

5. **Peripheral Clock:**
   - LTDC Clock Mux: `PLL3_R` 선택

### 4단계: GPIO 확인

**Pinout view에서 다음 핀들이 설정되었는지 확인:**

- **GPIOD:**
  - PD7: GPIO_Output (LCD_DISP - LCD Enable)

- **GPIOI, J, K, H:** LTDC 신호 핀들 (AF14)
  - 보드 초기화 시 자동 설정되었을 것입니다

**수동으로 PD7 설정 (필요시):**
1. Pinout view에서 PD7 클릭
2. `GPIO_Output` 선택
3. System Core → GPIO → PD7:
   - GPIO mode: Output Push Pull
   - GPIO Pull-up/Pull-down: No pull-up and no pull-down
   - Maximum output speed: Low
   - User Label: `LCD_DISP`

### 5단계: 코드 생성

1. 메뉴: **Project → Generate Code**
2. 또는 단축키: **Option+K** (macOS) / **Alt+K** (Windows)
3. 코드 생성 완료 대기
4. Project Explorer에서 **F5** (Refresh)

### 6단계: 생성된 코드 확인

**CM7/Core/Src/main.c에 다음 함수들이 생성되었는지 확인:**
- `MX_LTDC_Init()`
- `extern LTDC_HandleTypeDef hltdc;`

**CM7/Core/Src/ltdc.c에 초기화 코드 확인 (또는 main.c에 통합)**

### 7단계: 애플리케이션 코드 통합

#### 7.1 main.c 수정

**Includes 추가:**
```c
/* USER CODE BEGIN Includes */
#include "ltdc_app.h"
#include "RGB565_480x272.h"
/* USER CODE END Includes */
```

**변수 선언 (필요시):**
```c
/* USER CODE BEGIN PV */
extern LTDC_HandleTypeDef hltdc;
/* USER CODE END PV */
```

**main() 함수에서 LTDC 초기화:**
```c
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_LTDC_Init();  /* ← 이 함수가 있어야 합니다 */

  /* USER CODE BEGIN 2 */

  /* Initialize LCD and display image */
  LTDC_App_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  while (1)
  {
    /* USER CODE BEGIN 3 */
    /* USER CODE END 3 */
  }
}
```

#### 7.2 프로젝트에 파일 추가

**추가된 파일:**
- `CM7/Core/Inc/RGB565_480x272.h` - 이미지 데이터
- `CM7/Core/Inc/ltdc_app.h` - LTDC 애플리케이션 헤더
- `CM7/Core/Src/ltdc_app.c` - LTDC 애플리케이션 코드

### 8단계: 빌드 및 테스트

1. **프로젝트 빌드:**
   - Project 우클릭 → Build Project
   - 또는 **Cmd+B** (macOS)

2. **오류 해결:**
   - `hltdc` 선언 오류: MX_LTDC_Init() 생성 확인
   - Include 오류: 파일 경로 확인

3. **보드에 다운로드:**
   - Run → Debug Configurations
   - CM7 Debug 구성 선택
   - Debug 버튼 클릭

4. **결과 확인:**
   - LCD에 RGB565 이미지가 표시되어야 합니다
   - LED2가 켜지지 않아야 합니다 (오류 없음)

## 문제 해결

### LTDC 초기화 함수가 생성되지 않음
- .ioc 파일에서 LTDC가 활성화되었는지 확인
- Project → Generate Code 재실행
- Project Explorer에서 F5 (Refresh)

### 화면에 아무것도 표시되지 않음
- PD7 (LCD_DISP) 핀이 HIGH로 설정되었는지 확인
- LTDC 클럭 (PLL3) 설정 확인
- 타이밍 파라미터 값 재확인

### 빌드 오류
- RGB565_480x272.h 파일이 CM7/Core/Inc/에 있는지 확인
- ltdc_app.c/h 파일 추가 확인
- 프로젝트 Clean 후 재빌드

## 참고 자료

- 원본 프로젝트: `LTDC_Display_1Layer/`
- 상세 가이드: `LTDC_Project_Migration_Guide.html`
- 전체 개발 가이드: `STM32H745I-DISCO_개발가이드.html`

## 다음 단계

1. ✅ LTDC 활성화 및 설정
2. ✅ 코드 생성
3. ✅ 애플리케이션 코드 통합
4. ✅ 빌드 및 테스트
5. 🔜 터치 스크린 기능 추가
6. 🔜 그래픽 라이브러리 통합 (TouchGFX)
7. 🔜 애니메이션 및 UI 개발

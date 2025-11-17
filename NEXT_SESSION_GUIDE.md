# 다음 세션 시작 가이드

## 🎯 다음 세션 시작 시 Claude에게 전달할 메시지

```
이전 세션에서 STM32H745I-DISCO LTDC 프로젝트를 진행했습니다.
다음 파일들을 읽고 현재 상태를 파악해주세요:

1. NEXT_SESSION_GUIDE.md - 이 파일 (프로젝트 현황)
2. SESSION_REPORT.md - 전체 작업 내용 및 현재 상태
3. LTDC_2Layer_Migration_Guide.html - 2-Layer 마이그레이션 가이드
4. /Users/maeg/todo/stm32/ltdc/CM7/Core/Src/main.c - 메인 코드

현재 상태: 2-Layer LTDC 디스플레이가 정상 작동하며, 레이어 애니메이션과 1초 카운터가 시리얼로 출력됩니다.

[여기에 다음 작업할 내용 입력]
```

---

## 📂 프로젝트 구조

```
/Users/maeg/todo/stm32/ltdc/
├── NEXT_SESSION_GUIDE.md              ⭐ 이 파일 (다음 세션 가이드)
├── SESSION_REPORT.md                  ⭐ 이전 세션 전체 요약
├── LTDC_2Layer_Migration_Guide.html   ⭐ 1-Layer → 2-Layer 마이그레이션 가이드
├── test_instructions.md               - 초기 진단 가이드
├── ltdc.ioc                           - STM32CubeMX 설정 파일
├── CM7/                               - Cortex-M7 코드
│   ├── Core/
│   │   ├── Src/
│   │   │   ├── main.c                 ⭐ 메인 프로그램 (애니메이션 + 1초 카운터)
│   │   │   ├── ltdc.c                 ⭐ LTDC 초기화
│   │   │   ├── ltdc_app.c             ⭐ 2-Layer 설정 및 제어 함수
│   │   │   ├── gpio.c                 ⭐ GPIO 설정 (PD7 LCD 백라이트)
│   │   │   ├── usart.c                ⭐ UART3 시리얼 통신
│   │   │   └── stm32h7xx_it.c         ⭐ LTDC 인터럽트 핸들러
│   │   └── Inc/
│   │       ├── L8_320x240.h           ⭐ Layer 1 이미지 (L8 indexed + CLUT)
│   │       ├── RGB565_320x240.h       ⭐ Layer 2 이미지 (RGB565)
│   │       └── ltdc_app.h             - Layer 제어 함수 헤더
│   └── Debug/                         - 빌드 출력
├── CM4/                               - Cortex-M4 코드
└── LTDC_Display_2Layers/              - 참조 프로젝트
```

---

## ✅ 현재 시스템 상태

### 작동 중인 기능
- ✅ LTDC 2-Layer 디스플레이 (L8 + RGB565)
- ✅ Layer 1: 320×240 L8 indexed color with CLUT (256 colors)
- ✅ Layer 2: 320×240 RGB565 with alpha blending (78% opacity)
- ✅ 레이어 애니메이션 (vertical blanking 동기화)
- ✅ RK043FN48H LCD 패널 정상 작동 (480×272)
- ✅ LED 3개 (LD6, LD7, LD8) 애니메이션 사이클마다 토글
- ✅ UART3 시리얼 출력 (115200 baud)
- ✅ 1초마다 증가하는 카운터 출력
- ✅ 듀얼 코어 동기화 (CM7 + CM4)

### 핵심 설정

**LTDC 구성:**
- **해상도:** 480×272 (RK043FN48H LCD)
- **Layer 1 (Background):**
  - 위치: (0, 0) ~ (320, 240)
  - 픽셀 포맷: L8 (8-bit indexed color)
  - CLUT: 256 RGB888 entries
  - Alpha: 255 (fully opaque)
  - 프레임버퍼: Flash (L8_320x240)

- **Layer 2 (Foreground):**
  - 초기 위치: (160, 32) ~ (480, 272)
  - 픽셀 포맷: RGB565 (16-bit)
  - Alpha: 200 (78% opaque)
  - 프레임버퍼: Flash (RGB565_320x240)
  - 블렌딩: PAxCA

**하드웨어:**
- **LCD Enable:** PD7 (GPIO Output, HIGH)
- **LTDC Clock:** 9.63 MHz (PLL3)
- **LTDC Interrupt:** Enabled (LTDC_IRQn)
- **VCP Serial:** USART3 (PB10/PB11, 115200 baud)

### LED 핀 매핑
- **LD6:** PI13 (애니메이션 사이클마다 토글)
- **LD7:** PJ2 (애니메이션 사이클마다 토글)
- **LD8:** PD3 (애니메이션 사이클마다 토글)

### 애니메이션 동작
1. Animation 1: 두 레이어가 서로를 향해 이동 (32 steps)
2. 500ms 지연
3. Animation 2: 원래 위치로 복귀 (32 steps)
4. 500ms 지연
5. LED 토글 및 반복

### 시리얼 출력
```
=================================
STM32H745I-DISCO LTDC Display
LTDC_Display_2Layers Example
CM7 Core
=================================

DEBUG: Before LTDC_App_Init
LTDC 2-Layer initialized
DEBUG: After LTDC_App_Init
LCD initialized successfully!
2 Layers displayed with alpha blending

Counter: 1 sec
Counter: 2 sec
Counter: 3 sec
...
```

---

## 🔧 주요 코드 구조

### main.c:243-305
```c
/* Layer animation variables */
uint32_t counter = 0;
uint32_t last_tick = HAL_GetTick();

while (1) {
  // Animation 1: Move layers towards each other
  for (index = 0; index < 32; index++) {
    LTDC_SetLayer1Position(index * 5, index);
    LTDC_SetLayer2Position(160 - index * 5, 32 - index);
    LTDC_ReloadAndWait();
  }

  HAL_Delay(500);

  // Animation 2: Return to original positions
  for (index = 0; index < 32; index++) {
    LTDC_SetLayer2Position(index * 5, index);
    LTDC_SetLayer1Position(160 - index * 5, 32 - index);
    LTDC_ReloadAndWait();
  }

  HAL_Delay(500);

  // Toggle LEDs
  HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_13);  /* LD6 */
  HAL_GPIO_TogglePin(GPIOJ, GPIO_PIN_2);   /* LD7 */
  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_3);   /* LD8 */

  // Print counter every 1 second
  if (HAL_GetTick() - last_tick >= 1000) {
    counter++;
    printf("Counter: %lu sec\r\n", counter);
    last_tick = HAL_GetTick();
  }
}
```

### ltdc_app.c 주요 함수
- `LTDC_App_Init()` - 2-Layer 초기화 및 CLUT 로드
- `LCD_Config_2Layers()` - Layer 1, 2 설정
- `LTDC_SetLayer1Position()` - Layer 1 위치 업데이트
- `LTDC_SetLayer2Position()` - Layer 2 위치 업데이트
- `LTDC_ReloadAndWait()` - Vertical blanking 동기화
- `LTDC_SetLayer1Alpha()` - Layer 1 투명도 조절
- `LTDC_SetLayer2Alpha()` - Layer 2 투명도 조절
- `HAL_LTDC_ReloadEventCallback()` - LTDC 인터럽트 콜백

---

## 🚀 다음 세션 가능 작업

### Option 1: DMA2D 활용 (추천도: ⭐⭐⭐⭐⭐)

**목표:** 하드웨어 가속 그래픽 처리

**작업 내용:**
1. DMA2D 초기화 (STM32CubeMX)
2. RAM 프레임버퍼로 픽셀 복사
3. 색상 변환 테스트 (L8 → RGB565 → ARGB8888)
4. 기본 도형 그리기 (사각형, 선, 원)
5. Alpha blending 효과

**예상 시간:** 2-3시간

**장점:**
- CPU 부하 없이 고속 그래픽 처리
- 픽셀 복사 성능 10배 이상 향상
- 실시간 애니메이션 가능

---

### Option 2: RAM 프레임버퍼 + 실시간 그래픽 (추천도: ⭐⭐⭐⭐⭐)

**목표:** 동적 컨텐츠 표시

**작업 내용:**
1. SDRAM 초기화 (16MB)
2. 프레임버퍼를 SDRAM으로 이동
3. DMA2D로 도형/텍스트 그리기
4. 더블 버퍼링 구현
5. 실시간 센서 데이터 그래프 표시

**예상 시간:** 3-4시간

**주의사항:**
- Cache 관리 필수 (SCB_CleanDCache)
- MPU 설정 조정 (SDRAM 영역)

---

### Option 3: 터치스크린 통합 (추천도: ⭐⭐⭐⭐)

**목표:** 터치 입력 감지 및 UI 인터랙션

**작업 내용:**
1. I2C 터치 컨트롤러 (FT5336) 초기화
2. 터치 좌표 읽기 및 보정
3. 버튼/슬라이더 UI 구현
4. 터치 이벤트로 레이어 제어

**예상 시간:** 3-4시간

**필요 리소스:**
- FT5336 BSP 드라이버
- I2C4 설정 (CubeMX)

---

### Option 4: TouchGFX 통합 (추천도: ⭐⭐⭐⭐)

**목표:** 전문적인 GUI 구현

**작업 내용:**
1. TouchGFX Designer 설치
2. 기존 프로젝트 변환
3. 버튼/게이지/차트 디자인
4. 애니메이션 효과 추가
5. 터치 이벤트 핸들링

**예상 시간:** 4-6시간

**참고:**
- TouchGFX는 무료이며 ST 공식 지원
- 드래그 앤 드롭 GUI 디자이너
- C++ 코드 자동 생성

---

### Option 5: Layer Alpha 애니메이션 (추천도: ⭐⭐⭐)

**목표:** 페이드 인/아웃 효과

**작업 내용:**
1. Layer 2 Alpha를 0~255로 부드럽게 변경
2. Fade In/Out 함수 구현
3. 크로스페이드 효과 (여러 이미지 전환)
4. 타이밍 최적화

**예상 시간:** 1-2시간

**예제 코드:**
```c
/* Fade Out Layer 2 */
for (uint8_t alpha = 200; alpha > 0; alpha -= 5) {
  LTDC_SetLayer2Alpha(alpha);
  HAL_Delay(20);
}
```

---

### Option 6: 시리얼 명령 인터페이스 (추천도: ⭐⭐⭐)

**목표:** UART로 디스플레이 제어

**작업 내용:**
1. UART 수신 인터럽트 설정
2. 간단한 명령 파서 구현
3. 명령어로 레이어 위치/투명도 제어
4. 상태 정보 출력

**예상 시간:** 2-3시간

**명령어 예시:**
```
> layer1 pos 100 50     # Layer 1을 (100, 50)으로 이동
> layer2 alpha 128      # Layer 2 투명도 50%
> status                # 현재 상태 출력
```

---

### Option 7: 센서 데이터 시각화 (추천도: ⭐⭐⭐⭐)

**목표:** 실시간 그래프 표시

**작업 내용:**
1. ADC로 센서 데이터 수집
2. 실시간 라인 차트 그리기 (DMA2D)
3. 스크롤링 그래프 구현
4. 최대/최소/평균 값 표시

**예상 시간:** 3-4시간

---

### Option 8: QR 코드 / 바코드 표시 (추천도: ⭐⭐)

**목표:** 정보를 QR 코드로 표시

**작업 내용:**
1. QR 코드 생성 라이브러리 통합
2. QR 이미지를 프레임버퍼에 렌더링
3. URL/텍스트 인코딩
4. 동적 QR 코드 업데이트

**예상 시간:** 2-3시간

---

## 🔧 빠른 시작 명령어

### 빌드
```bash
cd /Users/maeg/todo/stm32/ltdc/CM7/Debug
export PATH="/Applications/STM32CubeIDE.app/Contents/Eclipse/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.macos64_1.0.0.202411102158/tools/bin:/usr/bin:/bin:/usr/sbin:/sbin"
rm -f Core/Src/main.o && make all
```

### 플래시 (CM7 + CM4)
```bash
export PROG="/Applications/STM32CubeIDE.app/Contents/Eclipse/plugins/com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.macos64_2.2.200.202503041107/tools/bin/STM32_Programmer_CLI"

# CM7 플래시
"$PROG" -c port=SWD -w /Users/maeg/todo/stm32/ltdc/CM7/Debug/ltdc_CM7.elf -v -rst

# CM4 플래시
"$PROG" -c port=SWD -w /Users/maeg/todo/stm32/ltdc/CM4/Debug/ltdc_CM4.elf -v -rst
```

### 시리얼 모니터
```bash
# screen 사용
screen /dev/cu.usbmodem142103 115200
# 종료: Ctrl+A → K → y

# 또는 Python으로 읽기
python3 -c "
import serial, time, sys
ser = serial.Serial('/dev/cu.usbmodem142103', 115200, timeout=0.5)
while True:
    if ser.in_waiting:
        print(ser.read(ser.in_waiting).decode('utf-8', errors='ignore'), end='')
        sys.stdout.flush()
    time.sleep(0.1)
"
```

---

## 📋 문제 해결 참고

### LCD 화면 문제
- **흰색 화면:** PD7 (LCD_DISP) 핀 확인 → gpio.c:54-62
- **깜빡임:** Reload 타이밍 조정 → LTDC_ReloadAndWait()
- **색상 이상:** 픽셀 포맷 확인 (L8/RGB565)
- **Layer 미표시:** CLUT 로드 확인 → HAL_LTDC_ConfigCLUT()

### LTDC 타이밍 문제
- **CubeMX 재생성 시 타이밍 오류:** ltdc.c:48-56의 타이밍 값 확인
  ```c
  hltdc.Init.HorizontalSync = 40;
  hltdc.Init.VerticalSync = 9;
  hltdc.Init.AccumulatedHBP = 53;
  hltdc.Init.AccumulatedVBP = 11;
  hltdc.Init.AccumulatedActiveW = 533;
  hltdc.Init.AccumulatedActiveH = 283;
  hltdc.Init.TotalWidth = 565;
  hltdc.Init.TotalHeigh = 285;
  ```

### 애니메이션 문제
- **버벅임:** HAL_Delay() 제거, LTDC_ReloadAndWait()만 사용
- **동기화 실패:** LTDC 인터럽트 확인 → stm32h7xx_it.c:206-209

### 시리얼 출력 문제
- **출력 없음:** UART3 초기화 확인 → main.c:197
- **깨진 문자:** 보드레이트 확인 (115200)
- **printf 동작 안함:** __io_putchar() 구현 확인 → main.c:84-92

### 빌드 문제
- **PATH 오류:** export PATH 명령 실행
- **링커 오류:** 메모리 부족, FLASH/RAM 크기 확인
- **Missing 헤더:** Include path 확인

### 플래시 문제
- **연결 실패:** ST-LINK 케이블 확인
- **Permission denied:** PROG 경로 확인
- **Erase 실패:** 보드 전원 재시작

---

## 💡 개발 팁

### 1. 이미지 변환

**RGB565 변환:**
```python
from PIL import Image
import struct

def convert_to_rgb565(input_path, output_path, width, height):
    img = Image.open(input_path)
    img = img.resize((width, height))
    img = img.convert('RGB')

    with open(output_path, 'w') as f:
        f.write(f'const uint16_t RGB565_{width}x{height}[{width*height}] = {{\n')
        pixels = img.load()
        for y in range(height):
            for x in range(width):
                r, g, b = pixels[x, y]
                rgb565 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)
                f.write(f'0x{rgb565:04X}, ')
            f.write('\n')
        f.write('};\n')

convert_to_rgb565('input.png', 'output.h', 320, 240)
```

**L8 + CLUT 변환:**
```python
def convert_to_l8_clut(input_path, output_path, width, height):
    img = Image.open(input_path)
    img = img.resize((width, height))
    img = img.convert('P', palette=Image.ADAPTIVE, colors=256)

    palette = img.getpalette()
    clut = []
    for i in range(0, 768, 3):
        r, g, b = palette[i:i+3]
        argb8888 = 0xFF000000 | (r << 16) | (g << 8) | b
        clut.append(argb8888)

    with open(output_path, 'w') as f:
        # Image data
        f.write(f'const uint8_t L8_{width}x{height}[{width*height}] = {{\n')
        pixels = list(img.getdata())
        for i, p in enumerate(pixels):
            f.write(f'0x{p:02X}, ')
            if (i+1) % 16 == 0:
                f.write('\n')
        f.write('};\n\n')

        # CLUT
        f.write(f'const uint32_t L8_{width}x{height}_CLUT[256] = {{\n')
        for i, c in enumerate(clut):
            f.write(f'0x{c:08X}, ')
            if (i+1) % 4 == 0:
                f.write('\n')
        f.write('};\n')
```

### 2. 메모리 최적화
```c
/* Flash에 이미지 저장 (읽기 전용) */
const uint16_t image[] __attribute__((section(".rodata"))) = {...};

/* SDRAM에 프레임버퍼 할당 */
uint16_t framebuffer[480*272] __attribute__((section(".sdram")));

/* 4-byte alignment 필수 */
__attribute__((aligned(4))) uint8_t buffer[...];
```

### 3. 성능 측정
```c
/* DWT Cycle Counter 사용 (더 정확) */
CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
DWT->CYCCNT = 0;

uint32_t start = DWT->CYCCNT;
// ... 작업 ...
uint32_t cycles = DWT->CYCCNT - start;
printf("Cycles: %lu (%.2f us)\n", cycles, cycles / 400.0f);
```

### 4. Cache 관리
```c
/* RAM 프레임버퍼 업데이트 후 반드시 호출 */
SCB_CleanDCache_by_Addr((uint32_t*)framebuffer, width*height*2);

/* MPU 설정으로 Cache 비활성화 (SDRAM 영역) */
MPU_Region_InitTypeDef MPU_InitStruct;
MPU_InitStruct.Enable = MPU_REGION_ENABLE;
MPU_InitStruct.BaseAddress = 0xD0000000;
MPU_InitStruct.Size = MPU_REGION_SIZE_16MB;
MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
```

---

## ⚠️ 주의사항

### 1. .ioc 파일 재생성 시

STM32CubeMX로 .ioc 파일을 재생성하면 다음 수정사항이 사라집니다:

**gpio.c (PD7 설정):**
```c
/* Configure PD7 (LCD_DISP - LCD Backlight Enable) */
GPIO_InitStruct.Pin = GPIO_PIN_7;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
```

**ltdc.c (타이밍 + 인터럽트):**
```c
/* RK043FN48H LCD Timing (lines 48-56) */
/* Layer 설정 제거 (lines 65-69) */
/* LTDC 인터럽트 활성화 (lines 174-175) */
HAL_NVIC_SetPriority(LTDC_IRQn, 0, 0);
HAL_NVIC_EnableIRQ(LTDC_IRQn);
```

**stm32h7xx_it.c (인터럽트 핸들러):**
```c
void LTDC_IRQHandler(void)
{
  HAL_LTDC_IRQHandler(&hltdc);
}
```

**main.c:**
- MPU_Config() 호출
- CPU_CACHE_Enable() 호출
- LTDC_App_Init() 호출
- LED/카운터 코드

→ **해결책:** USER CODE BEGIN/END 섹션 안에 코드 작성

### 2. 듀얼 코어 동기화
CM7과 CM4가 동일한 리소스에 접근하지 않도록 주의:
- HSEM (Hardware Semaphore) 사용
- LTDC는 CM7에서만 제어
- UART는 한쪽 코어에서만 사용

### 3. 메모리 정렬
- 프레임버퍼는 반드시 4-byte aligned
- DMA2D 사용 시 주소 정렬 확인
- CLUT 테이블도 4-byte aligned

---

## 🎯 추천 진행 순서

**초급 → 고급:**

1. ✅ **2-Layer 구현** (완료!)
2. **Layer Alpha 애니메이션** (간단, 1-2시간)
3. **DMA2D 도형 그리기** (중급, 2-3시간)
4. **RAM 프레임버퍼** (중급, 3-4시간)
5. **터치스크린** (중급, 3-4시간)
6. **TouchGFX** (고급, 4-6시간)

---

## 📞 세션 시작 템플릿

```
안녕하세요! 이전 세션에서 STM32H745I-DISCO LTDC 2-Layer 프로젝트를 완료했습니다.

다음 파일들을 읽어주세요:
1. /Users/maeg/todo/stm32/ltdc/NEXT_SESSION_GUIDE.md
2. /Users/maeg/todo/stm32/ltdc/SESSION_REPORT.md

현재 상태:
- 2-Layer LTDC 디스플레이 정상 작동 ✅
- Layer 애니메이션 동작 ✅
- 시리얼 1초 카운터 출력 ✅

오늘 작업: [여기에 원하는 작업 입력]
예: "DMA2D를 사용해서 실시간으로 도형을 그려주세요"
```

---

**작성일:** 2025년 11월 17일
**프로젝트:** STM32H745I-DISCO LTDC 2-Layer Display
**현재 버전:** 2-Layer Animation + Serial Counter
**다음 세션:** 준비 완료! 🚀

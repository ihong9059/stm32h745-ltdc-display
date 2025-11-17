# 다음 세션 시작 가이드

## 🎯 다음 세션 시작 시 Claude에게 전달할 메시지

```
이전 세션에서 STM32H745I-DISCO LTDC 프로젝트를 진행했습니다.
다음 파일들을 읽고 현재 상태를 파악해주세요:

1. SESSION_REPORT.md - 전체 작업 내용 및 현재 상태
2. LTDC_Layer_Guide.html - LTDC Layer 구성 가이드
3. /Users/maeg/todo/stm32/ltdc/CM7/Core/Src/main.c - 메인 코드

현재 상태: RGB565 이미지가 LCD에 정상 표시되고, LED 3개가 1초마다 깜빡입니다.

[여기에 다음 작업할 내용 입력]
```

---

## 📂 프로젝트 구조

```
/Users/maeg/todo/stm32/ltdc/
├── SESSION_REPORT.md           ⭐ 이전 세션 전체 요약
├── NEXT_SESSION_GUIDE.md       ⭐ 이 파일 (다음 세션 가이드)
├── LTDC_Layer_Guide.html       ⭐ 1-Layer vs 2-Layer 가이드
├── test_instructions.md        - 초기 진단 가이드
├── ltdc.ioc                    - STM32CubeMX 설정 파일
├── CM7/                        - Cortex-M7 코드
│   ├── Core/
│   │   ├── Src/
│   │   │   ├── main.c          ⭐ 메인 프로그램
│   │   │   ├── ltdc.c          ⭐ LTDC 초기화
│   │   │   ├── ltdc_app.c      ⭐ LTDC Layer 설정
│   │   │   └── gpio.c          ⭐ GPIO 설정 (PD7)
│   │   └── Inc/
│   │       ├── RGB565_480x272.h ⭐ 이미지 데이터
│   │       └── ltdc_app.h
│   └── Debug/                  - 빌드 출력
└── CM4/                        - Cortex-M4 코드
```

---

## ✅ 현재 시스템 상태

### 작동 중인 기능
- ✅ LTDC를 통한 480×272 RGB565 이미지 표시
- ✅ RK043FN48H LCD 패널 정상 작동
- ✅ LED 3개 (LD6, LD7, LD8) 1초 간격 깜빡임
- ✅ UART3 시리얼 출력 (115200 baud)
- ✅ 듀얼 코어 동기화 (CM7 + CM4)

### 핵심 설정
- **해상도:** 480×272
- **픽셀 포맷:** RGB565 (16-bit)
- **프레임버퍼:** Flash 메모리 (읽기 전용)
- **LTDC 구성:** 1-Layer (Layer 1만 사용)
- **LCD Enable:** PD7 (GPIO Output, HIGH)
- **LTDC Clock:** 9.63 MHz (PLL3)

### LED 핀 매핑
- **LD6:** PI13 (초기화 완료 표시, 깜빡임)
- **LD7:** PJ2 (초기화 완료 표시, 깜빡임)
- **LD8:** PD3 (초기화 완료 표시, 깜빡임)

---

## 🚀 다음 세션 가능 작업

### Option 1: 2-Layer 구현 (추천도: ⭐⭐⭐⭐⭐)

**목표:** Layer 2를 추가하여 UI 오버레이 구현

**작업 내용:**
1. Layer 2 설정 추가 (반투명 UI 요소)
2. Alpha Blending 테스트
3. 동적으로 Layer 2 On/Off

**예상 시간:** 2-3시간

**참고 파일:**
- `LTDC_Layer_Guide.html` - 2-Layer 구현 예제
- `LTDC_Display_2Layers` 참조 프로젝트

**예제 코드:**
```c
/* Layer 2: UI Overlay */
pLayerCfg2.WindowX0 = 100;
pLayerCfg2.WindowX1 = 380;
pLayerCfg2.WindowY0 = 50;
pLayerCfg2.WindowY1 = 222;
pLayerCfg2.PixelFormat = LTDC_PIXEL_FORMAT_ARGB4444;
pLayerCfg2.FBStartAdress = (uint32_t)&UIOverlay;
pLayerCfg2.Alpha = 180;  // 70% opaque
HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg2, 2);
```

---

### Option 2: 터치스크린 통합 (추천도: ⭐⭐⭐⭐)

**목표:** 터치 입력 감지 및 UI 인터랙션

**작업 내용:**
1. I2C 터치 컨트롤러 (FT5336) 초기화
2. 터치 좌표 읽기
3. 터치 이벤트 처리
4. 간단한 버튼 UI 구현

**예상 시간:** 3-4시간

**필요 리소스:**
- FT5336 드라이버 (BSP)
- I2C 통신 설정

---

### Option 3: DMA2D 활용 (추천도: ⭐⭐⭐⭐⭐)

**목표:** 하드웨어 가속 그래픽 처리

**작업 내용:**
1. DMA2D 초기화
2. 픽셀 복사 테스트
3. 색상 변환 (RGB565 ↔ ARGB8888)
4. 사각형/선 그리기 함수 구현

**예상 시간:** 2-3시간

**장점:**
- CPU 부하 감소
- 빠른 그래픽 업데이트
- 애니메이션 성능 향상

---

### Option 4: RAM 프레임버퍼 (추천도: ⭐⭐⭐)

**목표:** 동적 컨텐츠 표시

**작업 내용:**
1. SDRAM 초기화 (16MB)
2. 프레임버퍼를 SDRAM으로 이동
3. 실시간 그래픽 업데이트
4. 더블 버퍼링 구현

**예상 시간:** 3-4시간

**주의사항:**
- Cache 관리 필요 (SCB_CleanDCache)
- MPU 설정 조정

---

### Option 5: GUI 라이브러리 통합 (추천도: ⭐⭐⭐⭐)

**목표:** TouchGFX 또는 STemWin 적용

**작업 내용:**
1. TouchGFX Designer 설치
2. 간단한 UI 디자인
3. 프로젝트 통합
4. 이벤트 핸들링

**예상 시간:** 4-6시간

**참고:**
- TouchGFX 권장 (무료, 현대적 UI)
- STemWin (상업용 라이선스 필요)

---

### Option 6: 애니메이션 구현 (추천도: ⭐⭐⭐)

**목표:** 이미지 슬라이드쇼 또는 페이드 효과

**작업 내용:**
1. 여러 이미지 준비
2. 타이머를 사용한 자동 전환
3. Fade In/Out 효과 (Alpha 조절)
4. 슬라이드 애니메이션

**예상 시간:** 2-3시간

---

### Option 7: 센서 데이터 표시 (추천도: ⭐⭐⭐⭐)

**목표:** 실시간 센서 데이터 시각화

**작업 내용:**
1. 센서 연결 (온도, 가속도계 등)
2. 데이터 수집
3. 그래프/차트 표시
4. 업데이트 주기 최적화

**예상 시간:** 3-4시간

---

### Option 8: 비디오 재생 (추천도: ⭐⭐)

**목표:** MJPEG 또는 간단한 비디오 재생

**작업 내용:**
1. JPEG 디코더 활용
2. SD 카드에서 프레임 로드
3. 순차 재생
4. FPS 제어

**예상 시간:** 4-5시간

**주의사항:**
- JPEG Codec 하드웨어 가속 필요
- 높은 SD 카드 읽기 속도 필요

---

## 🔧 빠른 시작 명령어

### 빌드
```bash
cd /Users/maeg/todo/stm32/ltdc/CM7/Debug
export PATH="/Applications/STM32CubeIDE.app/Contents/Eclipse/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.macos64_1.0.0.202411102158/tools/bin:/usr/bin:/bin:/usr/sbin:/sbin:$PATH"
make clean && make all
```

### 플래시
```bash
export PROG="/Applications/STM32CubeIDE.app/Contents/Eclipse/plugins/com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.macos64_2.2.200.202503041107/tools/bin/STM32_Programmer_CLI"
"$PROG" -c port=SWD -w /Users/maeg/todo/stm32/ltdc/CM7/Debug/ltdc_CM7.elf -rst
```

### 시리얼 모니터
```bash
screen /dev/cu.usbmodem142103 115200
# 또는
python3 simple_serial.py
```

---

## 📋 문제 해결 참고

### LCD 화면 문제
- **흰색 화면:** PD7 (LCD_DISP) 핀 확인
- **깜빡임:** Reload 타이밍 조정
- **색상 이상:** 픽셀 포맷 확인

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
RGB565 이미지 생성 방법:
```bash
# ImageMagick 사용
convert input.png -resize 480x272! -depth 16 -define bmp:subtype=RGB565 output.bmp

# Python PIL 사용
from PIL import Image
img = Image.open('input.png')
img = img.resize((480, 272))
# RGB565 변환 코드 필요
```

### 2. 메모리 최적화
- Flash에 이미지 저장: `const` 키워드 사용
- SDRAM 활용: 동적 컨텐츠용
- DMA2D로 복사 최적화

### 3. 성능 측정
```c
/* SysTick을 사용한 시간 측정 */
uint32_t start = HAL_GetTick();
// ... 작업 ...
uint32_t elapsed = HAL_GetTick() - start;
printf("Elapsed: %lu ms\n", elapsed);
```

### 4. Cache 관리
```c
/* RAM 프레임버퍼 사용 시 */
SCB_CleanDCache_by_Addr((uint32_t*)framebuffer, size);
```

---

## 📚 유용한 리소스

### 공식 문서
- [RM0399: STM32H745 Reference Manual](https://www.st.com/resource/en/reference_manual/rm0399-stm32h745755-and-stm32h747757-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [AN4861: LTDC on STM32 MCUs](https://www.st.com/resource/en/application_note/an4861-lcdtft-display-controller-ltdc-on-stm32-mcus-stmicroelectronics.pdf)
- [UM2488: STM32H745I-DISCO User Manual](https://www.st.com/resource/en/user_manual/um2488-discovery-kit-with-stm32h745xi-and-stm32h750xb-mcus-stmicroelectronics.pdf)

### STM32 예제
- `STM32Cube_FW_H7_V1.x.x/Projects/STM32H745I-DISCO/Examples/LTDC/`
  - LTDC_Display_1Layer
  - LTDC_Display_2Layers
  - LTDC_AnimatedPicture

### TouchGFX
- [TouchGFX Designer 다운로드](https://www.st.com/en/development-tools/touchgfxdesigner.html)
- [TouchGFX Documentation](https://support.touchgfx.com/)

---

## ⚠️ 주의사항

### 1. .ioc 파일 재생성 시
STM32CubeMX로 .ioc 파일을 재생성하면 다음 수정사항이 사라집니다:

**gpio.c:**
- PD7 GPIO Output 설정

**main.c:**
- MPU_Config() 호출
- CPU_CACHE_Enable() 호출
- LED 디버깅 코드
- LED 깜빡임 코드

**ltdc_app.c:**
- LCD_Config() 활성화

→ **해결책:** USER CODE BEGIN/END 섹션 안에 코드 작성

### 2. 듀얼 코어 동기화
CM7과 CM4가 동시에 동일한 리소스에 접근하지 않도록 주의:
- HSEM (Hardware Semaphore) 사용
- LTDC는 CM7에서만 제어

### 3. 메모리 정렬
프레임버퍼는 반드시 4-byte aligned:
```c
__attribute__((aligned(4))) uint8_t framebuffer[...];
```

---

## 🎯 추천 진행 순서

**초급 → 고급 순서:**

1. **2-Layer 구현** (LTDC 심화)
2. **DMA2D 활용** (그래픽 가속)
3. **터치스크린** (사용자 입력)
4. **RAM 프레임버퍼** (동적 컨텐츠)
5. **GUI 라이브러리** (고급 UI)

---

## 📞 세션 시작 템플릿

```
안녕하세요! 이전 세션에서 STM32H745I-DISCO LTDC 프로젝트를 진행했습니다.

다음 파일들을 읽어주세요:
1. /Users/maeg/todo/stm32/ltdc/SESSION_REPORT.md
2. /Users/maeg/todo/stm32/ltdc/LTDC_Layer_Guide.html

현재 상태:
- RGB565 이미지 LCD 표시 ✅
- LED 3개 깜빡임 ✅
- 시리얼 출력 정상 ✅

오늘 작업: [여기에 원하는 작업 입력]
예: "2-Layer 구현해서 반투명 UI 오버레이 추가해주세요"
```

---

**작성일:** 2025년 11월 17일
**프로젝트:** STM32H745I-DISCO LTDC Display
**다음 세션:** 준비 완료! 🚀

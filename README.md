# STM32H745I-DISCO LTDC Display Project

STM32H745I-DISCO 보드에서 LTDC를 사용하여 RGB565 이미지를 LCD에 표시하는 프로젝트입니다.

## 📌 프로젝트 정보

- **보드:** STM32H745I-DISCO
- **LCD:** RK043FN48H 4.3" TFT (480×272)
- **픽셀 포맷:** RGB565 (16-bit color)
- **LTDC 구성:** 1-Layer
- **상태:** Production Ready ✅

## ✅ 구현된 기능

- LTDC를 통한 480×272 RGB565 이미지 표시
- RK043FN48H LCD 패널 정상 구동
- UART3 시리얼 출력 (115200 baud)
- LED 3개 동시 깜빡임 (1초 간격)
- 듀얼 코어 동기화 (CM7 + CM4)

## 📂 프로젝트 구조

```
ltdc/
├── README.md                   # 이 파일
├── SESSION_REPORT.md           # 상세 작업 보고서
├── NEXT_SESSION_GUIDE.md       # 다음 세션 가이드
├── LTDC_Layer_Guide.html       # LTDC Layer 가이드
├── CM7/                        # Cortex-M7 코드
│   ├── Core/
│   │   ├── Src/
│   │   │   ├── main.c          # 메인 프로그램
│   │   │   ├── ltdc.c          # LTDC 초기화
│   │   │   ├── ltdc_app.c      # LTDC Layer 설정
│   │   │   └── gpio.c          # GPIO 설정
│   │   └── Inc/
│   │       └── RGB565_480x272.h # 이미지 데이터
│   └── Debug/                  # 빌드 출력
└── CM4/                        # Cortex-M4 코드
```

## 🚀 빌드 및 플래시

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

## 📚 문서

- **SESSION_REPORT.md** - 전체 작업 내용 및 문제 해결 과정
- **NEXT_SESSION_GUIDE.md** - 다음 세션 시작 가이드
- **LTDC_Layer_Guide.html** - 1-Layer vs 2-Layer 비교 가이드

## 🔧 핵심 설정

### LTDC 타이밍
- **해상도:** 480×272
- **HSYNC:** 41, **VSYNC:** 10
- **HBP:** 13, **VBP:** 2
- **HFP:** 32, **VFP:** 2
- **LTDC Clock:** 9.63 MHz (PLL3)

### 핀 설정
- **LCD_DISP (Enable):** PD7 (GPIO Output, HIGH)
- **LD6 (LED):** PI13
- **LD7 (LED):** PJ2
- **LD8 (LED):** PD3

## 🎯 다음 작업 아이디어

1. **2-Layer 구현** - UI 오버레이, Alpha Blending
2. **터치스크린 통합** - FT5336 터치 컨트롤러
3. **DMA2D 활용** - 하드웨어 가속 그래픽
4. **RAM 프레임버퍼** - 동적 컨텐츠 표시
5. **GUI 라이브러리** - TouchGFX 통합

## 📝 라이선스

STM32 HAL Library 기반 프로젝트

## 👤 작성자

- GitHub: ihong9059
- 날짜: 2025년 11월 17일

# LTDC 프로젝트 빌드 가이드

## 빌드 방법

### 방법 1: STM32CubeIDE에서 빌드 (권장)

#### 단계별 절차:

1. **STM32CubeIDE 실행**
   ```
   /Applications/STM32CubeIDE.app 더블클릭
   ```

2. **워크스페이스 선택**
   - Workspace: `/Users/maeg/todo/stm32` 선택
   - Launch 클릭

3. **프로젝트 임포트 (처음 한 번만)**
   - File → Import
   - General → Existing Projects into Workspace
   - Select root directory: `/Users/maeg/todo/stm32/ltdc` 찾아보기
   - "ltdc" 프로젝트 체크
   - Finish 클릭

4. **빌드**
   - **CM7 빌드:**
     - Project Explorer에서 `ltdc_CM7` 우클릭
     - Build Project 선택 (또는 Cmd+B)

   - **CM4 빌드:**
     - Project Explorer에서 `ltdc_CM4` 우클릭
     - Build Project 선택

5. **빌드 결과 확인**
   - Console 창에서 빌드 로그 확인
   - 성공 시:
     ```
     Finished building target: ltdc_CM7.elf

     arm-none-eabi-size  ltdc_CM7.elf
        text    data     bss     dec     hex filename
       12345    1234    5678   19257    4b39 ltdc_CM7.elf

     Finished building: default.size.stdout
     Build Finished. 0 errors, 0 warnings.
     ```

### 방법 2: 명령줄에서 빌드

#### 주의사항:
- STM32CubeIDE가 **실행 중이 아니어야** 합니다 (워크스페이스 잠금 방지)
- 터미널에서 실행

#### 빌드 스크립트 사용:

```bash
cd /Users/maeg/todo/stm32/ltdc
chmod +x build.sh
./build.sh
```

#### 수동 빌드:

```bash
# GCC 도구체인을 PATH에 추가
export PATH="/Applications/STM32CubeIDE.app/Contents/Eclipse/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.macos64_1.0.0.202411102158/tools/bin:$PATH"

# CM7 빌드
cd /Users/maeg/todo/stm32/ltdc/CM7/Debug
make clean
make -j4

# CM4 빌드
cd /Users/maeg/todo/stm32/ltdc/CM4/Debug
make clean
make -j4
```

## 빌드 출력 파일

빌드 성공 시 다음 파일들이 생성됩니다:

```
ltdc/
├── CM7/Debug/
│   ├── ltdc_CM7.elf     ← 실행 파일
│   ├── ltdc_CM7.bin     ← 바이너리 파일
│   ├── ltdc_CM7.hex     ← HEX 파일
│   ├── ltdc_CM7.list    ← 어셈블리 리스트
│   └── ltdc_CM7.map     ← 메모리 맵
└── CM4/Debug/
    ├── ltdc_CM4.elf
    ├── ltdc_CM4.bin
    ├── ltdc_CM4.hex
    ├── ltdc_CM4.list
    └── ltdc_CM4.map
```

## 빌드 오류 해결

### 오류 1: "No such file or directory"

**원인:** 소스 파일이 누락되었거나 경로가 잘못됨

**해결:**
1. Project Explorer에서 F5 (Refresh)
2. 파일이 실제로 존재하는지 확인
3. .ioc 파일에서 "Project → Generate Code" 재실행

### 오류 2: "undefined reference to..."

**원인:** 링크 오류, 라이브러리 누락

**해결:**
1. 프로젝트 우클릭 → Clean Project
2. 프로젝트 다시 빌드
3. 필요한 경우 .c 파일이 빌드에 포함되어 있는지 확인

### 오류 3: "arm-none-eabi-gcc: command not found"

**원인:** ARM GCC 컴파일러가 PATH에 없음

**해결 (명령줄 빌드 시):**
```bash
export PATH="/Applications/STM32CubeIDE.app/Contents/Eclipse/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.macos64_1.0.0.202411102158/tools/bin:$PATH"
```

**해결 (STM32CubeIDE):**
- STM32CubeIDE에서 빌드하면 자동으로 PATH 설정됨

### 오류 4: printf 관련 오류

**증상:**
```
undefined reference to `_write'
undefined reference to `_sbrk'
```

**해결:**
1. syscalls.c와 sysmem.c 파일이 프로젝트에 포함되어 있는지 확인
2. 이미 포함되어 있어야 함 (Core/Src/)

### 오류 5: 빌드는 성공하지만 실행 안 됨

**확인 사항:**
1. 두 코어 모두 빌드되었는지 확인 (CM7, CM4)
2. 디버그 구성이 올바른지 확인
3. 보드가 올바르게 연결되었는지 확인

## 메모리 사용량 확인

빌드 후 메모리 사용량 확인:

```bash
cd CM7/Debug
arm-none-eabi-size ltdc_CM7.elf
```

출력 예시:
```
   text    data     bss     dec     hex filename
  15234    1024    2048   18306    477e ltdc_CM7.elf
```

- **text**: 코드 영역 (Flash 메모리)
- **data**: 초기화된 데이터 (Flash + RAM)
- **bss**: 초기화되지 않은 데이터 (RAM)
- **dec/hex**: 전체 크기

## 빌드 최적화

### Debug 빌드 (기본)
- 최적화: -O0 (최적화 없음)
- 디버그 정보 포함
- 크기: 크고 느림
- 용도: 개발 및 디버깅

### Release 빌드

1. Build Configuration 변경:
   - 프로젝트 우클릭 → Build Configurations → Set Active → Release

2. 최적화 수준:
   - 프로젝트 속성 → C/C++ Build → Settings
   - MCU GCC Compiler → Optimization
   - Optimization level: -O2 (균형) 또는 -Os (크기 최소화)

## 빌드 시간 단축

1. **병렬 빌드 활성화:**
   - Window → Preferences
   - General → Workspace → Build
   - "Use parallel build" 체크
   - 코어 수: 4 (또는 CPU 코어 수)

2. **불필요한 인덱싱 비활성화:**
   - Window → Preferences
   - C/C++ → Indexer
   - "Enable indexer" 체크 해제 (선택사항)

3. **캐시 정리:**
   ```bash
   # 빌드 캐시 삭제
   rm -rf CM7/Debug/*
   rm -rf CM4/Debug/*
   ```

## 클린 빌드

모든 오브젝트 파일을 삭제하고 처음부터 다시 빌드:

**STM32CubeIDE:**
- 프로젝트 우클릭 → Clean Project
- 또는 Project → Clean → 프로젝트 선택 → Clean

**명령줄:**
```bash
cd CM7/Debug && make clean
cd CM4/Debug && make clean
```

## 다음 단계

빌드가 성공했다면:

1. ✅ **프로그램 다운로드**
   - Run → Debug Configurations
   - CM7 Debug 선택
   - Debug 버튼 클릭

2. ✅ **시리얼 출력 확인**
   - 터미널에서: `screen /dev/tty.usbmodemXXXX 115200`
   - 카운터 출력 확인

3. ✅ **LCD 기능 추가**
   - .ioc 파일에서 LTDC 설정
   - ltdc_app.c 코드 활용

## 참고 문서

- **빌드 가이드**: BUILD_GUIDE.md (이 파일)
- **시리얼 테스트**: SERIAL_TEST_GUIDE.md
- **LTDC 설정**: README_CONFIGURATION.md
- **전체 가이드**: LTDC_Project_Migration_Guide.html

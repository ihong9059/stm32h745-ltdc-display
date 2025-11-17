# STM32H745I-DISCO Flash Programming 가이드

## Flash 프로그래밍 방법

### 방법 1: STM32CubeIDE에서 Flash (가장 쉬움) ⭐

#### Run 모드 (디버깅 없이 실행):

1. **보드 연결**
   - STM32H745I-DISCO를 USB 케이블로 PC에 연결

2. **Run Configuration 생성 (처음 한 번만)**
   - Run → Run Configurations
   - STM32 Cortex-M C/C++ Application 더블클릭
   - Name: `ltdc_CM7 Run`
   - C/C++ Application: `CM7/Debug/ltdc_CM7.elf`
   - 찾아보기 버튼으로 파일 선택
   - Apply 클릭

3. **Flash 및 실행**
   - Run 버튼 클릭 (또는 Ctrl+F11 / Cmd+F11)
   - 프로그램이 자동으로 flash되고 실행됩니다

4. **Serial 출력 확인**
   ```bash
   # 터미널에서
   screen /dev/tty.usbmodemXXXX 115200
   ```

#### Debug 모드:

1. **Debug Configuration 사용**
   - Run → Debug Configurations
   - STM32 Cortex-M C/C++ Application → ltdc_CM7 Debug
   - Debug 버튼 클릭 (F11)

2. **프로그램 실행**
   - Resume 버튼 (F8) 또는
   - Run → Resume

### 방법 2: 명령줄 Flash 스크립트 🚀

**가장 빠른 방법! 한 번에 CM7과 CM4를 모두 프로그래밍합니다.**

#### 사용 방법:

```bash
# ltdc 프로젝트 디렉토리로 이동
cd /Users/maeg/todo/stm32/ltdc

# Flash 스크립트 실행
./flash.sh
```

#### 예상 출력:

```
==================================
STM32H745I-DISCO Flash Programming
==================================

Checking binary files...
Found: CM7/Debug/ltdc_CM7.elf
Found: CM4/Debug/ltdc_CM4.elf

Checking ST-LINK connection...
ST-LINK connected successfully!

Programming CM7...
      -------------------------------------------------------------------
                       STM32CubeProgrammer v2.x.x
      -------------------------------------------------------------------

Memory Programming ...
Opening and parsing file: ltdc_CM7.elf
  File          : ltdc_CM7.elf
  Size          : 15234 Bytes
  Address       : 0x08000000

Erasing memory corresponding to segment 0:
Erasing memory corresponding to segment 1:
Download in Progress:

File download complete
Time elapsed during download operation: 00:00:01.234

RUNNING Program ...
  Address:      : 0x8000000
Application is running
CM7 programmed successfully!

Programming CM4...
[... similar output ...]
CM4 programmed successfully!

==================================
Programming Summary
==================================
CM7: SUCCESS
CM4: SUCCESS

All cores programmed successfully!

Next steps:
1. Open serial terminal: screen /dev/tty.usbmodemXXXX 115200
2. Press RESET button on the board (black button)
3. You should see the counter output
```

### 방법 3: STM32CubeProgrammer GUI

#### 설치:
https://www.st.com/stm32cubeprog 에서 다운로드 및 설치

#### 사용 방법:

1. **STM32CubeProgrammer 실행**
   ```
   /Applications/STMicroelectronics/STM32Cube/STM32CubeProgrammer/STM32CubeProgrammer.app
   ```

2. **ST-LINK 연결**
   - 오른쪽 상단: Connect 방식 → ST-LINK
   - Port: SWD
   - Frequency: 4000 kHz
   - Mode: Normal
   - **Connect** 버튼 클릭

3. **CM7 프로그래밍**
   - Erasing & Programming 탭
   - File path: CM7/Debug/ltdc_CM7.elf 선택
   - Start address: 0x08000000 (자동 설정됨)
   - ✓ Verify programming
   - ✓ Run after programming
   - **Start Programming** 클릭

4. **CM4 프로그래밍**
   - File path: CM4/Debug/ltdc_CM4.elf 선택
   - Start address: 0x08100000 (확인 필요)
   - **Start Programming** 클릭

5. **Disconnect**
   - 오른쪽 상단 Disconnect 버튼 클릭

### 방법 4: 수동 명령줄 (고급 사용자용)

```bash
# STM32_Programmer_CLI 경로
PROG="/Applications/STM32CubeIDE.app/Contents/Eclipse/plugins/com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.macos64_2.2.200.202503041107/tools/bin/STM32_Programmer_CLI"

# ST-LINK 연결 확인
"$PROG" -c port=SWD -q

# CM7 프로그래밍
"$PROG" -c port=SWD -w CM7/Debug/ltdc_CM7.elf -v -rst

# CM4 프로그래밍
"$PROG" -c port=SWD -w CM4/Debug/ltdc_CM4.elf -v -rst

# 연결 해제
"$PROG" -c port=SWD -q
```

## Flash 주소 맵

STM32H745는 듀얼 코어이므로 각 코어의 코드가 다른 주소에 저장됩니다:

| 코어 | Flash 시작 주소 | 크기 | 설명 |
|------|----------------|------|------|
| CM7  | 0x08000000     | ~1MB | 메인 애플리케이션 |
| CM4  | 0x08100000     | ~1MB | 서브 프로세서 |

## 문제 해결

### 오류 1: "Cannot connect to ST-LINK"

**원인:**
- 보드가 연결되지 않음
- ST-LINK 드라이버 문제
- 다른 프로그램이 ST-LINK 사용 중

**해결:**
1. USB 케이블 재연결
2. 보드의 전원 LED (빨간색) 확인
3. STM32CubeIDE 닫기
4. 다른 ST-LINK 프로그램 종료
5. macOS: System Preferences → Security & Privacy에서 권한 확인

### 오류 2: "Error: No device found"

**원인:**
- ST-LINK 펌웨어 버전 문제
- 보드가 BOOT0 모드에 있음

**해결:**
1. ST-LINK 펌웨어 업데이트:
   ```bash
   "$PROG" -c port=SWD --fwupgrade
   ```
2. BOOT0 점퍼 확인 (정상 동작 시 LOW)

### 오류 3: "Error: Flash memory is protected"

**원인:**
- Read/Write protection 활성화됨

**해결:**
```bash
# Read protection 해제
"$PROG" -c port=SWD -ob RDP=0xAA

# 주의: 이 명령은 Flash를 지웁니다!
```

### 오류 4: ".elf file not found"

**원인:**
- 프로젝트가 빌드되지 않음

**해결:**
1. STM32CubeIDE에서 프로젝트 빌드
2. 또는: `./build.sh` 실행
3. Debug 폴더에 .elf 파일 확인

### 오류 5: Flash 후 프로그램 실행 안 됨

**체크리스트:**
- [ ] CM7과 CM4 둘 다 프로그래밍 했는가?
- [ ] RESET 버튼을 눌렀는가?
- [ ] 전원이 연결되어 있는가?
- [ ] Serial 터미널 설정이 올바른가? (115200 baud)

**강제 리셋:**
```bash
# 프로그래밍 후 자동 리셋
"$PROG" -c port=SWD -rst
```

## 옵션 바이트 설정

듀얼 코어 부팅을 위한 옵션 바이트:

```bash
# CM4 코어 자동 부팅 비활성화 (CM7이 제어)
"$PROG" -c port=SWD -ob BCM4=0

# 또는 CM4 자동 부팅 활성화
"$PROG" -c port=SWD -ob BCM4=1
```

**주의:** 원본 예제는 CM7이 CM4를 수동으로 부팅하므로 BCM4=0 권장

## Flash 검증

프로그래밍 후 검증:

```bash
# Flash 내용 읽기 및 비교
"$PROG" -c port=SWD -r "read_cm7.bin" 0x08000000 0x10000

# ELF 파일과 비교
"$PROG" -c port=SWD -w CM7/Debug/ltdc_CM7.elf -v
```

## 대량 프로그래밍 (Multiple Boards)

여러 보드를 순차적으로 프로그래밍:

```bash
#!/bin/bash
for i in {1..10}; do
    echo "Programming board $i..."
    ./flash.sh
    echo "Press Enter after connecting next board..."
    read
done
```

## 성능 팁

### Flash 속도 향상:

1. **SWD 속도 증가:**
   ```bash
   "$PROG" -c port=SWD freq=24000 -w ltdc_CM7.elf
   ```

2. **Verify 비활성화 (빠르지만 위험):**
   ```bash
   "$PROG" -c port=SWD -w ltdc_CM7.elf --skipErase
   ```

3. **병렬 프로그래밍 (여러 ST-LINK):**
   ```bash
   # ST-LINK 시리얼 번호로 구분
   "$PROG" -c port=SWD sn=066BFF32534E8867 -w CM7.elf &
   "$PROG" -c port=SWD sn=066CFF32534E8868 -w CM4.elf &
   wait
   ```

## 백업 및 복원

### Flash 백업:

```bash
# 전체 Flash 백업 (2MB)
"$PROG" -c port=SWD -r "backup_full.bin" 0x08000000 0x200000

# CM7 영역만 백업
"$PROG" -c port=SWD -r "backup_cm7.bin" 0x08000000 0x100000

# CM4 영역만 백업
"$PROG" -c port=SWD -r "backup_cm4.bin" 0x08100000 0x100000
```

### Flash 복원:

```bash
# 백업 복원
"$PROG" -c port=SWD -w "backup_full.bin" 0x08000000 -v
```

## 자동화 스크립트 예제

### 빌드 후 자동 Flash:

```bash
#!/bin/bash
# build_and_flash.sh

echo "Building project..."
./build.sh

if [ $? -eq 0 ]; then
    echo "Build successful! Flashing..."
    ./flash.sh
else
    echo "Build failed!"
    exit 1
fi
```

사용:
```bash
chmod +x build_and_flash.sh
./build_and_flash.sh
```

## 다음 단계

Flash가 성공했다면:

1. ✅ **Serial 터미널 연결**
   ```bash
   ls /dev/tty.usb*
   screen /dev/tty.usbmodemXXXX 115200
   ```

2. ✅ **보드 리셋**
   - 검은색 RESET 버튼 누르기

3. ✅ **출력 확인**
   ```
   =================================
   STM32H745I-DISCO Serial Test
   LTDC Project - CM7 Core
   =================================
   Starting counter...

   Counter: 0
   Counter: 1
   Counter: 2
   ...
   ```

## 참고 문서

- **Flash 가이드**: FLASH_GUIDE.md (이 파일)
- **빌드 가이드**: BUILD_GUIDE.md
- **Serial 테스트**: SERIAL_TEST_GUIDE.md
- **STM32CubeProgrammer 매뉴얼**: https://www.st.com/resource/en/user_manual/um2237-stm32cubeprogrammer-software-description-stmicroelectronics.pdf

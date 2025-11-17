# Serial 출력 테스트 가이드

## 개요
STM32H745I-DISCO 보드에서 USART3를 통해 1초마다 증가하는 카운터를 출력하는 예제입니다.

## 수정된 파일
- `CM7/Core/Src/main.c` - printf 리디렉션 및 카운터 출력 추가

## 추가된 기능

### 1. printf 리디렉션
USART3를 통해 printf 함수를 사용할 수 있도록 `__io_putchar()` 함수를 구현했습니다.

### 2. 카운터 출력
1초마다 0부터 시작하는 카운터 값을 출력합니다.

## 빌드 및 실행 방법

### 1단계: 프로젝트 빌드
```bash
# STM32CubeIDE에서
1. 프로젝트 우클릭 → Build Project
2. 또는 Cmd+B (macOS) / Ctrl+B (Windows)
```

### 2단계: 보드 연결
1. STM32H745I-DISCO 보드를 USB 케이블로 PC에 연결
2. ST-LINK Virtual COM Port가 자동으로 인식됩니다

### 3단계: 시리얼 터미널 설정

#### macOS의 경우:

**옵션 1: screen 명령 사용**
```bash
# 포트 확인
ls /dev/tty.usb*

# 시리얼 연결 (보통 /dev/tty.usbmodem* 형태)
screen /dev/tty.usbmodemXXXX 115200

# 종료: Ctrl+A, 그 다음 K, 그 다음 Y
```

**옵션 2: CoolTerm 사용**
1. CoolTerm 다운로드: https://freeware.the-meiers.org/
2. 실행 후 Options 클릭
3. Port 선택 (usbmodem으로 시작하는 포트)
4. Baudrate: 115200
5. Data Bits: 8
6. Parity: None
7. Stop Bits: 1
8. Connect 클릭

**옵션 3: minicom 사용**
```bash
# minicom 설치
brew install minicom

# 실행
minicom -D /dev/tty.usbmodemXXXX -b 115200

# 종료: Ctrl+A, 그 다음 X
```

#### Windows의 경우:

**옵션 1: PuTTY**
1. PuTTY 다운로드 및 설치
2. Connection type: Serial
3. Serial line: COM3 (장치 관리자에서 확인)
4. Speed: 115200
5. Open 클릭

**옵션 2: Tera Term**
1. Tera Term 다운로드 및 설치
2. Setup → Serial port
3. Port: COM3 (장치 관리자에서 확인)
4. Baud rate: 115200
5. Data: 8bit
6. Parity: none
7. Stop bits: 1bit
8. OK 클릭

#### Linux의 경우:
```bash
# 포트 확인
ls /dev/ttyACM*

# minicom 사용
sudo minicom -D /dev/ttyACM0 -b 115200

# 또는 screen 사용
sudo screen /dev/ttyACM0 115200
```

### 4단계: 프로그램 다운로드 및 실행

**STM32CubeIDE에서:**
1. Run → Debug Configurations
2. CM7 Debug 구성 선택
3. Debug 버튼 클릭 (F11)
4. Resume 버튼 클릭 (F8) 또는 Run 사용

**또는 Run 모드로 실행:**
1. Run → Run Configurations
2. CM7 Run 구성 선택
3. Run 버튼 클릭 (Ctrl+F11 / Cmd+F11)

## 예상 출력

시리얼 터미널에서 다음과 같은 출력을 볼 수 있습니다:

```
=================================
STM32H745I-DISCO Serial Test
LTDC Project - CM7 Core
=================================
Starting counter...

Counter: 0
Counter: 1
Counter: 2
Counter: 3
Counter: 4
Counter: 5
...
```

## 문제 해결

### 1. 아무것도 출력되지 않는 경우

**원인 1: COM 포트 찾기 실패**
- macOS: `ls /dev/tty.usb*` 또는 `ls /dev/cu.usb*` 실행
- Windows: 장치 관리자 → 포트(COM & LPT) 확인
- Linux: `ls /dev/ttyACM*` 또는 `dmesg | grep tty` 실행

**원인 2: Baudrate 불일치**
- 터미널 설정이 115200 baud인지 확인
- USART3 설정 확인 (usart.c 파일)

**원인 3: 프로그램 실행 안됨**
- 디버거에서 Resume (F8) 버튼을 눌렀는지 확인
- 보드 리셋 버튼 (검은색 버튼) 눌러보기

### 2. 깨진 문자가 출력되는 경우
- Baudrate를 115200으로 재확인
- Data bits: 8, Parity: None, Stop bits: 1 확인
- 다른 터미널 프로그램으로 시도

### 3. 빌드 오류

**오류: undefined reference to `_write`**

해결책: 프로젝트 설정 확인
1. 프로젝트 우클릭 → Properties
2. C/C++ Build → Settings
3. MCU GCC Linker → Miscellaneous
4. Other flags에 `-specs=nosys.specs` 추가 (이미 있을 수 있음)

**또는 syscalls.c 파일 수정:**

syscalls.c 파일이 있다면 _write 함수가 구현되어 있는지 확인

### 4. Permission denied (macOS/Linux)
```bash
# 권한 부여
sudo chmod 666 /dev/tty.usbmodemXXXX

# 또는 sudo로 실행
sudo screen /dev/tty.usbmodemXXXX 115200
```

## USART3 핀 정보

STM32H745I-DISCO 보드의 USART3는 ST-LINK를 통해 Virtual COM Port로 연결됩니다.

- **TX (전송)**: PB10 (MCU → PC)
- **RX (수신)**: PB11 (PC → MCU)
- **Baudrate**: 115200 bps
- **데이터**: 8 bits
- **패리티**: None
- **정지비트**: 1 bit

## 코드 설명

### printf 리디렉션
```c
#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}
```

GCC 컴파일러에서는 `__io_putchar()` 함수를 구현하면 printf가 자동으로 리디렉션됩니다.

### 메인 루프
```c
while (1)
{
  printf("Counter: %lu\r\n", counter);
  counter++;
  HAL_Delay(1000);
}
```

- `%lu`: unsigned long 형식 지정자
- `\r\n`: Carriage Return + Line Feed (윈도우/리눅스 호환)
- `HAL_Delay(1000)`: 1000ms (1초) 지연

## 추가 기능 아이디어

### 1. LED 토글 추가
```c
while (1)
{
  HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_13); // LED2 토글
  printf("Counter: %lu\r\n", counter);
  counter++;
  HAL_Delay(1000);
}
```

### 2. 시간 정보 출력
```c
printf("Counter: %lu, Time: %lu ms\r\n", counter, HAL_GetTick());
```

### 3. UART 수신 추가
```c
uint8_t rxData;
if(HAL_UART_Receive(&huart3, &rxData, 1, 100) == HAL_OK)
{
  printf("Received: %c\r\n", rxData);
}
```

## 참고

- USART3는 CM7에서만 초기화되었습니다
- CM4에서도 serial 출력을 원하면 별도 UART를 설정해야 합니다
- printf는 시스템 리소스를 많이 사용하므로, 실제 제품에서는 경량 로깅 함수를 사용하는 것이 좋습니다

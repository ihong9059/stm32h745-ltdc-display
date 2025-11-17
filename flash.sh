#!/bin/bash

# STM32H745I-DISCO Flash Programming Script
# This script programs both CM7 and CM4 cores to the target board

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# STM32CubeProgrammer CLI path
PROGRAMMER="/Applications/STM32CubeIDE.app/Contents/Eclipse/plugins/com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.macos64_2.2.200.202503041107/tools/bin/STM32_Programmer_CLI"

# Alternative path if first one doesn't exist
if [ ! -f "$PROGRAMMER" ]; then
    PROGRAMMER="/Applications/STMicroelectronics/STM32Cube/STM32CubeProgrammer/STM32CubeProgrammer.app/Contents/MacOs/bin/STM32_Programmer_CLI"
fi

# Check if programmer exists
if [ ! -f "$PROGRAMMER" ]; then
    echo -e "${RED}Error: STM32_Programmer_CLI not found!${NC}"
    echo "Please install STM32CubeProgrammer"
    echo "Download from: https://www.st.com/stm32cubeprog"
    exit 1
fi

# ELF file paths
CM7_ELF="CM7/Debug/ltdc_CM7.elf"
CM4_ELF="CM4/Debug/ltdc_CM4.elf"

echo -e "${CYAN}==================================${NC}"
echo -e "${CYAN}STM32H745I-DISCO Flash Programming${NC}"
echo -e "${CYAN}==================================${NC}"
echo ""

# Function to check if file exists
check_file() {
    local file=$1
    if [ ! -f "$file" ]; then
        echo -e "${RED}Error: $file not found!${NC}"
        echo "Please build the project first using STM32CubeIDE or ./build.sh"
        return 1
    fi
    echo -e "${GREEN}Found: $file${NC}"
    return 0
}

# Check if ELF files exist
echo "Checking binary files..."
check_file "$CM7_ELF" || exit 1
check_file "$CM4_ELF" || exit 1
echo ""

# Function to program a core
program_core() {
    local CORE=$1
    local ELF_FILE=$2

    echo -e "${YELLOW}Programming $CORE...${NC}"

    # Program the flash
    "$PROGRAMMER" -c port=SWD -w "$ELF_FILE" -v -rst

    if [ $? -eq 0 ]; then
        echo -e "${GREEN}$CORE programmed successfully!${NC}"
        return 0
    else
        echo -e "${RED}Failed to program $CORE${NC}"
        return 1
    fi
}

# Check if ST-LINK is connected
echo "Checking ST-LINK connection..."
"$PROGRAMMER" -c port=SWD -q > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo -e "${RED}Error: Cannot connect to ST-LINK!${NC}"
    echo ""
    echo "Troubleshooting:"
    echo "1. Make sure STM32H745I-DISCO board is connected via USB"
    echo "2. Check if ST-LINK driver is installed"
    echo "3. Close any other applications using ST-LINK (STM32CubeIDE, STM32CubeProgrammer GUI)"
    echo "4. Try reconnecting the USB cable"
    exit 1
fi

echo -e "${GREEN}ST-LINK connected successfully!${NC}"
echo ""

# Program CM7 first
program_core "CM7" "$CM7_ELF"
CM7_STATUS=$?

echo ""

# Program CM4
program_core "CM4" "$CM4_ELF"
CM4_STATUS=$?

echo ""
echo -e "${CYAN}==================================${NC}"
echo -e "${CYAN}Programming Summary${NC}"
echo -e "${CYAN}==================================${NC}"

if [ $CM7_STATUS -eq 0 ]; then
    echo -e "CM7: ${GREEN}SUCCESS${NC}"
else
    echo -e "CM7: ${RED}FAILED${NC}"
fi

if [ $CM4_STATUS -eq 0 ]; then
    echo -e "CM4: ${GREEN}SUCCESS${NC}"
else
    echo -e "CM4: ${RED}FAILED${NC}"
fi

echo ""

# Overall status
if [ $CM7_STATUS -eq 0 ] && [ $CM4_STATUS -eq 0 ]; then
    echo -e "${GREEN}All cores programmed successfully!${NC}"
    echo ""
    echo "Next steps:"
    echo "1. Open serial terminal: screen /dev/tty.usbmodemXXXX 115200"
    echo "2. Press RESET button on the board (black button)"
    echo "3. You should see the counter output"
    exit 0
else
    echo -e "${RED}Some programming failed${NC}"
    exit 1
fi

#!/bin/bash

# STM32H745I-DISCO LTDC Project Build Script
# This script builds both CM7 and CM4 cores

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# GCC toolchain path
GCC_PATH="/Applications/STM32CubeIDE.app/Contents/Eclipse/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.macos64_1.0.0.202411102158/tools/bin"

# Check if GCC toolchain exists
if [ ! -d "$GCC_PATH" ]; then
    echo -e "${RED}Error: ARM GCC toolchain not found at $GCC_PATH${NC}"
    echo "Please install STM32CubeIDE or update the GCC_PATH in this script"
    exit 1
fi

# Add toolchain to PATH
export PATH="$GCC_PATH:$PATH"

# Verify compiler is available
if ! command -v arm-none-eabi-gcc &> /dev/null; then
    echo -e "${RED}Error: arm-none-eabi-gcc not found in PATH${NC}"
    exit 1
fi

echo -e "${GREEN}==================================${NC}"
echo -e "${GREEN}STM32H745I-DISCO Build Script${NC}"
echo -e "${GREEN}==================================${NC}"
echo ""

# Function to build a core
build_core() {
    local CORE=$1
    local BUILD_DIR="${CORE}/Debug"

    echo -e "${YELLOW}Building ${CORE}...${NC}"

    if [ ! -d "$BUILD_DIR" ]; then
        echo -e "${RED}Error: Build directory not found: $BUILD_DIR${NC}"
        return 1
    fi

    cd "$BUILD_DIR"

    # Clean previous build
    echo "  Cleaning..."
    make clean > /dev/null 2>&1

    # Build
    echo "  Compiling..."
    if make -j4 2>&1 | grep -E "error:|Error|failed"; then
        echo -e "${RED}  Build FAILED for ${CORE}${NC}"
        cd ../..
        return 1
    fi

    # Check if ELF file was created
    ELF_FILE="ltdc_${CORE}.elf"
    if [ -f "$ELF_FILE" ]; then
        SIZE=$(ls -lh "$ELF_FILE" | awk '{print $5}')
        echo -e "${GREEN}  Build SUCCESS for ${CORE}${NC}"
        echo "  Output: $ELF_FILE ($SIZE)"

        # Show memory usage
        arm-none-eabi-size "$ELF_FILE"
    else
        echo -e "${RED}  Build FAILED for ${CORE} - ELF file not created${NC}"
        cd ../..
        return 1
    fi

    cd ../..
    return 0
}

# Build CM7
build_core "CM7"
CM7_STATUS=$?

echo ""

# Build CM4
build_core "CM4"
CM4_STATUS=$?

echo ""
echo -e "${GREEN}==================================${NC}"
echo -e "${GREEN}Build Summary${NC}"
echo -e "${GREEN}==================================${NC}"

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
    echo -e "${GREEN}All builds completed successfully!${NC}"
    echo ""
    echo "Binary files:"
    echo "  CM7: CM7/Debug/ltdc_CM7.elf"
    echo "  CM4: CM4/Debug/ltdc_CM4.elf"
    exit 0
else
    echo -e "${RED}Some builds failed${NC}"
    exit 1
fi

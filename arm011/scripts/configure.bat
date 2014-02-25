@ECHO OFF
cmake -G "CodeBlocks - MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm-none-eabi.cmake -DCMAKE_C_FLAGS="-nostartfiles" ../


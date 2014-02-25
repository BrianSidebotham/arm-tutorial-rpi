#!/bin/bash

cmake -G "CodeBlocks - Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm-none-eabi.cmake -DCMAKE_C_FLAGS="-nostartfiles" ../

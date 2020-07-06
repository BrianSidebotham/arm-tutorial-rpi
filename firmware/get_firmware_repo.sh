#!/bin/sh

scriptdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

cd ${scriptdir} && git clone --depth 1 https://github.com/raspberrypi/firmware.git firmware

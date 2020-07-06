#!/bin/sh

scriptdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

if [ -d "${scriptdir}/firmware" ]; then
  rm -rf "${scriptdir}/firmware"
fi

git clone --depth 1 https://github.com/raspberrypi/firmware.git firmware

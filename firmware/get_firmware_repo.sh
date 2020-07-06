#!/bin/sh

scriptdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

if [ -d "${scriptdir}/firmware" ]; then
  ls -la "${scriptdir}/firmware"
  rm -rf "${scriptdir}/firmware"
fi

git clone --depth 1 https://github.com/raspberrypi/firmware.git firmware

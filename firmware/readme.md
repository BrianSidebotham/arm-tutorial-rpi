# Firmware

Use the `get_latest_firmware.sh` script to get the latest firmware from the official raspberry pi
firmware repository.

https://github.com/raspberrypi/firmware

This firmware is used to start the Raspberry-Pi, even when we're talking about bare metal
programming. The GPU (Graphics Processor Unit) is responsible for bootstrapping the ARM processor
and the firmware is loaded and run by the GPU to do the initial booting before starting the ARM.

# Card Scripts

The scripts in this directory support generating card images. These images can then be written
straight to an SD card, ready to boot a raspberry pi.

## Bootable SD Cards

It's important to understand the booting process of the Raspberry Pi so we can generate a bootable
SD Card that the RPi will run from.

There's some information on the [RPi site](https://www.raspberrypi.org/documentation/configuration/config-txt/boot.md)
about the boot options in the `config.txt` file.

From the `readme.md` of the main RPi firmware [respository](https://github.com/raspberrypi/firmware)
there's some information regarding the `bootcode.bin`, `start*.elf` and `fixup*.dat` files.

Basically we need a FAT formatted SD Card with the `bootcode.bin` file and a matching set up
`start.elf` and `fixup.dat` files to get the RPi to run.

## Scripts

Some information about the various scripts and what they do

### create_card_image.sh

Creates a `.img` file which can then be written to a card with the `write_card.sh` tool.

### write_card.sh

Writes a `.img` file to an SD card. This tool generally needs to be run as root so you usually need
to `sudo ./write_card.sh`

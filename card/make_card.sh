#!/bin/sh

# Authors:
# - Brian Sidebotham <brian.sidebotham@gmail.com>

scriptdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

if [ $# -lt 1 ]; then
    echo "usage: make_card.sh <pi-model> [model] [kernel_file]" >&2
    echo "       pi-model options: rpi0, rpi1, rpi1bp, rpi2, rpi3, rpi4, rpibp" >&2
    exit 1
fi

# The raspberry pi model we're targetting
model="${1}"

tutorial="none"
if [ $# -gt 1 ]; then
    tutorial="${2}"
fi

kernel_file=kernel.${tutorial}.${model}.img
if [ $# -gt 2 ]; then
    kernel_file=${3}
fi

diskspace=16

requires="sfdisk mcopy"
for required in ${requires}; do
    which ${required} > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "ERROR: Cannot find ${required} which is required" >&2
        exit 1
    fi
done


disk_image=card.${tutorial}.${model}.img

tmpcardimg=$(mktemp)
tmpcardpart=$(mktemp)
tmpcfg=$(mktemp)

# We generate a dummy file the same size as the disk size. We could just generate a file that has the same size as
# the parition table only, but sfdisk bombs out when you doing, reporting that the partition start position is past
# the end of the image. Perfectly acceptable failure mode I guess.
dd bs=1024 count=$((1024*${diskspace})) if=/dev/zero of="${tmpcardimg}"

# Create the partition table putting the FAT32 partition at 1MiB offset (2048 sector offset with 512 byte sector size)
# to allow for the partition table. We pipe a heredoc to sfdisk

# start=2048 -- The start sector for the partition
# size=      -- The size of the partition (so sfdisk can calculate the amount of sectors consumed by the partition)
# type=c     -- 0xc is the WIN95 FAT partition type
# bootable   -- Set the bootable flag - don't think the RPi takes any notice of this to be fair
cat << EOF | sfdisk "${tmpcardimg}"
label: dos

start=2048,size=${diskspace}M,type=c,bootable
EOF

# These are the normal files required to boot a Raspberry Pi - which come from the official Raspberry Pi github
# firmware repository. For the RPI4 we require a different startup file because it uses a different version of
# VideoCore
startfile=start.elf
fixupfile=fixup.dat

if [ "${model}X" = "rpi4X" ]; then
    startfile=start4.elf
    fixupfile=fixup4.dat
fi

# Create a configuration file, pointing the boot process towards the files it should be using (useful when you've got
# multiple options in place).
cat << EOF > ${tmpcfg}
start_file=${startfile}
fixup_file=${fixupfile}
EOF

# Now we create a separate partition file which we can write the filesystem on. This can then be concatentated to the
# parititon table we generated above
dd bs=1024 count=$(($((${diskspace} - 1)) * 1024)) if=/dev/zero of="${tmpcardpart}"

# Generate a FAT file system (Leave mkfs.fat to work out which FAT to use)
# -I use the entire space of the partition (We've got the partition table elsewhere)
# -S 512 - make sure the geometry is 512 sector size
mkfs.fat -S 512 -I "${tmpcardpart}"

# mcopy is cool - it works with fat file systems in image files which saves us doing these things as root on a
# mounted loop device which is always flaky as hell.

# See the source code for the -i option which sets a disk disk_image instead of a device.
# https://github.com/Distrotech/mtools/blob/master/mcopy.c
# It's also mentioned in "man mtools" too
# Copy the kernel image file to the image's FAT file system and name the target file kernel.img
mcopy -v -i ${tmpcardpart} ${kernel_file} ::kernel.img

# Copy the rest of the files required in the same way
mcopy -v -i ${tmpcardpart} ${scriptdir}/../firmware/firmware/boot/bootcode.bin  ::bootcode.bin
mcopy -v -i ${tmpcardpart} ${scriptdir}/../firmware/firmware/boot/${fixupfile}  ::${fixupfile}
mcopy -v -i ${tmpcardpart} ${scriptdir}/../firmware/firmware/boot/${startfile}  ::${startfile}
mcopy -v -i ${tmpcardpart} ${tmpcfg}                                               ::config.txt

# Stich the disk image together by copying the partition table from the fake disk image and then concatentate the FAT
# file system partition on the end
dd bs=512 if="${tmpcardimg}" of="${disk_image}" count=2048
cat ${tmpcardpart} >> "${disk_image}"

rm -f ${tmpcardpart}
rm -f ${tmpcardimg}
rm -f ${tmpcfg}

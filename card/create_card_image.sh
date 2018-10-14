#!/bin/sh

scriptdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
base=${scriptdir}/..

. ${base}/shell/common_functions.sh

# Require being root to run this script as we need disk partitioning tools which need to use the
# kernel device-mapper driver and that fails unless you have elevated permissions - even though we
# just want to mount and partition an .img file!

if [ "$(id -u)" -ne 0 ]; then
    echo "You must run this script using sudo, or as root" >&2
    exit 1
fi

# Create a Bootable RPi SD Card image that can be directly written to an SD Card
# The card image size in MiB
image_size=16

# The place to mount the loop in order to write files to the disk image
mount_point=$(mktemp -d)

usage() {
    printf "%s\n" "usage: ${0} kernel_file disk_name.img"
}

if [ $# -lt 2 ]; then
    usage
    exit 1
fi

kernel_file=${1}
image_name=${2}

# Quit if the kernel file is not found
if [ ! -f "${kernel_file}" ]; then
    printf "%s\n" "kernel_file ${kernel_file} does not exist" >&2
    exit 1
fi

# Pre-requisites
required_tools="dd kpartx parted partprobe sync mkfs.vfat"
must_have ${required_tools}

if [ -f "${image_name}" ]; then
    # If the image name already exists, unmount loops and start again by deleting the image file
    losetup -D
    rm -f "${image_name}"
fi

must_run dd if=/dev/zero of="${image_name}" bs=1M count=${image_size} > /dev/null 2>&1
must_run kpartx -av ${image_name}

all_loops=$(sudo losetup -a)
printf "%s\n" "all_loops: ${all_loops}"

disk=$(echo "${all_loops}" | grep -v "deleted" | grep -o -m 1 "/dev/loop[0-9].*${image_name}" | grep -o "/dev/loop[0-9]")
echo "Using ${disk} as the target"

if [ "${disk}X" = "X" ]; then
    echo "Could not find a loop device to use!" >&2
fi

must_run parted -s ${disk} mklabel msdos
must_run parted -s ${disk} mkpart primary fat32 1M ${image_size}
must_run sync
must_run partprobe ${disk}
must_run mkfs.vfat ${disk}p1
must_run mount ${disk}p1 ${mount_point}

must_run cp -rv "${base}/firmware/firmware/boot/bootcode.bin" ${mount_point}/
must_run cp -rv "${base}/firmware/firmware/boot/fixup.dat" ${mount_point}/
must_run cp -rv "${base}/firmware/firmware/boot/start.elf" ${mount_point}/

# If we need to do anything with configuration files - do it in the heredoc
sudo cat << EOF > ${mount_point}/config.txt
start_file=start.elf
fixup_file=fixup.dat
EOF

printf "%s\n" "Copying ${kernel_file} to kernel.img on the card image"
must_run cp -v ${kernel_file} ${mount_point}/kernel.img

must_run sync
must_run umount ${disk}p1

#!/bin/sh

scriptdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

if [ $# -lt 2 ]; then
    removable_devices=$(lsblk --ascii --output KNAME,SIZE,RM,TYPE | grep -E '^[a-zA-Z0-9]+[ ]+[0-9MGK\.]+[ ]+1 disk[ ]?$')

    device_options=""
    for device in "${removable_devices}"; do
        device_name=$(printf "%s\n" "${device}" | awk '{print $1}')
        device_size=$(printf "%s\n" "${device}" | awk '{print $2}')
        device_options="${device_name} (${device_size})
${device_options}"
    done

    echo "Please select the device to write: "
    printf "%s\n" "${device_options}" | nl

    option=""
    while true; do
        read -p ': ' option
        if [ "${option}" -eq "${option}" ] && [ "${option}" -gt 0 ]; then
            break
        fi
    done

    value="$(printf "%s\n" "${device_options}" | sed -n "${option}p")"
    echo "Using ${value}. Are you sure?"

    while true; do
        read -p '(yes/NO): ' option
        if [ "${option}X" = "yesX" ]; then
            break
        fi
        echo "OK, Exiting..."
        exit 0
    done
    disk="/dev/$(echo "${value}" | awk '{print $1}')"
else
    disk=${2}
fi

image_name=${1}

if [ ! -f ${image_name} ]; then
    echo "Cannot find ${image_name}" >&2
    exit 1
fi

printf "%s\n" "Writing ${image_name} to ${disk}"

# cat is generally quicker than dd
cat ${image_name} > ${disk}

# Make sure all writing is complete
sync && eject ${disk}

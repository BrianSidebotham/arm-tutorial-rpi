#!/bin/sh

# Use an include guard
if [ "${__valvers_arm_rpi_common_functions}X" != "X" ]; then
    return
fi

__valvers_arm_rpi_common_functions="included"

must_run() {
    $@
    if [ $? -ne 0 ]; then
        echo "$@ failed to run!" >&2
        exit 1
    fi
}

must_have() {
    while [ $# -ne 0 ]; do
        which $1 > /dev/null 2>&1
        if [ $? -ne 0 ]; then
            echo "$1 is required to run this script!" >&2
            exit 1
        fi
        shift
    done
}

#!/bin/sh

scriptdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

exit_if_errors() {
    $@
    if [ $? -ne 0 ]; then
        echo "ERROR: Running $@" >&2
        exit 1
    fi
}

# We can either use an environment variable or input argument to control the
# RPI model we're building for
if [ $# -gt 0 ]; then
    export RPI="${1}"
fi

if [ "${RPI}X" = "X" ]; then
    echo "The environment variable RPI must be set to a support raspberry pi model" >&2
    exit 1
fi


exit_if_errors ${scriptdir}/part-1/armc-02/build.sh ${RPI}
exit_if_errors ${scriptdir}/part-1/armc-03/build.sh ${RPI}

exit_if_errors ${scriptdir}/part-2/armc-04/build.sh ${RPI}
exit_if_errors ${scriptdir}/part-2/armc-05/build.sh ${RPI}
exit_if_errors ${scriptdir}/part-2/armc-06/build.sh ${RPI}
exit_if_errors ${scriptdir}/part-2/armc-07/build.sh ${RPI}
exit_if_errors ${scriptdir}/part-2/armc-08/build.sh ${RPI}
exit_if_errors ${scriptdir}/part-2/armc-09/build.sh ${RPI}

exit_if_errors ${scriptdir}/part-3/armc-010/build.sh ${RPI}
exit_if_errors ${scriptdir}/part-3/armc-011/build.sh ${RPI}
exit_if_errors ${scriptdir}/part-3/armc-012/build.sh ${RPI}

exit_if_errors ${scriptdir}/part-4/armc-013/build.sh ${RPI}

exit_if_errors ${scriptdir}/part-5/armc-014/build.sh ${RPI}
exit_if_errors ${scriptdir}/part-5/armc-015/build.sh ${RPI}
exit_if_errors ${scriptdir}/part-5/armc-016/build.sh ${RPI}

exit 0

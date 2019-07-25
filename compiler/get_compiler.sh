#!/bin/sh

scriptdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

# Include the common shell functions
. ${scriptdir}/../shell/common_functions.sh

compiler=gcc-arm-none-eabi-7-2018-q2-update
archive=${compiler}-linux.tar.bz2
source=https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/7-2018q2/${archive}

must_have curl
must_have md5sum
must_have tar

if [ ! -f ${scriptdir}/${archive} ]; then
    must_run curl --fail -o ${scriptdir}/${archive} ${source}
fi

# Generate an MD5 file on the fly - we know the MD5 of the compiler package we're after, and the filename needs to
# include the full path to the archive
cat << EOF > ${scriptdir}/.compiler.md5
299ebd3f1c2c90930d28ab82e5d8d6c0  ${scriptdir}/${archive}
EOF

must_run md5sum -c ${scriptdir}/.compiler.md5
must_run tar -C ${scriptdir} -xf ${scriptdir}/${archive}

# Remove the archive after we're finished with it
rm -rf ${scriptdir}/${archive} > /dev/null 2>&1

# Test the compiler before quitting
must_run ${scriptdir}/${compiler}/bin/arm-none-eabi-gcc --version

# Create a compiler configuration file
cat << EOF > "${scriptdir}/.compiler_config"
#!/bin/sh
tcpath=${scriptdir}/${compiler}/bin
toolchain=${scriptdir}/${compiler}/bin/arm-none-eabi-
EOF

exit 0

#!/usr/bin/env bash

# Check args
if [ $# -lt 1 ] || [ -z $1 ]; then
	echo "This script objcopies an archive into an object file to be embedded in an executable"
	echo "Usage: $0 <archive> [<object name>]"
	exit
fi;

# Check if there's a different object file name given
if [ $# -gt 1 ]; then
	output=${2}
else
	output="embed.o"
fi

# Check platform to determine output format
platform="$(uname -s)"
if [ ${platform} == "CYGWIN*" ] || [ ${platform} == "MINGW*" ]; then
	fmt="pe-x86-64" # I really hope this allows for MSVC compatibility
else
	fmt="elf64-x86-64"
fi
echo "Using output format ${fmt} because script is running under ${platform}..."

# Objcopy
echo "Objcopying archive ${1} into ${output}..."
rm ${output}
objcopy --input binary --output $fmt --binary-architecture i386:x86-64 --rename-section .data=.rodata,contents,alloc,load,readonly,data ${1} ${output}
if ! [ -z $? ]; then
	exit
fi

echo "Done!"

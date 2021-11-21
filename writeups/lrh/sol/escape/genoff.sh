#! /bin/sh

echo '#pragma once'
echo

SECRET=$1
[ "$SECRET" != "" ] || SECRET=./secret

objdump -S $SECRET | grep 'pop *%r15' | awk '{ print $1 }' | python -c 'print("#define OFF_POPRDI " + hex(int(input()[:-1], 16) + 1))'
objdump -S $SECRET | grep "<print_flag>:" | awk '{ print "#define OFF_PRTFLAG 0x" $1}'
objdump -S $SECRET | grep -A1 'call .* <work>' | tail -1 | awk '{ print "#define OFF_WORKRA 0x" substr($1, 0, length($1) - 1) }'

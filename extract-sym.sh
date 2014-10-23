#!/bin/bash

TC_PREFIX=arm-none-eabi-

function extract_sym()
{
    local SYM=$1
    local OUT=$2
    local SYMINFO=$(${TC_PREFIX}objdump -t $BINARY | grep -E "$SYM" | \
        sed -r 's/\s+/ /g')
    local SECT="$(echo $SYMINFO | cut -d' ' -f 4)"
    local STARTADDR="0x$(echo $SYMINFO | cut -d' ' -f 1)"
    local SIZE="0x$(echo $SYMINFO | cut -d' ' -f 5)"

    # Extract section (apparently can't seek to STARTADDR in the binary)
    SECT_OUT=$(mktemp)
    SECT_START="0x$(${TC_PREFIX}objdump -j $SECT -h $BINARY | sed -n '6p' \
        | sed -r 's/\s+/ /g' | cut -d' ' -f5)"
    ${TC_PREFIX}objdump -s --adjust-vma=-$SECT_START -j $SECT $BINARY | \
        sed '1,4d' | xxd -r > $SECT_OUT

    STARTADDR_SECT=$(($STARTADDR-$SECT_START))
    xxd -s $STARTADDR_SECT -l $SIZE $SECT_OUT | \
        xxd -r -seek -$STARTADDR_SECT > $OUT
}

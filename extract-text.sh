#!/bin/bash

#SECTION=xdc.noload
#OFFSET=-0x$(objdump -j $SECTION -h eventgen.x86U | sed -n '6p'  | sed -r 's/\s+/ /g' | cut -d' ' -f5)
#objdump -s --adjust-vma=$OFFSET -j $SECTION eventgen.x86U | sed '1,4d' | xxd -r > $SECTION.bin
#
#cat $SECTION.bin | xxd -s 0x60 | xxd -r -seek -0x60 > chartab.bin
#cat $SECTION.bin | xxd -l 0x58 | xxd -r > nodetab.bin

BINARY=$1

source extract-sym.sh

extract_sym charTab__A chartab.bin
extract_sym nodeTab__A nodetab.bin

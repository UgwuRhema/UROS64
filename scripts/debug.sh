#!/bin/bash

# please run 'make gdb' before running this script

cd "$(dirname "$0")" || exit 1

# Load the ELF (not the raw binary) so GDB has symbols and source line info.
gdb --silent ../debug/uroskrnl.elf \
    -ex "target remote localhost:1234"

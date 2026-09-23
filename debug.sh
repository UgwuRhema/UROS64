#!/bin/bash

#please run 'make gdb' before running this script
gdb --silent -ex "target remote localhost:1234"

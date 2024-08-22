#!/usr/bin/bash

set -xe

cppcheck --inconclusive --enable=all --suppress=missingInclude --output-file="cppcheck-output.txt" -I ../19-under-the-c/UNDER_THE_C/Headers/ ../19-under-the-c/UNDER_THE_C/Sources/*

echo "finished cppcheck"


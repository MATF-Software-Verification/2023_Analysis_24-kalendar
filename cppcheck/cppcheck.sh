#!/usr/bin/bash

set -xe

cppcheck --quiet --inconclusive --enable=all --std=c++17 --suppress=missingInclude --output-file="cppcheck-output-$(date).txt" -I ~/Desktop/vs/2023_Analysis_24-kalendar/19-under-the-c/UNDER_THE_C/Headers/ /home/jovana/Desktop/vs/2023_Analysis_24-kalendar/19-under-the-c/UNDER_THE_C/Sources/*

echo "finished cppcheck"

echo "finished cppcheck"

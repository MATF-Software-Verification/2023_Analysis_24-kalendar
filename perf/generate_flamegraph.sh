#!/bin/bash

# Set variables
TARGET_APP=/home/jovana/Desktop/vs/2023_Analysis_24-kalendar/19-under-the-c/UNDER_THE_C/build/Desktop_Qt_5_15_1_GCC_64bit-Debug/UNDER_THE_C
FLAMEGRAPH_DIR=./FlameGraph
PERF_DATA="perf.data"
PERF_SCRIPT_OUT="out.perf"
FOLDED_FILE="perf.folded"
FLAMEGRAPH_SVG="flamegraph.svg"

# Check if the target application is provided
if [ -z "$TARGET_APP" ]; then
    echo "Usage: $0 <target-application> <flamegraph-directory>"
    exit 1
fi

# Check if the FlameGraph directory is provided
if [ -z "$FLAMEGRAPH_DIR" ]; then
    echo "Usage: $0 <target-application> <flamegraph-directory>"
    exit 1
fi

# Check if FlameGraph directory exists
if [ ! -d "$FLAMEGRAPH_DIR" ]; then
    echo "FlameGraph directory $FLAMEGRAPH_DIR not found!"
    exit 1
fi

# Start recording with perf
echo "Recording performance data for $TARGET_APP..."
sudo perf record -F 99 -g -- $TARGET_APP

# Generate perf script output
echo "Generating perf script output..."
sudo perf script > $PERF_SCRIPT_OUT

# Generate folded stack output
echo "Generating folded stack output..."
$FLAMEGRAPH_DIR/stackcollapse-perf.pl $PERF_SCRIPT_OUT > $FOLDED_FILE

# Generate FlameGraph
echo "Generating FlameGraph..."
$FLAMEGRAPH_DIR/flamegraph.pl $FOLDED_FILE > $FLAMEGRAPH_SVG

# Clean up intermediate files
rm -f $PERF_DATA $PERF_SCRIPT_OUT $FOLDED_FILE

echo "FlameGraph generated: $FLAMEGRAPH_SVG"

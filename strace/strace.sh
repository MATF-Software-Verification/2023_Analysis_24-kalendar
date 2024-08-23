#!/bin/bash


program="../19-under-the-c/UNDER_THE_C/build/Desktop_Qt_5_15_1_GCC_64bit-Debug/UNDER_THE_C"
output_file="strace_output_$(date +%Y%m%d_%H%M%S).txt"

# Run the program with strace
strace -o "$output_file" "$program"

# Check the exit status of strace
exit_status=$?

# Display the exit status
echo "strace exit status: $exit_status"

# Display the strace output file location
echo "strace output saved to: $output_file"

# Display a portion of the strace output
echo "Preview of the strace output:"
head -n 10 "$output_file"


#!/bin/bash

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 <program> [args...]"
    exit 1
fi

program="$1"
output_file="strace_output.txt"

# Run the program with strace
strace -o "$output_file" "$@"

# Check the exit status of strace
exit_status=$?

# Display the exit status
echo "strace exit status: $exit_status"

# Display the strace output file location
echo "strace output saved to: $output_file"

# Display a portion of the strace output
echo "Preview of the strace output:"
head -n 10 "$output_file"

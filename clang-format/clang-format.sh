#!/bin/bash

# Directory containing the C++ files to format
input_dir="/mnt/c/Users/adzic/Desktop/vs/2023_Analysis_24-kalendar/2023_Analysis_24-kalendar/24-kalendar/calendar/sources"

# Loop through all C++ files in the input directory
for file in "$input_dir"/*.cpp; do
  # Check if the file exists
  if [ -f "$file" ]; then
    # Get the filename without the directory path
    filename=$(basename -- "$file")

    # Use clang-format to format the file and write the result to a new file
    # clang-format -i "$file"
    # formatted_file="${filename%.cpp}_formatted.cpp"
    # clang-format -style=file "$file" > "$formatted_file"
     # Use clang-format to format the file
    # clang-format -i "$file"

    # # Use diff to write the difference between the original and formatted files to a new file
    # diff -u "$file" "${filename%.cpp}_formatted.cpp" > "${filename%.cpp}_diff.txt"
     # Backup the original file
    cp "$file" "${filename%.cpp}_original.cpp"

    # Use clang-format to format the file
    clang-format -i "$file"

    # Use diff to write the difference between the original and formatted files to a new file
    diff -u "${filename%.cpp}_original.cpp" "$file" > "${filename%.cpp}_diff.txt"

    # Print the name of the diff file
    echo "Diff file: $filename.diff.txt"
    # # Print the name of the formatted file
    # echo "Formatted file: $filename"

  fi
done





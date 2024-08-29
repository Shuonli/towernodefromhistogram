#!/bin/bash

source /sphenix/u/shuhang98/setup.sh

echo "------------------setting up environment--------------------"
export Cur_dir=$(pwd)
echo "running area:" ${Cur_dir}
echo "-------------------------running----------------------------"
cd ${Cur_dir}
ls

# Read the single line from file.txt to get the file path
if [[ -f "file.txt" ]]; then
    file_path=$(cat file.txt)
    echo "Using file path from file.txt: ${file_path}"
else
    echo "file.txt not found in the current directory!"
    exit 1
fi

# Run the ROOT macro with the file path as an argument
root -q -b "Fun4All_runDST.C(\"${file_path}\")"

echo "JOB COMPLETE!"
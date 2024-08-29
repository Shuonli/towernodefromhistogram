#!/bin/bash

# Read the list of files from files.txt
i=0
export macropath=$(pwd)
while IFS= read -r file_path; do
    # Extract the directory path before the .root file
    dir_path=$(dirname "$file_path")

    subdir_name=$(basename "OutDir$i")

    # Create the subdirectory in the current directory
    mkdir -p "$subdir_name"

    # Write the path to the .root file in file.txt inside the subdirectory
    echo "$file_path" > "$subdir_name/file.txt"

    pushd "$subdir_name"

    export WorkDir=$(pwd)

    # Copy the macro and the script to the subdirectory
    cp -v "$macropath/CondorRun.sh" "CondorRunTC$i.sh"
    cp -v "$macropath/Fun4All_runDST.C" "Fun4All_runDST.C"

    cat >>ff.sub<< EOF
+JobFlavour                   = "microcentury"
transfer_input_files          = ${WorkDir}/CondorRunTC$i.sh,${WorkDir}/Fun4All_run_dst.C
Executable                    = CondorRunTC$i.sh
request_memory                = 4GB
Universe                      = vanilla
Notification                  = Never
GetEnv                        = True
Priority                      = +80
Output                        = test.out
Error                         = test.err
Log                           = /tmp/sli_$i_$j.log
Notify_user                   = sl4859@columbia.edu

Queue
EOF

    condor_submit ff.sub
    i=$((i+1))
    popd

done < files.txt



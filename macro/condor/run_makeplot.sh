#!/bin/bash

ndir=32

export macropath="/sphenix/user/shuhangli/FastSimValid/macro"

for i in $(seq 0 $ndir); do
    subdir_name=$(basename "OutDir$i")
    pushd "$subdir_name"
    root -b -q "$macropath/Plotpi0.C"
    root -b -q "$macropath/Plotcorr.C"
    popd
done
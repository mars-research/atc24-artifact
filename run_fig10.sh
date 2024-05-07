#!/usr/bin/env bash


cd user-trampoline-arm64
rm $PWD/../fig10.csv
echo "ARM_C" >> $PWD/../fig10.csv
git checkout atc_arm_c
nix develop --command bash -c  "make && taskset -c 8 ./nfv_c 1 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_c 4 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_c 8 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_c 16 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_c 32 >> $PWD/../fig10.csv && exit"
cd ..
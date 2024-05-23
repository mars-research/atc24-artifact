#!/usr/bin/env bash
set -euo pipefail

cd user-trampoline-arm64
rm $PWD/../fig10.csv
rm -f nfv_* *.o *.nosfi *.sfi 
echo "ARM_C" >> $PWD/../fig10.csv
git checkout atc_arm_c
nix develop --command bash -c  "make && taskset -c 8 ./nfv_c 1 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_c 4 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_c 8 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_c 16 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_c 32 >> $PWD/../fig10.csv && exit"
rm -f nfv_* *.o *.nosfi *.sfi 

echo "ARM_NACL" >> $PWD/../fig10.csv
git checkout atc_arm_nacl
nix develop --command bash -c  "make && taskset -c 8 ./nfv_nacl 1 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_nacl 4 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_nacl 8 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_nacl 16 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_nacl 32 >> $PWD/../fig10.csv && exit"
rm -f nfv_* *.o *.nosfi *.sfi 

echo "ARM_MTE" >> $PWD/../fig10.csv
git checkout atc_arm_mte
nix develop --command bash -c  "make && taskset -c 8 ./nfv_mte 1 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_mte 4 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_mte 8 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_mte 16 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_mte 32 >> $PWD/../fig10.csv && exit"
rm -f nfv_* *.o *.nosfi *.sfi 

echo "ARM_PAC" >> $PWD/../fig10.csv
git checkout atc_arm_pac
nix develop --command bash -c  "make && taskset -c 8 ./nfv_pac 1 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_pac 4 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_pac 8 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_pac 16 >> $PWD/../fig10.csv && taskset -c 8 ./nfv_pac 32 >> $PWD/../fig10.csv && exit"
rm -f nfv_* *.o *.nosfi *.sfi 

cd ..

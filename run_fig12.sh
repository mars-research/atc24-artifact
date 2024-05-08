#!/usr/bin/env bash


cd user-trampoline
rm $PWD/../fig12.csv
echo "X86_GS" >> $PWD/../fig12.csv
git checkout atc_gs
nix develop --command bash -c  "make && ./nfv_gs 1 >> $PWD/../fig12.csv && ./nfv_gs 4 >> $PWD/../fig12.csv && ./nfv_gs 8 >> $PWD/../fig12.csv && ./nfv_gs 16 >> $PWD/../fig12.csv && ./nfv_gs 32 >> $PWD/../fig12.csv && exit"
cd ..
#!/usr/bin/env bash
set -euo pipefail

cd user-trampoline
rm $PWD/../fig12.csv
rm -f *.o ipc.* *.so nfv_*

echo "X86_C" >> $PWD/../fig12.csv
git checkout atc_mpk_cet
nix develop --command bash -c  "make && ./nfv_c 1 >> $PWD/../fig12.csv && ./nfv_c 4 >> $PWD/../fig12.csv && ./nfv_c 8 >> $PWD/../fig12.csv && ./nfv_c 16 >> $PWD/../fig12.csv && ./nfv_c 32 >> $PWD/../fig12.csv && exit"
echo "X86_C-ideal" >> $PWD/../fig12.csv
nix develop --command bash -c  "make && ./nfv_ideal 1 >> $PWD/../fig12.csv && ./nfv_ideal 4 >> $PWD/../fig12.csv && ./nfv_ideal 8 >> $PWD/../fig12.csv && ./nfv_ideal 16 >> $PWD/../fig12.csv && ./nfv_ideal 32 >> $PWD/../fig12.csv && exit"
echo "X86_MPK" >> $PWD/../fig12.csv
nix develop --command bash -c  "make && ./nfv_mpk 1 >> $PWD/../fig12.csv && ./nfv_mpk 4 >> $PWD/../fig12.csv && ./nfv_mpk 8 >> $PWD/../fig12.csv && ./nfv_mpk 16 >> $PWD/../fig12.csv && ./nfv_mpk 32 >> $PWD/../fig12.csv && exit"
rm -f *.o ipc.* *.so nfv_*

echo "X86_NaCl" >> $PWD/../fig12.csv
git checkout atc_nacl
nix develop --command bash -c  "make && ./nfv_nacl 1 >> $PWD/../fig12.csv && ./nfv_nacl 4 >> $PWD/../fig12.csv && ./nfv_nacl 8 >> $PWD/../fig12.csv && ./nfv_nacl 16 >> $PWD/../fig12.csv && ./nfv_nacl 32 >> $PWD/../fig12.csv && exit"

echo "X86_GS" >> $PWD/../fig12.csv
git checkout atc_gs
nix develop --command bash -c  "make && ./nfv_gs 1 >> $PWD/../fig12.csv && ./nfv_gs 4 >> $PWD/../fig12.csv && ./nfv_gs 8 >> $PWD/../fig12.csv && ./nfv_gs 16 >> $PWD/../fig12.csv && ./nfv_gs 32 >> $PWD/../fig12.csv && exit"
rm -f *.o ipc.* *.so nfv_*

cd ..

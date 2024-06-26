#!/usr/bin/env bash
set -euo pipefail

cd morello-mars
rm ../fig11.csv 
nix develop --command bash -c  "cd ../user-trampoline-arm64 && git checkout atc_cheri && make -j && echo "morello_c" >> $PWD/../fig11.csv &&  ./nfv_c 1 >> $PWD/../fig11.csv && ./nfv_c 4 >> $PWD/../fig11.csv && ./nfv_c 8 >> $PWD/../fig11.csv && ./nfv_c 16 >> $PWD/../fig11.csv && ./nfv_c >> $PWD/../fig11.csv && exit"
nix develop --command bash -c  "cd ../dome && git checkout atc_morello_hybrid && meson build && meson configure -Dbuildtype=release -Ddefault-engine=morello build/ && ninja -C build && ./domains/build-domains.sh morello && echo "morello_hybrid" >> $PWD/../fig11.csv &&  ./build/main 1 >> $PWD/../fig11.csv && ./build/main 4 >> $PWD/../fig11.csv && ./build/main 8 >> $PWD/../fig11.csv && ./build/main 16 >> $PWD/../fig11.csv && ./build/main 32 >> $PWD/../fig11.csv && exit"
nix develop --command bash -c  "cd ../dome && git checkout atc_morello_revocation && meson build && meson configure -Dbuildtype=release -Ddefault-engine=morello build/ && ninja -C build && ./domains/build-domains.sh morello && echo "morello_hybrid_revocation" >> $PWD/../fig11.csv &&  ./build/main 1 >> $PWD/../fig11.csv && ./build/main 4 >> $PWD/../fig11.csv && ./build/main 8 >> $PWD/../fig11.csv && ./build/main 16 >> $PWD/../fig11.csv && ./build/main 32 >> $PWD/../fig11.csv && exit"

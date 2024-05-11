cd morello-mars
nix develop --command bash -c  "cd ../user-trampoline-arm64 && git checkout atc_cheri && make -j && echo "morello_c_normal_kernel" >> $PWD/../fig11.csv &&  ./nfv_c 1 >> $PWD/../fig11.csv && ./nfv_c 4 >> $PWD/../fig11.csv && ./nfv_c 8 >> $PWD/../fig11.csv && ./nfv_c 16 >> $PWD/../fig11.csv && ./nfv_c >> $PWD/../fig11.csv && exit"

#!/bin/bash

cd spec-env
nix develop --command bash -c "cd specCPU2006 && SPEC=\$PWD bin/runspec --config=linux_x86_c --iterations 1 401.bzip2 429.mcf 433.milc 444.namd 445.gobmk 453.povray 458.sjeng 462.libquantum 464.h264ref 470.lbm 473.astar 482.sphinx3 403.gcc 471.omnetpp && exit"
nix develop --command bash -c "cd specCPU2006 && SPEC=\$PWD bin/runspec --config=linux_x86_reg --iterations 1 401.bzip2 429.mcf 433.milc 444.namd 445.gobmk 453.povray 458.sjeng 462.libquantum 464.h264ref 470.lbm 473.astar 482.sphinx3 403.gcc 471.omnetpp && exit"
nix develop --command bash -c "cd specCPU2006 && SPEC=\$PWD bin/runspec --config=linux_x86_ldst --iterations 1 401.bzip2 429.mcf 433.milc 444.namd 445.gobmk 453.povray 458.sjeng 462.libquantum 464.h264ref 470.lbm 473.astar 482.sphinx3 403.gcc 471.omnetpp && exit"
nix develop --command bash -c "cd specCPU2006 && SPEC=\$PWD bin/runspec --config=linux_x86_retcall --iterations 1 401.bzip2 429.mcf 433.milc 444.namd 445.gobmk 453.povray 458.sjeng 462.libquantum 464.h264ref 470.lbm 473.astar 482.sphinx3 403.gcc 471.omnetpp && exit"
nix develop --command bash -c "cd specCPU2006 && SPEC=\$PWD bin/runspec --config=linux_x86_nacl --iterations 1 401.bzip2 429.mcf 433.milc 444.namd 445.gobmk 453.povray 458.sjeng 462.libquantum 464.h264ref 470.lbm 473.astar 482.sphinx3 403.gcc 471.omnetpp && exit"
nix develop --command bash -c "cd specCPU2006 && SPEC=\$PWD bin/runspec --config=linux_x86_gs --iterations 1 401.bzip2 429.mcf 433.milc 444.namd 445.gobmk 453.povray 458.sjeng 462.libquantum 464.h264ref 470.lbm 473.astar 482.sphinx3 403.gcc 471.omnetpp && exit"
nix develop --command bash -c "cd specCPU2006 && SPEC=\$PWD bin/runspec --config=linux_x86_mpk --iterations 1 401.bzip2 429.mcf 433.milc 444.namd 445.gobmk 453.povray 458.sjeng 462.libquantum 464.h264ref 470.lbm 473.astar 482.sphinx3 403.gcc 471.omnetpp && exit"
cd ..
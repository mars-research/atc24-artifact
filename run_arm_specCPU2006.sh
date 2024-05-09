#!/bin/bash

cd spec-env
nix develop --command bash -c "cd specCPU2006 && SPEC=\$PWD bin/runspec --config=linux_arm_c --iteration 3 --rebuild 401.bzip2 429.mcf 433.milc 444.namd 445.gobmk 453.povray 458.sjeng 462.libquantum 464.h264ref 470.lbm 473.astar && exit"
nix develop --command bash -c "cd specCPU2006 && SPEC=\$PWD bin/runspec --config=linux_arm_pac --iteration 3 --rebuild 401.bzip2 429.mcf 433.milc 444.namd 445.gobmk 453.povray 458.sjeng 462.libquantum 464.h264ref 470.lbm 473.astar && exit"
nix develop --command bash -c "cd specCPU2006 && SPEC=\$PWD bin/runspec --config=linux_arm_nacl --iteration 3 --rebuild 401.bzip2 429.mcf 433.milc 444.namd 445.gobmk 453.povray 458.sjeng 462.libquantum 464.h264ref 470.lbm 473.astar && exit"
nix develop --command bash -c "cd specCPU2006 && SPEC=\$PWD bin/runspec --config=linux_arm_reg --iteration 3 --rebuild 401.bzip2 429.mcf 433.milc 444.namd 445.gobmk 453.povray 458.sjeng 462.libquantum 464.h264ref 470.lbm 473.astar && exit"
nix develop --command bash -c "cd specCPU2006 && SPEC=\$PWD bin/runspec --config=linux_arm_mte --iteration 3 --rebuild 401.bzip2 429.mcf 433.milc 444.namd 445.gobmk 453.povray 458.sjeng 462.libquantum 464.h264ref 470.lbm 473.astar && exit"
nix develop --command bash -c "cd specCPU2006 && SPEC=\$PWD bin/runspec --config=linux_arm_retcall --iteration 3 --rebuild 401.bzip2 429.mcf 433.milc 444.namd 445.gobmk 453.povray 458.sjeng 462.libquantum 464.h264ref 470.lbm 473.astar && exit"
nix develop --command bash -c "cd specCPU2006 && SPEC=\$PWD bin/runspec --config=linux_arm_ldst --iteration 3 --rebuild 401.bzip2 429.mcf 433.milc 444.namd 445.gobmk 453.povray 458.sjeng 462.libquantum 464.h264ref 470.lbm 473.astar && exit"
cd ..
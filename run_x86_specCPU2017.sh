#!/bin/bash

cd spec-env
nix develop --command bash -c "cd specCPU2017 && SPEC=\$PWD bin/runcpu --config=linux_x86_c --iterations=3 --threads=1 -copies=1 intspeed fpspeed && exit"
nix develop --command bash -c "cd specCPU2017 && SPEC=\$PWD bin/runcpu --config=linux_x86_nacl --iterations=3 --threads=1 -copies=1 intspeed fpspeed && exit"
nix develop --command bash -c "cd specCPU2017 && SPEC=\$PWD bin/runcpu --config=linux_x86_mpk --iterations=3 --threads=1 -copies=1 intspeed fpspeed && exit"
nix develop --command bash -c "cd specCPU2017 && SPEC=\$PWD bin/runcpu --config=linux_x86_gs --iterations=3 --threads=1 -copies=1 intspeed fpspeed && exit"
cd ..
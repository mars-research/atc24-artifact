#!/bin/bash

cd spec-env
nix develop --command bash -c "cd specCPU2017 && runcpu --config=morello_specCPU2017 --iterations=3 --threads=1 -copies=1 intspeed fpspeed && exit"
cd ..
#!/bin/bash

cd spec-env
nix develop --command bash -c "cd specCPU2006 && SPEC=\$PWD bin/runspec --config=morello_specCPU2006 --iteration 3 --rebuild all && exit"
cd ..
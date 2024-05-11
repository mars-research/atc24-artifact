# ATC'24 Artifact Evaluation

## Overview

We provide SSH access to the following machines to run our experiments:

- `x86-64`: Framework Laptop 13
    - Intel Core i7-1165G7
- `aarch64`: Pixel 8
    - Tensor G3 (1x Cortex-X3, 4x Cortex-A715, 4x Cortex-A510)
- `morello`: Morello Development Board

For each machine above, the general steps include:

1. SSH into the prepared machine
1. Verify experiment conditions
1. Clone this repository
1. Git submodule init
1. Build and install modified clang
1. Run experiments
1. Verify results in `results`

## `x86-64`

```bash
bash install_x86_clangs.sh
bash run_fig12.sh
bash x86_setup_specCPU2006.sh
bash x86_setup_specCPU2017.sh
bash run_x86_specCPU2006.sh
bash run_x86_specCPU2017.sh
just run-x86-64
```

## `arm`

```bash
bash install_arm_clangs.sh
bash run_fig10.sh
bash arm_setup_specCPU2006.sh
bash run_arm_specCPU2006.sh
```

## `morello`

```bash
bash run_fig11.sh
bash morello_setup_specCPU2006.sh
bash morello_setup_specCPU2017.sh
bash run_morello_specCPU2006.sh
bash run_morello_specCPU2017.sh
```

### Plots

- Figure 8 - Overhead of Safe and Unsafe IPC (`fig8`):
    - `x86-64-null.csv`
    - `x86-64-ideal.csv`

- Figuer 10, Figuer 11, Figuer 12:
    - `fig10.csv`
    - `fig11.csv`
    - `fig12.csv`

### SPECCPU numbers

All the numbers used in the paper are listed in [this google sheet](https://docs.google.com/spreadsheets/d/1QQmUcpg08b73es8k5PawnE1VA0wWnKoAghqjsrcmsGk/edit?usp=sharing).

Please go into `spec-env/specCPU20XX/result` to cross check the result.  

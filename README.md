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
1. Run experiments
1. Verify results in `results`

## `x86-64`

```bash
just run-x86-64
```

### Plots

- Figure 8 - Overhead of Safe and Unsafe IPC (`fig8`):
    - `x86-64-null.csv`
    - `x86-64-ideal.csv`

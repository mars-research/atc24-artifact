# Artifact Evaluation for _Opportunities and Limitations of Modern Hardware Isolation Mechanisms_

Thank you for your time and picking our paper for the artifact evaluation.

This documentation contains steps necessary to reproduce the artifacts for our paper titled **Opportunities and Limitations of Modern Hardware Isolation Mechanisms**.

## Overview

The general steps include:

1. Prepare hardware
1. Set up SPEC
1. Run experiments
1. Verify results

## 1. Prepare hardware

We use the following machines to run our experiments:

- `x86-64`: Framework Laptop 13
    - Intel Core i7-1165G7
- `aarch64`: Pixel 8
    - Tensor G3 (1x Cortex-X3, 4x Cortex-A715, 4x Cortex-A510)
- `morello`: Morello Development Board

> [!TIP]
> To simplify the evaluation process, we provide SSH access to the above machines with the benchmark environment already set up.
> We still include the full manual setup process in this document for reference.

TODO: Verify constant frequency

## 2. Set up SPEC

> [!TIP]
> On prepared nodes, this step has already been done.
> Unmodified copies of the SPEC benchmarks are available at `~/specCPU2006.tar.gz` and `~/specCPU2017.tar.gz`.

We patch SPEC CPU2006 and CPU2017 to fix compilation errors with newer toolchains ([spec2006.patch](https://github.com/mars-research/spec-env/blob/main/spec2006.patch), [spec2017.patch](https://github.com/mars-research/spec-env/blob/main/spec2017.patch)).
Clone this repository, and install SPEC under the following subdirectories:

- CPU2006: `spec2006`
- CPU2017: `spec2017`

After installation, the `spec2006` and `spec2017` directories should both contain the `MANIFEST` file.

Next, run the following commands to apply the patches:

```
just setup-spec2006
just setup-spec2017
```

Our configurations are available under `spec-configs` which will be used in later steps.

## 3. Run experiments

### `x86-64` (Fig. 3, 6, 8, 12)

> [!TIP]
> You can inspect what each target does in `justfile`.

1. Build and install Clang
    - `just clang-x86-64`
    - This can take a long time. For convenience, the prepared nodes already have a warm compilation cache.
1. Reproduce Figure 12 - NF overheads on varying batch sizes (x86)
    - `just fig12` -> `results/fig12.csv`
1. Reproduce Figure 8 - Overhead of Safe and Unsafe IPC
    - `just fig8-x86-64` -> `results/x86-64-ideal.csv`
1. Reproduce SPEC benchmarks (Fig. 3, 6)
    - `just spec2006-x86-64` -> `spec2006/result`
    - `just spec2017-x86-64` -> `spec2017/result`

### `aarch64` (Fig. 4, 7, 8, 10)

1. Build and install Clang
    - `just clang-aarch64`
    - This can take a long time. For convenience, the prepared nodes already have a warm compilation cache.
1. Reproduce Figure 8 - Overhead of Safe and Unsafe IPC
    - `just fig8-aarch64` -> `results/aarch64-ideal.csv`
1. Reproduce Figure 10 - NF overheads on varying batch sizes (AArch64)
    - `just fig10` -> `results/fig10.csv`
1. Reproduce SPEC benchmarks (Fig. 4, 7)
    - `just spec2006-aarch64` -> `spec2006/result`

### `morello` (Fig. 5, 8, 11)

We run our benchmarks on both the mainline kernel (`mainline`) and the Morello-enabled kernel (`morello`).

> [!TIP]
> By default, the prepared node is booted with the Morello-enabled kernel.
> To switch between kernels, run `sudo reboot-mainline` or `sudo reboot-morello`.
> The machine will shut down and be reachable again in around 5 minutes.

Run on Morello-enabled kernel:

1. Reproduce Figure 8 - Overhead of Safe and Unsafe IPC
    - `just fig8-morello` -> `results/morello-ideal.csv`, `results/morello-exswitch.csv`
1. Reproduce Figure 11 - NF overheads on varying batch sizes (Morello)
    - `just fig11-morello` ->  `results/fig11-morello.csv`
1. Reproduce SPEC benchmarks (Fig. 5)
    - `just spec2006-morello` -> `spec2006/result-morello`
    - `just spec2017-morello` -> `spec2017/result-morello`

Run on mainline kernel:

1. Reproduce Figure 11 - NF overheads on varying batch sizes (Morello)
    - `just fig11-mainline` ->  `results/fig11-mainline.csv`
1. Reproduce SPEC benchmarks (Fig. 5)
    - `just spec2006-morello-mainline` -> `spec2006/result-mainline`
    - `just spec2017-morello-mainline` -> `spec2017/result-mainline`

## 4. Verify results

Cross check the results you obtained with the ones on the paper.

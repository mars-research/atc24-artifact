# SPEC Benchmark Environment

## SPEC CPU2017

You need to install SPEC CPU2017 1.0.1 in `specCPU2017`.

### Applying patches
```bash
cd specCPU2017
patch -N -p1 < ../spec2017.patch
```

### Regenerating patches
```bash
diff -Naur -x config -x build -x exe -x run -x result -x tmp specCPU2017.orig/ specCPU2017/ | tee spec2017.patch
```

## SPEC CPU2006

You need to extract SPEC CPU2006 1.0 in `specCPU2006`.
Don't run the install scripts as we will use our toolset with AArch64 support (`specperl`, `specdiff`, `spectar`, etc.).

### Setting up tools

```bash
just setup-spec2006
```

### Running benchmarks

```bash
cd specCPU2006
SPEC=$PWD bin/runspec --config=wasm2006 [...]
```

### Regenerating patches

```bash
just regen-spec2006
```

## Notes

```
clang version 16.0.1
Target: wasm32-unknown-wasi
Thread model: posix
InstalledDir: /nix/store/hd2nbv87wmcq0l0xcvgm18dhdz0d61dg-clang-16.0.1/bin
```

WASI SDK: https://github.com/WebAssembly/wasi-sdk/releases/tag/wasi-sdk-20

We are _not_ using the WASI toolchain shipped in nixpkgs since it forces various flags.

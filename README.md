# SPEC on WebAssembly

## Instructions

You need to install SPEC CPU 2017 1.0.1 in `specCPU2017`.

### Applying patches
```bash
cd specCPU2017
patch -N -p1 < ../spec2017.patch
```

### Regenerating patches
```bash
diff -Naur -x config -x build -x exe -x run -x result -x tmp specCPU2017.orig/ specCPU2017/
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

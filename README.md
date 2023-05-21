# SPEC on WebAssembly

Apply patches:
```bash
cd specCPU2017
patch -N -p1 < ../spec2017.patch
```

Regenerate patches:
```bash
diff -Naur -x config -x build -x exe -x run -x result -x tmp specCPU2017.orig/ specCPU2017/
```

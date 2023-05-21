{
  description = "A simple project";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    mars-std.url = "github:mars-research/mars-std";
    mars-std.inputs.nixpkgs.follows = "nixpkgs";
  };

  outputs = { self, mars-std, ... }: let
    # System types to support.
    supportedSystems = [ "x86_64-linux" "aarch64-linux" ];
  in mars-std.lib.eachSystem supportedSystems (system: let
    pkgs = mars-std.legacyPackages.${system};
    inherit (pkgs) lib;

    wasi-sdk = builtins.fetchTarball {
      name = "wasi-sdk";
      url = "https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-20/wasi-sdk-20.0-linux.tar.gz";
      sha256 = "1rjh19g1mcvaixyp3fs6d9bfa1nqv5b6s6v1nb24q7wbb75y9m8x";
    };
    wasi-toolchain = let
      llvm = pkgs.llvmPackages_16;
      makeNormal = name: pkgs.writeShellScript "wasi-${name}" ''
        exec ${llvm.clang-unwrapped}/bin/${name} \
          --sysroot ${wasi-sdk}/share/wasi-sysroot \
          -isystem ${wasi-sdk}/share/wasi-sysroot/include/c++/v1 \
          -isystem ${wasi-sdk}/share/wasi-sysroot/include \
          -resource-dir=${wasi-sdk}/lib/clang/16 \
          --target=wasm32-wasi \
          "$@"
      '';
      makeThreaded = name: pkgs.writeShellScript "wasi-threads-${name}" ''
        exec ${llvm.clang-unwrapped}/bin/${name} \
          --sysroot ${wasi-sdk}/share/wasi-sysroot \
          -isystem ${wasi-sdk}/share/wasi-sysroot/include/c++/v1 \
          -isystem ${wasi-sdk}/share/wasi-sysroot/include \
          -resource-dir=${wasi-sdk}/lib/clang/16 \
          --target=wasm32-wasi-threads \
          -pthread \
          -Wl,--whole-archive,-lpthread,--no-whole-archive \
          -Wl,--import-memory,--export-memory,--max-memory=67108864 \
          "$@"
      '';
    in pkgs.runCommand "wasi-clang" {} ''
      mkdir -p $out/bin
      ln -s ${makeNormal "clang"} $out/bin/wasi-clang
      ln -s ${makeNormal "clang++"} $out/bin/wasi-clang++
      ln -s ${makeThreaded "clang"} $out/bin/wasi-threads-clang
      ln -s ${makeThreaded "clang++"} $out/bin/wasi-threads-clang++
      ln -s ${llvm.lld}/bin/wasm-ld $out/bin/wasm-ld
    '';
  in {
    devShell = pkgs.mkShellNoCC {
      nativeBuildInputs = with pkgs; [
        wasmtime
        wasi-toolchain
      ];

      NIX_LD = pkgs.stdenv.cc.bintools.dynamicLinker;
      NIX_LD_LIBRARY_PATH = lib.makeLibraryPath [ pkgs.libxcrypt-legacy ];

      WASI_SDK_PATH = wasi-sdk;
    };
  });
}

{
  description = "A simple project";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    mars-std.url = "github:mars-research/mars-std";
    mars-std.inputs.nixpkgs.follows = "nixpkgs";
    nixpkgs-perl520 = {
      url = "github:NixOS/nixpkgs/878bbaf4e9d49c251d90f0790d2e7ebcf622ddd0";
      flake = false;
    };
  };

  outputs = { self, mars-std, nixpkgs-perl520, ... }: let
    # System types to support.
    supportedSystems = [ "x86_64-linux" "aarch64-linux" ];
  in mars-std.lib.eachSystem supportedSystems (system: let
    pkgs = mars-std.legacyPackages.${system};
    llvm = pkgs.llvmPackages_16;
    inherit (pkgs) lib;

    mkShell = pkgs.mkShell.override {
      inherit (llvm) stdenv;
    };

    # Perl 5.20 for CPU2006
    pkgsPerl520 = import nixpkgs-perl520 {
      inherit system;
      overlays = [];
    };
    perl520Packages = pkgsPerl520.callPackage (pkgsPerl520.path + "/pkgs/top-level/perl-packages.nix") {
      overrides = {};
      pkgs = pkgsPerl520 // {
        buildPerlPackage = attrs: (pkgsPerl520.callPackage (pkgsPerl520.path + "/pkgs/development/perl-modules/generic") pkgsPerl520.perl520) ({
          doCheck = false;
        } // attrs);
        perl = pkgsPerl520.perl520;
      };
    };
    perl520Path = pkgsPerl520.lib.makePerlPath (with perl520Packages; [
      CompressBzip2
      XMLSAX XMLNamespaceSupport
      LWPUserAgent
      HTTPMessage HTTPDate
      URI IOStringy EncodeLocale
      GD MailTools FontAFM MIMETypes
    ]);
    # specperl -> specperl520
    specperl520wrapper = pkgs.writeShellScriptBin "specperl" ''
      exec specperl520 "$@"
    '';
    specperl520 = pkgs.writeShellScriptBin "specperl520" ''
      if [ -z "''${SPEC}" ]; then
        >&2 echo "This perl is only intended for SPEC 2006. Set \$SPEC to continue"
        exit 1
      fi

      export PATH="''${PATH:+''${PATH}:}${specperl520wrapper}/bin:$SPEC/bin"
      export PERL5LIB="''${PERL5LIB:+''${PERL5LIB}:}$PWD/bin:${perl520Path}"
      exec ${pkgsPerl520.perl520}/bin/perl "$@"
    '';

    # WASM toolchain
    wasi-sdk = builtins.fetchTarball {
      name = "wasi-sdk";
      url = "https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-20/wasi-sdk-20.0-linux.tar.gz";
      sha256 = "1rjh19g1mcvaixyp3fs6d9bfa1nqv5b6s6v1nb24q7wbb75y9m8x";
    };
    wasi-toolchain = let
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
    devShell = mkShell {
      nativeBuildInputs = with pkgs; [
        bashInteractive
        just
        wasmtime
        wasi-toolchain
        specperl520
      ];

      NIX_LD = pkgs.stdenv.cc.bintools.dynamicLinker;
      NIX_LD_LIBRARY_PATH = lib.makeLibraryPath [ pkgs.libxcrypt-legacy ];

      WASI_SDK_PATH = wasi-sdk;
    };
  });
}

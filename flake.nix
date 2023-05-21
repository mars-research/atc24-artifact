{
  description = "A simple project";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    mars-std.url = "github:mars-research/mars-std";
    mars-std.inputs.nixpkgs.follows = "nixpkgs";
  };

  outputs = { self, mars-std, ... }: let
    # System types to support.
    supportedSystems = [ "x86_64-linux" ];
  in mars-std.lib.eachSystem supportedSystems (system: let
    nativePkgs = mars-std.legacyPackages.${system};
    pkgs = mars-std.legacyPackages.${system}.pkgsCross.wasi32;
    inherit (pkgs) lib;
  in {
    devShell = pkgs.mkShell {
      nativeBuildInputs = with pkgs; [
        nativePkgs.wasmtime
      ];

      NIX_LD = nativePkgs.stdenv.cc.bintools.dynamicLinker;
      NIX_LD_LIBRARY_PATH = lib.makeLibraryPath [ nativePkgs.libxcrypt-legacy ];
    };
  });
}

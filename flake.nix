{
  description = "Dome";

  inputs = {
    mars-std.url = "github:mars-research/mars-std";
    mars-std.inputs.nixpkgs.url = "nixpkgs";
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, mars-std, ... }: let
    # System types to support.
    supportedSystems = [ "x86_64-linux" ];
  in mars-std.lib.eachSystem supportedSystems (system: let
    pkgs = mars-std.legacyPackages.${system};
    mkShell = pkgs.mkShell.override {
      stdenv = pkgs.llvmPackages_14.stdenv;
    };
  in {
    devShell = mkShell {
      nativeBuildInputs = with pkgs; [
        meson ninja
      ];
    };
  });
}

{
  description = "A simple project";

  inputs = {
    mars-std.url = "github:mars-research/mars-std";
    nixpkgs.url = "github:zhaofengli/nixpkgs/zhaofeng-22.11";
  };

  outputs = { self, nixpkgs, mars-std, ... }: let
    # System types to support.
    supportedSystems = [ "x86_64-linux" ];
  in mars-std.lib.eachSystem supportedSystems (system: let
    pkgs = mars-std.legacyPackages.${system};
    linuxPkgs = nixpkgs.legacyPackages.${system};

    src = pkgs.fetchFromGitHub {
      owner = "zhaofengli";
      repo = "linux";
      rev = "7210a20be4ad9471feb1308c1bc4f5f6f7ca1589";
      sha256 = "sha256-TdDSRVi4Af/Jqw2rM5Km2BbOCq/axMuzd06Evdp3piY=";
    };
    version = "5.17.15";

    linux_5_17_cet_headers = linuxPkgs.makeLinuxHeaders {
      inherit src version;
    };

    linux_5_17_cet = linuxPkgs.buildLinux rec {
      modDirVersion = version;

      inherit src;

      kernelPatches = [];

      extraConfig = ''
        X86_SHADOW_STACK y
      '';

      extraMeta.branch = "5.17";
    };
  in {
    devShell = pkgs.mkShell {
      buildInputs = with pkgs; [
        linux_5_17_cet_headers
      ];
      nativeBuildInputs = with pkgs; [
              stdenv nasm gnuplot
      pktgen
      gdb
      clang
      dpdk

      python3 python3Packages.jinja2 python3Packages.flask

      pkgs.rust-bin.nightly."2022-06-19".default
      ];
    };
  });
}

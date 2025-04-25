{
  inputs = {
    mars-std.url = "github:mars-research/mars-std";
  };

  outputs = { self, mars-std, ... }: let
    # System types to support.
    supportedSystems = [ "x86_64-linux" "aarch64-linux" ];
  in mars-std.lib.eachSystem supportedSystems (system: let
    pkgs = mars-std.legacyPackages.${system};
    armPkgs = if system == "aarch64-linux" then pkgs else pkgs.pkgsCross.aarch64-multiplatform;
    armNativePkgs = mars-std.legacyPackages.aarch64-linux;

    mkShell = pkgs.mkShell.override {
      stdenv = armPkgs.llvmPackages_14.stdenv;
    };
  in {
    devShell = mkShell {
      nativeBuildInputs = with pkgs; [
        # Insert dev dependencies here
        qemu 
        gdb
        
        pkg-config
      ];
      buildInputs = with pkgs; [
        dpdk
      ];

      ARM_CFLAGS = let
        gcc = armNativePkgs.stdenv.cc.cc;
        linuxArch = armNativePkgs.stdenv.targetPlatform.linuxArch;
        platform = armNativePkgs.stdenv.targetPlatform.config;
        gccPath = "${gcc}/lib/gcc/${platform}/${gcc.version}";
        libc = armNativePkgs.glibc;
      in "--target=aarch64-pc-linux -I ${libc.dev}/include -L ${gccPath} -B ${gccPath} -L ${libc}/lib -B ${libc}/lib -Wl,-dynamic-linker,${libc}/lib/ld-linux-aarch64.so.1";
    };
  });
}

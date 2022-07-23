{ pkgs ? import <nixpkgs> { } }: with pkgs;
mkShell {
  buildInputs = [
    cmake
    boost
    sqlite
  ];
}

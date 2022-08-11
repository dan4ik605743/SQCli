{ stdenv, lib, fetchgit, cmake, boost, sqlite }:

stdenv.mkDerivation {
  name = "tgbot_cpp";

  /*src = fetchgit {
    url = "https://github.com/dan4ik605743/SQCli";
    sha256 = "sha256-FxMx+a7Y3Es+6a7PTWKgTp5hitEXMRdqrwwiuicnbb8=";
    deepClone = true;
  };*/
  src = ./.;

  nativeBuildInputs = [ 
    cmake 
  ];
  
  buildInputs = [ 
    boost 
    sqlite
  ];

  meta = with lib; {
    description = "CLI utility that allows you to work with the SQLite DBMS";
    homepage = "https://github.com/dan4ik605743/SQCli";
    license = licenses.mit;
    maintainers = with maintainers; [ dan4ik605743 ];
    platforms = platforms.linux;
  };
}

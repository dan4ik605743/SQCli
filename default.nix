{ stdenv, lib, fetchgit, cmake, boost, sqlite }:

stdenv.mkDerivation {
  name = "tgbot_cpp";

  /*src = fetchgit {
    url = "https://github.com/dan4ik605743/SQCli";
    sha256 = "";
    deepClone = true;
  };*/
  src = ../sqcli;

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

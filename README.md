[![Linux](https://github.com/dan4ik605743/SQCli/actions/workflows/linux.yml/badge.svg)](https://github.com/dan4ik605743/SQCli/actions/workflows/linux.yml)
[![CodeFactor](https://www.codefactor.io/repository/github/dan4ik605743/sqcli/badge)](https://www.codefactor.io/repository/github/dan4ik605743/sqcli)

# SQCli 

CLI utility that allows you to work with the SQLite DBMS.

## Dependencies
* boost(>=1.74) 
* cmake 
* sqlite3

## Compilation
```
git clone https://github.com/dan4ik605743/SQCli
cd SQCli
mkdir build && cd build
cmake ../
cmake --build .
```

## Usage
```
sqcli -d /path/to/sample.db
```
//TODO dockerCI
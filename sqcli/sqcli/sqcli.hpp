#ifndef SQCLI_HPP
#define SQCLI_HPP

#include "options/options.hpp"
#include "sqlite3.h"

#include <string>

class sqcli {
   public:
    explicit sqcli(const std::string& path_db);
    ~sqcli();

    void start();

   private:
    friend class options;

    std::string path_db_;
    sqlite3* db_;
    char* err_msg_;
    int status_;
    options options_;
};

#endif  // SQCLI_HPP

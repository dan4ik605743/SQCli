#include "sqcli.hpp"

sqcli::sqcli(const std::string& path_db)
    : path_db_(path_db), err_msg_(0), options_(*this) {
    status_ = sqlite3_open(path_db_.c_str(), &db_);
}

sqcli::~sqcli() {
    sqlite3_close(db_);
}

void sqcli::start() {
    if (!options_.status_check(true)) {
        return;
    }

    while (options_.menu_it_ != 14) {
        options_.menu();
    }
}

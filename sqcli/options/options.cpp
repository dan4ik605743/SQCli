#include "options/options.hpp"
#include "fmt/core.h"

#include <iostream>
#include <algorithm>

namespace {
int callback_vector(void* data,
                    int argc,
                    char** argv,
                    [[maybe_unused]] char** field_name) {
    for (int i{0}; i < argc; i++) {
        static_cast<std::vector<std::string>*>(data)->emplace_back(
            argv[i] != nullptr ? argv[i] : "NULL");
    }

    return 0;
}

int callback_vector_map(void* data, int argc, char** argv, char** field_name) {
    auto* obj{
        static_cast<std::vector<std::map<std::string, std::string>>*>(data)};
    obj->emplace_back();

    for (int i{0}; i < argc; ++i) {
        obj->back().emplace(field_name[i],
                            argv[i] != nullptr ? argv[i] : "NULL");
    }

    return 0;
}
}  // namespace

options::options(const std::string& path_db) noexcept
    : check_default_id{0},
      check_autoincrement_{0},
      check_primary_key_{0},
      check_null_{0},
      db_{nullptr},
      err_msg_{nullptr},
      status_{sqlite3_open(path_db.c_str(), &db_)} {}

options::~options() noexcept {
    sqlite3_close(db_);
}

bool options::select_table(const std::string& table) {
    auto it{std::ranges::find(list_tables_, table)};
    return it != list_tables_.end();
}

void options::create_table(const std::string& table) {
    const std::string COMMAND{fmt::format(
        "CREATE TABLE {}(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT)",
        table)};
    status_ = sqlite3_exec(db_, COMMAND.c_str(), nullptr, nullptr, &err_msg_);
}

void options::table_list() {
    status_ = 0;
    list_tables_.clear();

    const std::string COMMAND{
        "SELECT name FROM sqlite_schema WHERE type='table' ORDER BY name;"};

    status_ = sqlite3_exec(db_, COMMAND.c_str(), callback_vector,
                           static_cast<void*>(&list_tables_), &err_msg_);
}

void options::rename_table(const std::string& table,
                           const std::string& new_table) {
    status_ = 0;

    const std::string COMMAND{
        fmt::format("ALTER TABLE {} RENAME TO {}", table, new_table)};
    status_ = sqlite3_exec(db_, COMMAND.c_str(), nullptr, nullptr, &err_msg_);
}

void options::delete_table(const std::string& table) {
    status_ = 0;

    const std::string COMMAND{fmt::format("DROP TABLE {}", table)};
    status_ = sqlite3_exec(db_, COMMAND.c_str(), nullptr, nullptr, &err_msg_);
}

void options::field_list(const std::string& table) {
    status_ = 0;
    list_fields_.clear();

    const std::string COMMAND{fmt::format("PRAGMA table_info({})", table)};

    status_ = sqlite3_exec(db_, COMMAND.c_str(), callback_vector_map,
                           static_cast<void*>(&list_fields_), &err_msg_);
}

void options::insert_data(const std::string& table,
                          const std::string& insert_fields,
                          const std::string& insert_data) {
    status_ = 0;
    list_fields_.clear();

    const std::string COMMAND{fmt::format("INSERT INTO {} ({}) VALUES ({})",
                                          table, insert_fields, insert_data)};
    status_ = sqlite3_exec(db_, COMMAND.c_str(), nullptr, nullptr, &err_msg_);
}

void options::update_data(const std::string& table,
                          const std::string& field,
                          const std::string& new_data,
                          const std::string& args) {
    status_ = 0;
    list_fields_.clear();
    list_data_.clear();

    const std::string COMMAND{fmt::format("UPDATE {} SET {}={} WHERE {}", table,
                                          field, new_data, args)};
    status_ = sqlite3_exec(db_, COMMAND.c_str(), nullptr, nullptr, &err_msg_);
}

void options::delete_data(const std::string& table, const std::string& args) {
    status_ = 0;
    list_tables_.clear();
    list_data_.clear();

    const std::string COMMAND{
        fmt::format("DELETE FROM {} WHERE {}", table, args)};
    status_ = sqlite3_exec(db_, COMMAND.c_str(), nullptr, nullptr, &err_msg_);
}

void options::list_data(const std::string& table) {
    status_ = 0;
    list_data_.clear();

    const std::string COMMAND{fmt::format("SELECT * FROM {}", table)};
    status_ = sqlite3_exec(db_, COMMAND.c_str(), callback_vector_map,
                           static_cast<void*>(&list_data_), &err_msg_);
}

void options::add_field(bool create_table,
                        const std::string& table,
                        const std::string& field_name,
                        const std::string& field_type,
                        choice_variant notnull,
                        choice_variant pk,
                        choice_variant ai) {
    status_ = 0;
    if (ai == choice_variant::YES && notnull == choice_variant::YES &&
        create_table) {
        const std::string COMMAND{fmt::format(
            "CREATE TABLE {}({} {} NOT NULL PRIMARY KEY AUTOINCREMENT)", table,
            field_name, field_type)};

        status_ =
            sqlite3_exec(db_, COMMAND.c_str(), nullptr, nullptr, &err_msg_);

    } else if (ai == choice_variant::YES && create_table) {
        const std::string COMMAND{
            fmt::format("CREATE TABLE {}({} {} PRIMARY KEY AUTOINCREMENT)",
                        table, field_name, field_type)};
        status_ =
            sqlite3_exec(db_, COMMAND.c_str(), nullptr, nullptr, &err_msg_);

    } else if (notnull == choice_variant::YES && pk == choice_variant::YES &&
               create_table) {
        const std::string COMMAND{
            fmt::format("CREATE TABLE {}({} {} NOT NULL PRIMARY KEY)", table,
                        field_name, field_type)};
        status_ =
            sqlite3_exec(db_, COMMAND.c_str(), nullptr, nullptr, &err_msg_);
    } else if (pk == choice_variant::YES && create_table) {
        const std::string COMMAND{
            fmt::format("CREATE TABLE {}({} {} PRIMARY KEY)", table, field_name,
                        field_type)};
        status_ =
            sqlite3_exec(db_, COMMAND.c_str(), nullptr, nullptr, &err_msg_);
    } else if (notnull == choice_variant::YES) {
        std::string COMMAND;

        if (create_table) {
            COMMAND = fmt::format("CREATE TABLE {}({} {} NOT NULL)", table,
                                  field_name, field_type);
        } else {
            COMMAND = fmt::format("ALTER TABLE {} ADD {} {} NOT NULL", table,
                                  field_name, field_type);
        }
        status_ =
            sqlite3_exec(db_, COMMAND.c_str(), nullptr, nullptr, &err_msg_);
    } else {
        std::string COMMAND;

        if (create_table) {
            COMMAND = fmt::format("CREATE TABLE {}({} {})", table, field_name,
                                  field_type);
        } else {
            COMMAND = fmt::format("ALTER TABLE {} ADD {} {}", table, field_name,
                                  field_type);
        }

        status_ =
            sqlite3_exec(db_, COMMAND.c_str(), nullptr, nullptr, &err_msg_);
    }
}

void options::delete_field(const std::string& table, const std::string& field) {
    status_ = 0;

    const std::string COMMAND{
        fmt::format("ALTER TABLE {} DROP COLUMN {}", table, field)};
    status_ = sqlite3_exec(db_, COMMAND.c_str(), nullptr, nullptr, &err_msg_);
}

void options::cli_command(const std::string& cmd) {
    status_ = 0;
    list_cli_info_.clear();

    status_ = sqlite3_exec(db_, cmd.c_str(), callback_vector_map,
                           static_cast<void*>(&list_cli_info_), &err_msg_);
}

bool options::status_check() const noexcept {
    return status_ == SQLITE_OK;
}

sqlite3* options::get_db() const noexcept {
    return db_;
}

const std::vector<std::string>& options::get_list_tables() const noexcept {
    return list_tables_;
}

const std::vector<std::map<std::string, std::string>>& options::get_list_data()
    const noexcept {
    return list_data_;
}

const std::vector<std::map<std::string, std::string>>&
options::get_list_fields() const noexcept {
    return list_fields_;
}

const std::vector<std::map<std::string, std::string>>&
options::get_list_cli_info() const noexcept {
    return list_cli_info_;
}

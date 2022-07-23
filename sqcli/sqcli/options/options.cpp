#include "options.hpp"
#include "fmt/core.h"
#include "sqcli/sqcli.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <utility>
#include <vector>

namespace {
int callback_default([[maybe_unused]] void* data,
                     int argc,
                     char** argv,
                     char** field_name) {
    for (int i{0}; i < argc; i++) {
        fmt::print(" {}: {}", field_name[i], argv[i] ? argv[i] : "NULL");
    }
    std::cout << '\n';

    return 0;
}

int callback_vector(void* data, int argc, char** argv, char** field_name) {
    for (int i{0}; i < argc; i++) {
        static_cast<std::vector<std::string>*>(data)->emplace_back(
            argv[i] ? argv[i] : "NULL");
        fmt::print(" {}: {}", field_name[i], argv[i] ? argv[i] : "NULL");
    }
    std::cout << '\n';

    return 0;
}

int callback_vector_map(void* data, int argc, char** argv, char** field_name) {
    auto obj{
        static_cast<std::vector<std::map<std::string, std::string>>*>(data)};
    obj->emplace_back();

    for (int i{0}; i < argc; i++) {
        obj->at(obj->size() - 1)
            .emplace(field_name[i], argv[i] ? argv[i] : "NULL");
        fmt::print(" {}: {}", field_name[i], argv[i] ? argv[i] : "NULL");
    }
    std::cout << '\n';

    return 0;
}

void clrscr() {
    std::cout << "\033[2J\033[1;1H";
}

void refresh_stdin() {
    std::cin.clear();
    std::cin.ignore();
    // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
}  // namespace

options::options(sqcli& sqcli) : sqcli_(sqcli), menu_it_(0) {}

void options::menu() {
    if (menu_it_ == 0) {
        clrscr();
    }

    fmt::print(
        "Выберите действие которое хотите выполнить над '{}'\n\n 1)\tВыбрать "
        "таблицу\n 2)\tСоздать "
        "таблицу\n 3)\tПереименовать таблицу\n"
        " 4)\tДобавить "
        "поле в таблицу\n 5)\tПосмотреть созданные таблицы\n 6)\tПосмотреть "
        "поля в таблице\n 7)\tВставить "
        "данные в "
        "таблицу\n 8)\tПосмотреть данные из таблицы\n 9)\tОбновить данные в "
        "таблице\n 10)\tУдалить данные из таблицы\n 11)\tУдалить таблицу\n "
        "12)\tУдалить поле из таблицы\n "
        "13)\tВыполнить SQL команду вручную\n "
        "14)\tЗавершить работу "
        "с базой данных\n\nВыбранная таблица: '{}'\n\n: ",
        sqcli_.path_db_, name_table_ == "" ? "NONE" : name_table_);
    std::cin >> menu_it_;

    if (std::cin.fail()) {
        if (std::cin.eof()) {
            menu_it_ = 14;
        } else {
            refresh_stdin();
            clrscr();

            std::cout << "SQCli error: unknown method selected\n\n";
            return;
        }
    } else if (menu_it_ == 1) {
        select_table();
    } else if (menu_it_ == 2) {
        create_table();
    } else if (menu_it_ == 3) {
        rename_table();
    } else if (menu_it_ == 4) {
        add_field();
    } else if (menu_it_ == 5) {
        table_list();
    } else if (menu_it_ == 6) {
        field_list();
    } else if (menu_it_ == 7) {
        insert_data();
    } else if (menu_it_ == 8) {
        list_data();
    } else if (menu_it_ == 9) {
        update_data();
    } else if (menu_it_ == 10) {
        delete_data();
    } else if (menu_it_ == 11) {
        delete_table();
    } else if (menu_it_ == 12) {
        delete_field();
    } else if (menu_it_ == 13) {
        cli_command();
    } else if (menu_it_ == 14) {
        clrscr();
        std::cout << "SQCli: exit successfully\n";
        return;
    } else {
        clrscr();
        std::cout << "SQCli error: unknown method selected\n\n";
        return;
    }
}

void options::create_table() {
    clrscr();

    std::cout << "Введите название таблицы\n\n: ";
    std::cin >> name_table_;
    std::cout << '\n';

    std::cout << "Нужно добавить минимум 1 поле, добавить поле 'ID' с "
                 "AUTOINCREMENT?\n1) Да\n2) Нет\n\n: ";
    std::cin >> check_default_id;
    std::cout << '\n';

    if (check_default_id == 1) {
        const std::string COMMAND{fmt::format(
            "CREATE TABLE {}(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT)",
            name_table_)};
        sqcli_.status_ = sqlite3_exec(sqcli_.db_, COMMAND.c_str(), nullptr,
                                      nullptr, &sqcli_.err_msg_);

        clrscr();
        status_check();
    } else if (check_default_id == 2) {
        add_field(true);
    } else {
        name_table_.clear();

        if (std::cin.fail()) {
            refresh_stdin();
        }

        clrscr();
        std::cout << "SQCli error: unknown choice selected\n\n";
        return;
    }
}

void options::select_table() {
    clrscr();
    table_list();

    if (list_tables_.empty()) {
        return;
    }

    std::string name_table;
    std::cout << "Введите название таблицы\n\n: ";
    std::cin >> name_table;

    if (auto it{std::ranges::find(list_tables_, name_table)};
        it != list_tables_.end()) {
        name_table_ = name_table;

        clrscr();
        std::cout << "SQCli: successfully\n\n";
    } else {
        clrscr();
        std::cout << "SQCli error: unknown choice selected\n\n";
    }
}

void options::rename_table() {
    if (check_selected_table()) {
        std::string new_table_name;

        clrscr();
        fmt::print(
            "Введите название которое хотите поменять у таблицы '{}'\n\n: ",
            name_table_);
        std::cin >> new_table_name;

        const std::string COMMAND{fmt::format("ALTER TABLE {} RENAME TO {}",
                                              name_table_, new_table_name)};
        sqcli_.status_ = sqlite3_exec(sqcli_.db_, COMMAND.c_str(), nullptr,
                                      nullptr, &sqcli_.err_msg_);
        name_table_ = new_table_name;

        clrscr();
        status_check();
    }
}

void options::delete_table() {
    if (check_selected_table()) {
        const std::string COMMAND{fmt::format("DROP TABLE {}", name_table_)};
        sqcli_.status_ = sqlite3_exec(sqcli_.db_, COMMAND.c_str(), nullptr,
                                      nullptr, &sqcli_.err_msg_);

        name_table_.clear();

        clrscr();
        status_check();
    }
}

void options::insert_data() {
    if (check_selected_table()) {
        std::string insert_fields;
        std::string insert_data;

        clrscr();
        field_list();

        for (std::size_t i{0}; i != list_fields_.size(); ++i) {
            std::string tmp;

            if (list_fields_.at(i).at("pk") == "1" &&
                list_fields_.at(i).at("type") == "INTEGER") {
                // pizdec...
                // Although the SQLite documentation seems to imply that the
                // sqlite_sequence table is populated when the table is created
                // in fact this is not the case and the count only becomes
                // available after data is inserted.
            } else if (list_fields_.at(i).at("notnull") == "1" ||
                       list_fields_.at(i).at("pk") == "1") {
                insert_fields.append(list_fields_.at(i).at("name") + ",");

                fmt::print(
                    "Введите данные для поле '{}' ({}) (Обязательное)\n\n: ",
                    list_fields_.at(i).at("name"),
                    list_fields_.at(i).at("type"));
                std::cin >> tmp;
                std::cout << '\n';

                insert_data.append(tmp + ",");
            } else {
                int it{0};

                fmt::print(
                    "Заполнить поле '{}' ? ({}) (Необязательное)\n1) Да\n2) "
                    "Нет\n\n: ",
                    list_fields_.at(i).at("name"),
                    list_fields_.at(i).at("type"));
                std::cin >> it;
                std::cout << '\n';

                if (it == 1) {
                    insert_fields.append(list_fields_.at(i).at("name") + ",");

                    std::cout << ": ";
                    std::cin >> tmp;
                    std::cout << '\n';

                    insert_data.append(tmp + ",");
                } else if (it == 2) {
                } else {
                    if (std::cin.fail()) {
                        refresh_stdin();
                    }

                    clrscr();
                    std::cout << "SQCli error: unknown choice selected\n\n";
                    return;
                }
            }
        }

        insert_fields.erase(insert_fields.size() - 1, 1);
        insert_data.erase(insert_data.size() - 1, 1);

        const std::string COMMAND{fmt::format("INSERT INTO {} ({}) VALUES ({})",
                                              name_table_, insert_fields,
                                              insert_data)};
        sqcli_.status_ = sqlite3_exec(sqcli_.db_, COMMAND.c_str(), nullptr,
                                      nullptr, &sqcli_.err_msg_);

        clrscr();
        status_check();
    }
}

void options::update_data() {
    if (check_selected_table()) {
        std::string field;
        int index;

        clrscr();
        field_list(false);
        list_data();

        std::cout << "Введите поле в котором хотите изменить данные\n\n: ";
        std::cin >> field;
        std::cout << '\n';

        if (check_field(field, &index)) {
            std::string new_data;
            std::string args;

            fmt::print("Введите новое значение для поле '{}' ({})\n\n: ", field,
                       list_fields_.at(index).at("type"));
            std::cin >> new_data;
            std::cout << '\n';

            std::cout << "Введите условие для применения изменений\n\n: ";
            std::cin >> args;
            std::cout << '\n';

            const std::string COMMAND{
                fmt::format("UPDATE {} SET {}={} WHERE {}", name_table_, field,
                            new_data, args)};
            sqcli_.status_ = sqlite3_exec(sqcli_.db_, COMMAND.c_str(), nullptr,
                                          nullptr, &sqcli_.err_msg_);

            clrscr();
            status_check();
        } else {
            clrscr();
            std::cout << "SQCli error: unknown field selected\n\n";
            return;
        }
    }
}

void options::delete_data() {
    if (check_selected_table()) {
        std::string args;

        field_list();
        list_data(false);

        if (list_data_.empty()) {
            clrscr();
            std::cout << "SQCli error: not found data\n\n";
            return;
        }

        std::cout << "Введите условие для удаления данных\n\n: ";
        std::cin >> args;
        std::cout << '\n';

        const std::string COMMAND{
            fmt::format("DELETE FROM {} WHERE {}", name_table_, args)};
        sqcli_.status_ = sqlite3_exec(sqcli_.db_, COMMAND.c_str(), nullptr,
                                      nullptr, &sqcli_.err_msg_);

        clrscr();
        status_check();
    }
}

void options::list_data(const bool clear_screen) {
    if (check_selected_table()) {
        if (clear_screen) {
            clrscr();
        }

        list_data_.clear();

        fmt::print("Данные из таблицы: '{}'\n\n", name_table_);

        const std::string COMMAND{fmt::format("SELECT * FROM {}", name_table_)};
        sqcli_.status_ =
            sqlite3_exec(sqcli_.db_, COMMAND.c_str(), callback_vector,
                         static_cast<void*>(&list_data_), &sqcli_.err_msg_);

        if (list_data_.empty()) {
            std::cout << "\tNONE\n";
        }

        std::cout << '\n';
        status_check();
    }
}

void options::add_field(const bool create_table) {
    if (!create_table) {
        clrscr();
    }

    std::cout << "Введите название поле\n\n: ";
    std::cin >> field_name_;
    std::cout << '\n';

    std::cout << "Введите тип поле\n\n: ";
    std::cin >> field_type_;
    std::cout << '\n';

    std::cout << "Сделать его NOT NULL?\n1) Да 2) Нет\n\n: ";
    std::cin >> check_null_;
    std::cout << '\n';

    if (check_null_ != 1 && check_null_ != 2) {
        if (std::cin.fail()) {
            refresh_stdin();
        }

        clrscr();
        std::cout << "SQCli error: unknown choice selected\n\n";
        return;
    }

    if (create_table) {
        std::cout << "Сделать его PRIMARY KEY?\n1) Да 2) Нет\n\n: ";
        std::cin >> check_primary_key_;
        std::cout << '\n';

        if (check_primary_key_ != 1 && check_primary_key_ != 2) {
            if (std::cin.fail()) {
                refresh_stdin();
            }

            clrscr();
            std::cout << "SQCli error: unknown choice selected\n\n";
            return;
        }

        if (check_primary_key_ == 1 &&
            (field_type_ == "INTEGER" || field_type_ == "integer")) {
            std::cout << "Сделать его AUTOINCREMENT?\n1) Да 2) Нет\n\n: ";
            std::cin >> check_autoincrement_;
            std::cout << '\n';

            if (check_autoincrement_ != 1 && check_autoincrement_ != 2) {
                if (std::cin.fail()) {
                    refresh_stdin();
                }

                clrscr();
                std::cout << "SQCli error: unknown choice selected\n\n";
                return;
            }
        }
    }
    if (check_autoincrement_ == 1 && check_null_ == 1 && create_table) {
        std::string COMMAND;

        if (create_table) {
            COMMAND = fmt::format(
                "CREATE TABLE {}({} {} NOT NULL PRIMARY KEY AUTOINCREMENT)",
                name_table_, field_name_, field_type_);
        } else {
            COMMAND = fmt::format(
                "ALTER TABLE {} ADD {} {} NOT NULL PRIMARY KEY "
                "AUTOINCREMENT",
                name_table_, field_name_, field_type_);
        }

        sqcli_.status_ = sqlite3_exec(sqcli_.db_, COMMAND.c_str(), nullptr,
                                      nullptr, &sqcli_.err_msg_);

        clrscr();
        status_check();
    } else if (check_autoincrement_ == 1 && create_table) {
        std::string COMMAND;

        if (create_table) {
            COMMAND =
                fmt::format("CREATE TABLE {}({} {} PRIMARY KEY AUTOINCREMENT)",
                            name_table_, field_name_, field_type_);
        } else {
            COMMAND = fmt::format(
                "ALTER TABLE {} ADD {} {} PRIMARY KEY AUTOINCREMENT",
                name_table_, field_name_, field_type_);
        }

        sqcli_.status_ = sqlite3_exec(sqcli_.db_, COMMAND.c_str(), nullptr,
                                      nullptr, &sqcli_.err_msg_);

        clrscr();
        status_check();

    } else if (check_null_ == 1 && check_primary_key_ == 1 && create_table) {
        std::string COMMAND;

        if (create_table) {
            COMMAND = fmt::format("CREATE TABLE {}({} {} NOT NULL PRIMARY KEY)",
                                  name_table_, field_name_, field_type_);
        } else {
            COMMAND =
                fmt::format("ALTER TABLE {} ADD {} {} NOT NULL PRIMARY KEY",
                            name_table_, field_name_, field_type_);
        }

        sqcli_.status_ = sqlite3_exec(sqcli_.db_, COMMAND.c_str(), nullptr,
                                      nullptr, &sqcli_.err_msg_);

        clrscr();
        status_check();
    } else if (check_primary_key_ == 1 && create_table) {
        std::string COMMAND;

        if (create_table) {
            COMMAND = fmt::format("CREATE TABLE {}({} {} PRIMARY KEY)",
                                  name_table_, field_name_, field_type_);
        } else {
            COMMAND = fmt::format("ALTER TABLE {} ADD {} {} PRIMARY KEY",
                                  name_table_, field_name_, field_type_);
        }

        sqcli_.status_ = sqlite3_exec(sqcli_.db_, COMMAND.c_str(), nullptr,
                                      nullptr, &sqcli_.err_msg_);

        clrscr();
        status_check();
    } else if (check_null_ == 1) {
        std::string COMMAND;

        if (create_table) {
            COMMAND = fmt::format("CREATE TABLE {}({} {} NOT NULL)",
                                  name_table_, field_name_, field_type_);

        } else {
            COMMAND = fmt::format("ALTER TABLE {} ADD {} {} NOT NULL",
                                  name_table_, field_name_, field_type_);
        }

        sqcli_.status_ = sqlite3_exec(sqcli_.db_, COMMAND.c_str(), nullptr,
                                      nullptr, &sqcli_.err_msg_);

        clrscr();
        status_check();
    } else {
        std::string COMMAND;

        if (create_table) {
            COMMAND = fmt::format("CREATE TABLE {}({} {})", name_table_,
                                  field_name_, field_type_);
        } else {
            COMMAND = fmt::format("ALTER TABLE {} ADD {} {}", name_table_,
                                  field_name_, field_type_);
        }

        sqcli_.status_ = sqlite3_exec(sqcli_.db_, COMMAND.c_str(), nullptr,
                                      nullptr, &sqcli_.err_msg_);

        clrscr();
        status_check();
    }
}

void options::delete_field() {
    if (check_selected_table()) {
        std::string field;

        clrscr();
        field_list();

        std::cout << "Введите название поле для удаления\n\n: ";
        std::cin >> field;
        std::cout << '\n';

        if (check_field(field)) {
            const std::string COMMAND{fmt::format(
                "ALTER TABLE {} DROP COLUMN {}", name_table_, field)};
            sqcli_.status_ = sqlite3_exec(sqcli_.db_, COMMAND.c_str(), nullptr,
                                          nullptr, &sqcli_.err_msg_);
            clrscr();
            status_check();
        } else {
            clrscr();
            std::cout << "SQCli error: unknown name selected\n\n";
            return;
        }
    }
}

void options::table_list(const bool clear_screen) {
    if (clear_screen) {
        clrscr();
    }

    list_tables_.clear();

    const char* COMMAND{
        "SELECT name FROM sqlite_schema WHERE type='table' ORDER BY name;"};
    std::cout << "Таблицы созданные в базе данных:\n\n";

    sqcli_.status_ =
        sqlite3_exec(sqcli_.db_, COMMAND, callback_vector,
                     static_cast<void*>(&list_tables_), &sqcli_.err_msg_);

    if (list_tables_.empty()) {
        std::cout << "\tNONE\n";
    }

    std::cout << '\n';
    status_check();
}

void options::field_list(const bool clear_screen) {
    if (clear_screen) {
        clrscr();
    }

    list_fields_.clear();

    if (check_selected_table()) {
        fmt::print("Поля созданные в таблице: '{}'\n\n", name_table_);

        const std::string COMMAND{
            fmt::format("PRAGMA table_info({})", name_table_)};
        sqcli_.status_ =
            sqlite3_exec(sqcli_.db_, COMMAND.c_str(), callback_vector_map,
                         static_cast<void*>(&list_fields_), &sqcli_.err_msg_);

        std::cout << "\n";
        status_check();
    }
}

void options::cli_command() {
    std::string COMMAND;

    clrscr();
    std::cout << "Введите команду для выполнения\n\n: ";

    std::getline(std::cin >> std::ws, COMMAND);
    clrscr();

    sqcli_.status_ = sqlite3_exec(sqcli_.db_, COMMAND.c_str(), callback_default,
                                  nullptr, &sqcli_.err_msg_);

    status_check();
}

bool options::status_check(const bool open_database) const {
    if (sqcli_.status_ != SQLITE_OK) {
        std::cout << "SQL error: " << sqlite3_errmsg(sqcli_.db_) << "\n\n";
        return false;
    } else if (open_database) {
        std::cout << "SQL: open database successfully\n\n";
        return true;
    } else {
        std::cout << "SQL: successfully\n\n";
        return true;
    }
}

bool options::check_selected_table() const {
    if (name_table_.empty()) {
        clrscr();
        std::cout << "SQCli error: table not select\n\n";

        return false;
    }
    return true;
}

bool options::check_field(const std::string& field, int* index) const {
    for (std::size_t i{0}; i != list_fields_.size(); i++) {
        if (list_fields_.at(i).at("name") == field) {
            if (index != nullptr) {
                *index = i;
            }
            return true;
        }
    }
    return false;
}

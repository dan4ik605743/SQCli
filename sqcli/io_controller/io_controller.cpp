#include "io_controller/io_controller.hpp"
#include "fmt/core.h"

#include <exception>
#include <iostream>

namespace {
void clrscr() {
    std::cout << "\033[2J\033[1;1H";
}

void refresh_stdin() {
    std::cin.clear();
    std::cin.ignore();
    // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
}  // namespace

void io_controller::menu() {
    if (path_db_.empty()) {
        clrscr();

        if (status_check_process(true)) {
            on_start_(path_db_);
        } else {
            on_exit_();
            return;
        }
    }

    fmt::print(
        "Выберите действие которое хотите выполнить над '{}'\n\n "
        "1)\tВыбрать "
        "таблицу\n 2)\tСоздать "
        "таблицу\n 3)\tПереименовать таблицу\n"
        " 4)\tДобавить "
        "поле в таблицу\n 5)\tПосмотреть созданные таблицы\n "
        "6)\tПосмотреть "
        "поля в таблице\n 7)\tВставить "
        "данные в "
        "таблицу\n 8)\tПосмотреть данные из таблицы\n 9)\tОбновить данные "
        "в "
        "таблице\n 10)\tУдалить данные из таблицы\n 11)\tУдалить таблицу\n "
        "12)\tУдалить поле из таблицы\n "
        "13)\tВыполнить SQL команду вручную\n "
        "14)\tЗавершить работу "
        "с базой данных\n\nВыбранная таблица: '{}'\n\n: ",
        path_db_, name_table_.empty() ? "NONE" : name_table_);

    if (std::cin.eof()) {
        clrscr();
        std::cout << "SQCli: exit successfully\n";

        on_exit_();
        return;
    }

    try {
        if (std::cin.fail()) {
            refresh_stdin();
            std::cin >> menu_it_;
            insert_value_to_enum(menu_it_, menu_options_);
        } else {
            std::cin >> menu_it_;
            insert_value_to_enum(menu_it_, menu_options_);
        }
    } catch (const std::exception& ex) {
        clrscr();
        std::cout << ex.what() << '\n';
        return;
    }

    switch (menu_options_) {
        case menu_options::SELECT_TABLE:
            select_table_process();
            break;
        case menu_options::CREATE_TABLE:
            create_table_process();
            break;
        case menu_options::RENAME_TABLE:
            rename_table_process();
            break;
        case menu_options::ADD_FIELD:
            add_field_process();
            break;
        case menu_options::TABLE_LIST:
            table_list_process();
            break;
        case menu_options::FIELD_LIST:
            field_list_process();
            break;
        case menu_options::INSERT_DATA:
            insert_data_process();
            break;
        case menu_options::DATA_LIST:
            list_data_process();
            break;
        case menu_options::UPDATE_DATA:
            update_data_process();
            break;
        case menu_options::DELETE_DATA:
            delete_data_process();
            break;
        case menu_options::DELETE_TABLE:
            delete_table_process();
            break;
        case menu_options::DELETE_FIELD:
            delete_field_process();
            break;
        case menu_options::CLI_COMMAND:
            cli_command_process();
            break;
        case menu_options::ON_EXIT:
            clrscr();
            std::cout << "SQCli: exit successfully\n";
            on_exit_();
            break;
        default:
            clrscr();
            std::cout << "SQCli error: unknown method selected\n\n";
            break;
    }
}

void io_controller::cli_command_process() {
    std::string cmd;

    clrscr();
    std::cout << "Введите команду для выполнения\n\n: ";
    std::getline(std::cin >> std::ws, cmd);

    on_cli_command_(cmd);
    if (status_check_process()) {
        const auto& obj{on_get_cli_info_()};

        if (obj.empty()) {
            clrscr();
            std::cout << "Вывод команды:\n\n NONE\n";
            std::cout << '\n';
            status_check_process();
        } else {
            clrscr();
            std::cout << "Вывод команды:\n\n";
            for (std::size_t i{0}; i != obj.size(); ++i) {
                for (const auto& [field_name, argv] : obj.at(i)) {
                    fmt::print(" {}: {}", field_name, argv);
                }
                std::cout << '\n';
            }
            std::cout << '\n';
        }
    } else {
        clrscr();
        status_check_process();
    }
}

bool io_controller::status_check_process(bool open_database) {
    if (open_database && on_status_check_()) {
        std::cout << "SQL: open database successfully\n\n";
        return true;
    }

    if (on_status_check_()) {
        std::cout << "SQL: successfully\n\n";
        return true;
    }

    std::cout << "SQL error: " << sqlite3_errmsg(on_get_db_()) << "\n\n";
    return false;
}

void io_controller::field_list_process(bool clear_screen) {
    if (check_selected_table()) {
        if (clear_screen) {
            clrscr();
        }

        fmt::print("Поля созданные в таблице: '{}'\n\n", name_table_);

        on_field_list_(name_table_);
        status_check_process();

        const auto& obj{on_get_list_fields_()};

        for (std::size_t i{0}; i != obj.size(); ++i) {
            for (const auto& [field_name, argv] : obj.at(i)) {
                fmt::print(" {}: {}", field_name, argv);
            }
            std::cout << '\n';
        }
        std::cout << '\n';
    }
}

bool io_controller::table_list_process(bool clear_screen) {
    if (clear_screen) {
        clrscr();
    }

    std::cout << "Таблицы созданные в базе данных:\n\n";

    on_table_list_();
    const auto& obj{on_get_list_tables_()};

    if (obj.empty()) {
        std::cout << " NONE\n\n";
        return false;
    }

    for (std::size_t i{0}; i != obj.size(); ++i) {
        fmt::print(" name: {}\n", obj.at(i));
    }

    std::cout << '\n';
    status_check_process();

    return true;
}

void io_controller::rename_table_process() {
    if (check_selected_table()) {
        std::string new_table;

        clrscr();

        fmt::print(
            "Введите название которое хотите поменять у таблицы '{}'\n\n: ",
            name_table_);
        std::getline(std::cin >> std::ws, new_table);

        on_rename_table_(name_table_, new_table);

        if (status_check_process()) {
            clrscr();

            status_check_process();
            name_table_ = new_table;
        } else {
            clrscr();
            status_check_process();
        }
    }
}

void io_controller::delete_table_process() {
    if (check_selected_table()) {
        on_delete_table_(name_table_);
        name_table_.clear();

        clrscr();
        status_check_process();
    }
}

void io_controller::insert_data_process() {
    if (check_selected_table()) {
        std::string insert_fields;
        std::string insert_data;

        field_list_process();

        const auto& obj{on_get_list_fields_()};
        for (std::size_t i{0}; i != obj.size(); ++i) {
            std::string tmp;
            if (obj.at(i).at("pk") == "1" &&
                obj.at(i).at("type") == "INTEGER") {
                // pizdec...
                // Although the SQLite documentation seems to imply that the
                // sqlite_sequence table is populated when the table is created
                // in fact this is not the case and the count only becomes
                // available after data is inserted.
            } else if (obj.at(i).at("notnull") == "1" ||
                       obj.at(i).at("pk") == "1") {
                insert_fields.append(obj.at(i).at("name") + ",");

                fmt::print(
                    "Введите данные для поле '{}' ({}) (Обязательное)\n\n: ",
                    obj.at(i).at("name"), obj.at(i).at("type"));
                std::getline(std::cin >> std::ws, tmp);
                std::cout << '\n';

                insert_data.append(tmp + ",");
            } else {
                std::uint16_t it{0};

                fmt::print(
                    "Заполнить поле '{}' ? ({}) (Необязательное)\n1) Да\n2) "
                    "Нет\n\n: ",
                    obj.at(i).at("name"), obj.at(i).at("type"));

                std::cin >> it;

                try {
                    insert_value_to_enum(it, choice_variant_);
                } catch (const std::exception& ex) {
                    clrscr();
                    std::cout << ex.what() << '\n';
                    return;
                }

                std::cout << '\n';

                switch (choice_variant_) {
                    case choice_variant::YES:
                        insert_fields.append(obj.at(i).at("name") + ",");

                        std::cout << ": ";
                        std::cin >> tmp;
                        std::cout << '\n';

                        insert_data.append(tmp + ",");
                        break;
                    case choice_variant::NO:
                        break;
                    default:
                        return;
                }
            }
        }

        if (insert_fields.empty()) {
            clrscr();
            std::cout << "SQCli error: no field found\n\n";
            return;
        }

        insert_fields.erase(insert_fields.size() - 1, 1);
        insert_data.erase(insert_data.size() - 1, 1);

        on_insert_data_(name_table_, insert_fields, insert_data);

        clrscr();
        status_check_process();
    }
}

void io_controller::update_data_process() {
    if (check_selected_table()) {
        std::string field;
        std::size_t index{0};

        field_list_process();
        list_data_process(false);

        std::cout << "Введите поле в котором хотите изменить данные\n\n: ";
        std::cin >> field;
        std::cout << '\n';

        if (check_field(field, &index)) {
            std::string new_data;
            std::string args;

            const auto& obj{on_get_list_fields_()};

            fmt::print("Введите новое значение для поле '{}' ({})\n\n: ", field,
                       obj.at(index).at("type"));
            std::cin >> new_data;
            std::cout << '\n';

            std::cout << "Введите условие для применения изменений\n\n: ";
            std::cin >> args;
            std::cout << '\n';

            on_update_data_(name_table_, field, new_data, args);

            clrscr();
            status_check_process();
        } else {
            clrscr();
            std::cout << "SQCli error: unknown field selected\n\n";
        }
    }
}

void io_controller::delete_data_process() {
    if (check_selected_table() && list_data_process()) {
        std::string args;

        field_list_process(false);

        std::cout << "Введите условие для удаления данных\n\n: ";
        std::getline(std::cin >> std::ws, args);
        std::cout << '\n';

        on_delete_data_(name_table_, args);

        clrscr();
        status_check_process();
    }
}

void io_controller::add_field_process(bool create_table) {
    if (check_selected_table()) {
        std::uint16_t it{0};
        std::string field_name;
        std::string field_type;
        choice_variant notnull{choice_variant::NO};
        choice_variant pk{choice_variant::NO};
        choice_variant ai{choice_variant::NO};

        clrscr();

        std::cout << "Введите название поле\n\n: ";
        std::cin >> field_name;
        std::cout << '\n';

        std::cout << "Введите тип поле\n\n: ";
        std::cin >> field_type;
        std::cout << '\n';

        std::cout << "Сделать его NOT NULL?\n1) Да 2) Нет\n\n: ";

        std::cin >> it;
        try {
            insert_value_to_enum(it, notnull);
        } catch (const std::exception& ex) {
            clrscr();
            std::cout << ex.what() << '\n';
            return;
        }
        std::cout << '\n';

        if (create_table) {
            std::cout << "Сделать его PRIMARY KEY?\n1) Да 2) Нет\n\n: ";

            std::cin >> it;
            try {
                insert_value_to_enum(it, pk);
            } catch (const std::exception& ex) {
                clrscr();
                std::cout << ex.what() << '\n';
                return;
            }
            std::cout << '\n';

            if (pk == choice_variant::YES &&
                (field_type == "INTEGER" || field_type == "integer")) {
                std::cout << "Сделать его AUTOINCREMENT?\n1) Да 2) Нет\n\n: ";

                std::cin >> it;
                try {
                    insert_value_to_enum(it, ai);
                } catch (const std::exception& ex) {
                    clrscr();
                    std::cout << ex.what() << '\n';
                    return;
                }
                std::cout << '\n';
            }
        }

        on_add_field_(create_table, name_table_, field_name, field_type,
                      notnull, pk, ai);
        clrscr();
        status_check_process();
    }
}

void io_controller::delete_field_process() {
    if (check_selected_table()) {
        std::string field;

        clrscr();
        field_list_process();

        std::cout << "Введите название поле для удаления\n\n: ";
        std::cin >> field;
        std::cout << '\n';

        if (check_field(field)) {
            on_delete_field_(name_table_, field);
            clrscr();
            status_check_process();
        } else {
            clrscr();
            std::cout << "SQCli error: unknown field selected\n\n";
        }
    }
}

void io_controller::create_table_process() {
    std::uint16_t it{0};

    clrscr();

    std::cout << "Введите название таблицы\n\n: ";
    std::cin >> name_table_;
    std::cout << '\n';

    std::cout << "Нужно добавить минимум 1 поле, добавить поле 'ID' с "
                 "AUTOINCREMENT?\n1) Да\n2) Нет\n\n: ";

    std::cin >> it;
    try {
        insert_value_to_enum(it, choice_variant_);
    } catch (const std::exception& ex) {
        clrscr();

        name_table_.clear();
        std::cout << ex.what() << '\n';

        return;
    }
    std::cout << '\n';

    switch (choice_variant_) {
        case choice_variant::YES:
            on_create_table_(name_table_);
            clrscr();
            status_check_process();
            break;
        case choice_variant::NO:
            add_field_process(true);
            break;
        default:
            return;
    };
}

bool io_controller::list_data_process(bool clear_screen) {
    if (check_selected_table()) {
        if (clear_screen) {
            clrscr();
        }

        fmt::print("Данные из таблицы: '{}'\n\n", name_table_);

        on_list_data_(name_table_);
        status_check_process();

        const auto& obj{on_get_list_data_()};

        if (obj.empty()) {
            std::cout << " NONE\n\n";
            return false;
        }

        for (std::size_t i{0}; i != obj.size(); ++i) {
            for (const auto& [field_name, argv] : obj.at(i)) {
                fmt::print(" {}: {}", field_name, argv);
            }
            std::cout << '\n';
        }
        std::cout << '\n';
    }

    return true;
}

void io_controller::select_table_process() {
    if (table_list_process()) {
        std::string tmp;
        std::cout << "Введите название таблицы\n\n: ";
        std::cin >> tmp;

        if (on_select_table_(tmp)) {
            name_table_ = tmp;

            clrscr();
            std::cout << "SQCli: successfully\n\n";
        } else {
            clrscr();
            std::cout << "SQCli error: unknown choice selected\n\n";
        }
    }
}

bool io_controller::check_selected_table() const noexcept {
    if (name_table_.empty()) {
        clrscr();
        std::cout << "SQCli error: table not select\n\n";

        return false;
    }
    return true;
}

bool io_controller::check_field(const std::string& field,
                                std::size_t* index) const noexcept {
    const auto& obj{on_get_list_fields_()};

    for (std::size_t i{0}; i != obj.size(); ++i) {
        if (obj.at(i).at("name") == field) {
            if (index != nullptr) {
                *index = i;
            }
            return true;
        }
    }

    return false;
}

void io_controller::set_on_create_table(on_create_table on_create_table) {
    on_create_table_ = std::move(on_create_table);
}

void io_controller::set_on_delete_field(on_delete_field on_delete_field) {
    on_delete_field_ = std::move(on_delete_field);
}

void io_controller::set_on_add_field(on_add_field on_add_field) {
    on_add_field_ = std::move(on_add_field);
}

void io_controller::set_on_delete_data(on_delete_data on_delete_data) {
    on_delete_data_ = std::move(on_delete_data);
}

void io_controller::set_on_update_data(on_update_data on_update_data) {
    on_update_data_ = std::move(on_update_data);
}

void io_controller::set_on_insert_data(on_insert_data on_insert_data) {
    on_insert_data_ = std::move(on_insert_data);
}

void io_controller::set_on_delete_table(on_delete_table on_delete_table) {
    on_delete_table_ = std::move(on_delete_table);
}

void io_controller::set_on_rename_table(on_rename_table on_rename_table) {
    on_rename_table_ = std::move(on_rename_table);
}

void io_controller::set_on_list_data(on_list_data on_list_data) {
    on_list_data_ = std::move(on_list_data);
}

void io_controller::set_on_field_list(on_field_list on_field_list) {
    on_field_list_ = std::move(on_field_list);
}

void io_controller::set_on_cli_command(on_cli_command on_cli_command) {
    on_cli_command_ = std::move(on_cli_command);
}

void io_controller::set_on_get_cli_info(on_get_cli_info on_get_cli_info) {
    on_get_cli_info_ = std::move(on_get_cli_info);
}

void io_controller::set_on_get_list_tables(
    on_get_list_tables on_get_list_tables) {
    on_get_list_tables_ = std::move(on_get_list_tables);
}

void io_controller::set_on_get_list_data(on_get_list_data on_get_list_data) {
    on_get_list_data_ = std::move(on_get_list_data);
}

void io_controller::set_on_get_list_fields(
    on_get_list_fields on_get_list_fields) {
    on_get_list_fields_ = std::move(on_get_list_fields);
}

void io_controller::set_on_get_db(on_get_db on_get_db) {
    on_get_db_ = std::move(on_get_db);
}

void io_controller::set_on_table_list(on_table_list on_table_list) {
    on_table_list_ = std::move(on_table_list);
}

void io_controller::set_on_start(on_start on_start) {
    on_start_ = std::move(on_start);
}

void io_controller::set_on_exit(on_exit on_exit) {
    on_exit_ = std::move(on_exit);
}

void io_controller::set_on_status_check(on_status_check on_status_check) {
    on_status_check_ = std::move(on_status_check);
}

void io_controller::set_on_select_table(on_select_table on_select_table) {
    on_select_table_ = std::move(on_select_table);
}

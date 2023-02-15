#include "io_controller/io_controller.hpp"

#include <fmt/format.h>
#include <boost/process.hpp>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <exception>
#include <iostream>

namespace {
void clrscr() {
    boost::process::child cmd("clear");
    cmd.wait();
}

void refresh_stdin() {
    std::cin.clear();
    std::cin.ignore();
    // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
}  // namespace

namespace sqcli {
io_controller::io_controller() noexcept {
    const auto sqcli_log = spdlog::stdout_color_mt("SQCli");
    const auto sql_log = spdlog::stdout_color_mt("SQL");

    // disable eol
    sqcli_log->set_formatter(std::make_unique<spdlog::pattern_formatter>(
        "[%n] [%^%l%$] %v", spdlog::pattern_time_type::local, std::string("")));
    sql_log->set_formatter(std::make_unique<spdlog::pattern_formatter>(
        "[%n] [%^%l%$] %v", spdlog::pattern_time_type::local, std::string("")));
}

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

    if (std::cin.eof()) {
        clrscr();
        spdlog::get("SQCli")->info("Exit successfully\n\n");

        on_exit_();
        return;
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
        "12)\tПереименовать поле из таблицы\n "
        "13)\tВыполнить SQL команду вручную\n "
        "14)\tЗавершить работу "
        "с базой данных\n\n",
        path_db_);
    spdlog::get("SQCli")->info("Выбранная таблица: '{}'\n\n: ",
                               name_table_.empty() ? "NONE" : name_table_);

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
        spdlog::get("SQCli")->error("{}\n\n", ex.what());
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
        case menu_options::RENAME_FIELD:
            // delete_field deprecated and not worked with sqlite version > 3.25
            rename_field_process();
            break;
        case menu_options::CLI_COMMAND:
            cli_command_process();
            break;
        case menu_options::ON_EXIT:
            clrscr();
            spdlog::get("SQCli")->info("Exit successfully\n\n");
            on_exit_();
            break;
        default:
            break;
    }
}

void io_controller::cli_command_process() {
    std::string cmd;

    clrscr();
    fmt::print("Введите команду для выполнения\n\n: ");
    std::getline(std::cin >> std::ws, cmd);

    on_cli_command_(cmd);
    if (status_check_process()) {
        const auto& obj = on_get_cli_info_();

        if (obj.empty()) {
            clrscr();
            spdlog::get("SQCli")->info("Вывод команды:\n\n NONE\n\n");
            status_check_process();
        } else {
            clrscr();
            spdlog::get("SQCli")->info("Вывод команды:\n\n");
            const auto size_obj = obj.size();
            for (std::size_t i = 0; i != size_obj; ++i) {
                for (const auto& [field_name, argv] : obj.at(i)) {
                    fmt::print(" {}: {}", field_name, argv);
                }
                fmt::print("\n");
            }
            fmt::print("\n");
        }
    } else {
        clrscr();
        status_check_process();
    }
}

bool io_controller::status_check_process(bool open_database) {
    if (open_database && on_status_check_()) {
        spdlog::get("SQL")->info("Open database successfully\n\n");
        return true;
    }

    if (on_status_check_()) {
        spdlog::get("SQL")->info("Successfully\n\n");
        return true;
    }

    spdlog::get("SQL")->error("{}\n\n", sqlite3_errmsg(on_get_db_()));
    return false;
}

void io_controller::field_list_process(bool clear_screen) {
    if (check_selected_table()) {
        if (clear_screen) {
            clrscr();
        }

        spdlog::get("SQCli")->info("Поля созданные в таблице: '{}'\n\n",
                                   name_table_);

        on_field_list_(name_table_);
        status_check_process();

        const auto& obj = on_get_list_fields_();
        const auto size_obj = obj.size();

        for (std::size_t i = 0; i != size_obj; ++i) {
            for (const auto& [field_name, argv] : obj.at(i)) {
                fmt::print(" {}: {}", field_name, argv);
            }
            fmt::print("\n");
        }
        fmt::print("\n");
    }
}

bool io_controller::table_list_process(bool clear_screen) {
    if (clear_screen) {
        clrscr();
    }

    spdlog::get("SQCli")->info("Таблицы созданные в базе данных:\n\n");

    on_table_list_();
    const auto& obj = on_get_list_tables_();

    if (obj.empty()) {
        fmt::print(" NONE\n\n");
        return false;
    }

    const auto size_obj = obj.size();
    for (std::size_t i = 0; i != size_obj; ++i) {
        fmt::print(" name: {}\n", obj.at(i));
    }

    fmt::print("\n");
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

        const auto& obj = on_get_list_fields_();
        const auto size_obj = obj.size();
        for (std::size_t i = 0; i != size_obj; ++i) {
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
                fmt::print("\n");

                insert_data.append(tmp + ",");
            } else {
                std::uint16_t it = 0;

                fmt::print(
                    "Заполнить поле '{}' ? ({}) (Необязательное)\n1) Да\n2) "
                    "Нет\n\n: ",
                    obj.at(i).at("name"), obj.at(i).at("type"));

                std::cin >> it;

                try {
                    insert_value_to_enum(it, choice_variant_);
                } catch (const std::exception& ex) {
                    clrscr();
                    spdlog::get("SQCli")->error("{}\n\n", ex.what());
                    return;
                }

                fmt::print("\n");

                switch (choice_variant_) {
                    case choice_variant::YES:
                        insert_fields.append(obj.at(i).at("name") + ",");

                        fmt::print(": ");
                        std::cin >> tmp;
                        fmt::print("\n");

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
            spdlog::get("SQCli")->error("No field found\n\n");
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
        std::size_t index = 0;

        field_list_process();
        list_data_process(false);

        fmt::print("Введите поле в котором хотите изменить данные\n\n: ");
        std::cin >> field;
        fmt::print("\n");

        if (check_field(field, &index)) {
            std::string new_data;
            std::string args;

            const auto& obj = on_get_list_fields_();

            fmt::print("Введите новое значение для поле '{}' ({})\n\n: ", field,
                       obj.at(index).at("type"));
            std::cin >> new_data;
            fmt::print("\n");

            fmt::print("Введите условие для применения изменений\n\n: ");
            std::cin >> args;
            fmt::print("\n");

            on_update_data_(name_table_, field, new_data, args);

            clrscr();
            status_check_process();
        } else {
            clrscr();
            spdlog::get("SQCli")->error("Unknown field selected\n\n");
        }
    }
}

void io_controller::delete_data_process() {
    if (check_selected_table() && list_data_process()) {
        std::string args;

        field_list_process(false);

        fmt::print("Введите условие для удаления данных\n\n: ");
        std::getline(std::cin >> std::ws, args);
        fmt::print("\n");

        on_delete_data_(name_table_, args);

        clrscr();
        status_check_process();
    }
}

void io_controller::add_field_process(bool create_table) {
    if (check_selected_table()) {
        std::uint16_t it = 0;
        std::string field_name;
        std::string field_type;
        choice_variant notnull = choice_variant::NO;
        choice_variant pk = choice_variant::NO;
        choice_variant ai = choice_variant::NO;

        clrscr();

        fmt::print("Введите название поле\n\n: ");
        std::cin >> field_name;
        fmt::print("\n");

        fmt::print("Введите тип поле\n\n: ");
        std::cin >> field_type;
        fmt::print("\n");

        fmt::print("Сделать его NOT NULL?\n1) Да 2) Нет\n\n: ");

        std::cin >> it;
        try {
            insert_value_to_enum(it, notnull);
        } catch (const std::exception& ex) {
            clrscr();
            spdlog::get("SQCli")->error("{}\n\n", ex.what());
            return;
        }
        fmt::print("\n");

        if (create_table) {
            fmt::print("Сделать его PRIMARY KEY?\n1) Да 2) Нет\n\n: ");

            std::cin >> it;
            try {
                insert_value_to_enum(it, pk);
            } catch (const std::exception& ex) {
                clrscr();
                spdlog::get("SQCli")->error("{}\n\n", ex.what());
                return;
            }
            fmt::print("\n");

            if (pk == choice_variant::YES &&
                (field_type == "INTEGER" || field_type == "integer")) {
                fmt::print("Сделать его AUTOINCREMENT?\n1) Да 2) Нет\n\n: ");

                std::cin >> it;
                try {
                    insert_value_to_enum(it, ai);
                } catch (const std::exception& ex) {
                    clrscr();
                    spdlog::get("SQCli")->error("{}\n\n", ex.what());
                    return;
                }
                fmt::print("\n");
            }
        }

        on_add_field_(create_table, name_table_, field_name, field_type,
                      notnull, pk, ai);
        clrscr();
        status_check_process();
    }
}

void io_controller::rename_field_process() {
    if (check_selected_table()) {
        std::string field;
        std::string new_field;

        clrscr();
        field_list_process();

        fmt::print("Введите название поле для переименования\n\n: ");
        std::cin >> field;
        fmt::print("\n");

        fmt::print("Введите новое название для поля\n\n: ");
        std::cin >> new_field;
        fmt::print("\n");

        if (check_field(field)) {
            on_rename_field_(name_table_, field, new_field);
            clrscr();
            status_check_process();
        } else {
            clrscr();
            spdlog::get("SQCli")->error("Unknown field selected\n\n");
        }
    }
}

void io_controller::create_table_process() {
    std::uint16_t it = 0;

    clrscr();

    fmt::print("Введите название таблицы\n\n: ");
    std::cin >> name_table_;
    fmt::print("\n");

    fmt::print(
        "Нужно добавить минимум 1 поле, добавить поле 'ID' с "
        "AUTOINCREMENT?\n1) Да\n2) Нет\n\n: ");

    std::cin >> it;
    try {
        insert_value_to_enum(it, choice_variant_);
    } catch (const std::exception& ex) {
        clrscr();

        name_table_.clear();
        spdlog::get("SQCli")->error("{}\n\n", ex.what());

        return;
    }
    fmt::print("\n");

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

        spdlog::get("SQCli")->info("Данные из таблицы: '{}'\n\n", name_table_);

        on_list_data_(name_table_);
        status_check_process();

        const auto& obj = on_get_list_data_();

        if (obj.empty()) {
            fmt::print(" NONE\n\n");
            return false;
        }

        const auto size_obj = obj.size();
        for (std::size_t i = 0; i != size_obj; ++i) {
            for (const auto& [field_name, argv] : obj.at(i)) {
                fmt::print(" {}: {}", field_name, argv);
            }
            fmt::print("\n");
        }
        fmt::print("\n");
    }

    return true;
}

void io_controller::select_table_process() {
    if (table_list_process()) {
        std::string tmp;
        fmt::print("Введите название таблицы\n\n: ");
        std::cin >> tmp;

        if (on_select_table_(tmp)) {
            name_table_ = tmp;

            clrscr();
            spdlog::get("SQCli")->info("Successfully\n\n");
        } else {
            clrscr();
            spdlog::get("SQCli")->error("Unknown choice selected\n\n");
        }
    }
}

bool io_controller::check_selected_table() const noexcept {
    if (name_table_.empty()) {
        clrscr();
        spdlog::get("SQCli")->error("Table not select\n\n");

        return false;
    }
    return true;
}

bool io_controller::check_field(const std::string& field,
                                std::size_t* index) const noexcept {
    const auto& obj = on_get_list_fields_();
    const auto size_obj = obj.size();

    for (std::size_t i{0}; i != size_obj; ++i) {
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

void io_controller::set_on_rename_field(on_rename_field on_rename_field) {
    on_rename_field_ = std::move(on_rename_field);
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
}  // namespace sqcli
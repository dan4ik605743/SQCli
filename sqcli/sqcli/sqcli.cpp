#include "sqcli/sqcli.hpp"

sqcli::sqcli(const std::string& path_db) noexcept
    : options_{path_db}, exit_{false}, path_db_{path_db} {}

void sqcli::run() {
    io_ctrlr_.set_on_start([this](std::string& path_db) mutable {
        path_db = std::move(path_db_);
    });

    io_ctrlr_.set_on_status_check([this]() { return options_.status_check(); });

    io_ctrlr_.set_on_table_list([this]() { options_.table_list(); });

    io_ctrlr_.set_on_rename_table(
        [this](const std::string& table, const std::string& new_table) {
            options_.rename_table(table, new_table);
        });

    io_ctrlr_.set_on_delete_table(
        [this](const std::string& table) { options_.delete_table(table); });

    io_ctrlr_.set_on_insert_data([this](const std::string& table,
                                        const std::string& insert_fields,
                                        const std::string& insert_data) {
        options_.insert_data(table, insert_fields, insert_data);
    });

    io_ctrlr_.set_on_update_data(
        [this](const std::string& table, const std::string& field,
               const std::string& new_data, const std::string& args) {
            options_.update_data(table, field, new_data, args);
        });

    io_ctrlr_.set_on_delete_data(
        [this](const std::string& table, const std::string& args) {
            options_.delete_data(table, args);
        });

    io_ctrlr_.set_on_add_field(
        [this](bool create_table, const std::string& table,
               const std::string& field_name, const std::string& field_type,
               choice_variant notnull, choice_variant pk, choice_variant ai) {
            options_.add_field(create_table, table, field_name, field_type,
                               notnull, pk, ai);
        });

    io_ctrlr_.set_on_rename_field([this](const std::string& table,
                                         const std::string& field,
                                         const std::string& new_field) {
        options_.rename_field(table, field, new_field);
    });

    io_ctrlr_.set_on_create_table(
        [this](const std::string& table) { options_.create_table(table); });

    io_ctrlr_.set_on_field_list(
        [this](const std::string& table) { options_.field_list(table); });

    io_ctrlr_.set_on_list_data(
        [this](const std::string& table) { options_.list_data(table); });

    io_ctrlr_.set_on_select_table([this](const std::string& name_table) {
        return options_.select_table(name_table);
    });

    io_ctrlr_.set_on_cli_command(
        [this](const std::string& cmd) { options_.cli_command(cmd); });

    io_ctrlr_.set_on_exit([this]() { exit_ = true; });

    io_ctrlr_.set_on_get_db([this]() { return options_.get_db(); });

    io_ctrlr_.set_on_get_list_tables(
        [this]() { return std::cref(options_.get_list_tables()); });

    io_ctrlr_.set_on_get_list_data(
        [this]() { return std::cref(options_.get_list_data()); });

    io_ctrlr_.set_on_get_list_fields(
        [this]() { return std::cref(options_.get_list_fields()); });

    io_ctrlr_.set_on_get_cli_info(
        [this]() { return std::cref(options_.get_list_cli_info()); });

    while (!exit_) {
        io_ctrlr_.menu();
    }
}

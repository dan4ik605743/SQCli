#ifndef IO_CONTROLLER_HPP
#define IO_CONTROLLER_HPP

#include "enum.hpp"
#include "sqlite3.h"

#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>

class io_controller {
   public:
    using on_start = std::function<void(std::string& path_db)>;
    using on_exit = std::function<void()>;
    using on_status_check = std::function<bool()>;
    using on_table_list = std::function<void()>;
    using on_select_table = std::function<bool(const std::string& name_table)>;
    using on_cli_command = std::function<void(const std::string& cmd)>;
    using on_field_list = std::function<void(const std::string& table)>;
    using on_list_data = std::function<void(const std::string& table)>;
    using on_rename_table = std::function<void(const std::string& table,
                                               const std::string& new_table)>;
    using on_delete_table = std::function<void(const std::string& table)>;
    using on_insert_data = std::function<void(const std::string& table,
                                              const std::string& insert_fields,
                                              const std::string& insert_data)>;
    using on_update_data = std::function<void(const std::string& table,
                                              const std::string& field,
                                              const std::string& new_data,
                                              const std::string& args)>;
    using on_delete_data =
        std::function<void(const std::string& table, const std::string& args)>;
    using on_add_field = std::function<void(bool create_table,
                                            const std::string& table,
                                            const std::string& field_name,
                                            const std::string& field_type,
                                            choice_variant notnull,
                                            choice_variant pk,
                                            choice_variant ai)>;
    using on_delete_field =
        std::function<void(const std::string& table, const std::string& field)>;
    using on_create_table = std::function<void(const std::string& table)>;

    using on_get_db = std::function<sqlite3*()>;
    using on_get_list_tables = std::function<const std::vector<std::string>&()>;
    using on_get_list_data =
        std::function<const std::vector<std::map<std::string, std::string>>&()>;
    using on_get_list_fields =
        std::function<const std::vector<std::map<std::string, std::string>>&()>;
    using on_get_cli_info =
        std::function<const std::vector<std::map<std::string, std::string>>&()>;

    void menu();

    void set_on_start(on_start on_start);
    void set_on_exit(on_exit on_exit);
    void set_on_status_check(on_status_check on_status_check);
    void set_on_table_list(on_table_list on_table_list);
    void set_on_select_table(on_select_table on_select_table);
    void set_on_cli_command(on_cli_command on_cli_command);
    void set_on_field_list(on_field_list on_field_list);
    void set_on_list_data(on_list_data on_list_data);
    void set_on_rename_table(on_rename_table on_rename_table);
    void set_on_delete_table(on_delete_table on_delete_table);
    void set_on_insert_data(on_insert_data on_insert_data);
    void set_on_update_data(on_update_data on_update_data);
    void set_on_delete_data(on_delete_data on_delete_data);
    void set_on_add_field(on_add_field on_add_field);
    void set_on_delete_field(on_delete_field on_delete_field);
    void set_on_create_table(on_create_table on_create_table);

    void set_on_get_db(on_get_db on_get_db);
    void set_on_get_list_tables(on_get_list_tables on_get_list_tables);
    void set_on_get_list_data(on_get_list_data on_get_list_data);
    void set_on_get_list_fields(on_get_list_fields on_get_list_fields);
    void set_on_get_cli_info(on_get_cli_info on_get_cli_info);

   private:
    std::uint16_t menu_it_;
    std::string path_db_;
    std::string name_table_;
    bool select_table_;

    menu_options menu_options_;
    choice_variant choice_variant_;

    on_start on_start_;
    on_exit on_exit_;
    on_status_check on_status_check_;
    on_table_list on_table_list_;
    on_select_table on_select_table_;
    on_cli_command on_cli_command_;
    on_field_list on_field_list_;
    on_list_data on_list_data_;
    on_rename_table on_rename_table_;
    on_delete_table on_delete_table_;
    on_insert_data on_insert_data_;
    on_update_data on_update_data_;
    on_delete_data on_delete_data_;
    on_add_field on_add_field_;
    on_delete_field on_delete_field_;
    on_create_table on_create_table_;

    on_get_db on_get_db_;
    on_get_list_tables on_get_list_tables_;
    on_get_list_data on_get_list_data_;
    on_get_list_fields on_get_list_fields_;
    on_get_cli_info on_get_cli_info_;

    void select_table_process();
    void cli_command_process();
    void field_list_process(bool clear_screen = true);
    bool list_data_process(bool clear_screen = true);
    void rename_table_process();
    void delete_table_process();
    void insert_data_process();
    void update_data_process();
    void delete_data_process();
    void add_field_process(bool create_table = false);
    void delete_field_process();
    void create_table_process();
    [[nodiscard]] bool check_selected_table() const noexcept;
    [[nodiscard]] bool check_field(const std::string& field,
                                   std::size_t* index = nullptr) const noexcept;
    bool table_list_process(bool clear_screen = true);
    bool status_check_process(bool open_database = false);
};

#endif  // IO_CONTROLLER_HPP

#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include "enum.hpp"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <sqlite3.h>

class options {
   public:
    using data_list = std::vector<std::string>;
    using ptr_data_list = std::vector<std::string>*;
    using c_ref_data_list = const std::vector<std::string>&;

    using other_data = std::vector<std::map<std::string, std::string>>;
    using ptr_other_data = std::vector<std::map<std::string, std::string>>*;
    using c_ref_other_data =
        const std::vector<std::map<std::string, std::string>>&;

    explicit options(const std::string& path_db) noexcept;
    options(const options&) = delete;
    options(options&&) = delete;
    options& operator=(const options&) = delete;
    options& operator=(options&&) = delete;
    ~options() noexcept;

    void create_table(const std::string& table);
    bool select_table(const std::string& table);
    void rename_table(const std::string& table, const std::string& new_table);
    void delete_table(const std::string& table);

    void insert_data(const std::string& table,
                     const std::string& insert_fields,
                     const std::string& insert_data);
    void update_data(const std::string& table,
                     const std::string& field,
                     const std::string& new_data,
                     const std::string& args);
    void delete_data(const std::string& table, const std::string& args);

    void add_field(bool create_table,
                   const std::string& table,
                   const std::string& field_name,
                   const std::string& field_type,
                   choice_variant notnull,
                   choice_variant pk,
                   choice_variant ai);
    void rename_field(const std::string& table,
                      const std::string& field,
                      const std::string& new_field);

    void table_list();
    void field_list(const std::string& table);
    void list_data(const std::string& table);

    void cli_command(const std::string& cmd);

    [[nodiscard]] sqlite3* get_db() const noexcept;
    [[nodiscard]] c_ref_data_list get_list_tables() const noexcept;
    [[nodiscard]] c_ref_other_data get_list_data() const noexcept;
    [[nodiscard]] c_ref_other_data get_list_cli_info() const noexcept;
    [[nodiscard]] c_ref_other_data get_list_fields() const noexcept;

    [[nodiscard]] bool status_check() const noexcept;

   private:
    data_list list_tables_;
    other_data list_fields_;
    other_data list_data_;
    other_data list_cli_info_;

    sqlite3* db_ = nullptr;
    char* err_msg_ = nullptr;
    std::int32_t status_ = 0;
};

#endif  // OPTIONS_HPP

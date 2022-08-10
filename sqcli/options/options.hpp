#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include "enum.hpp"
#include "sqlite3.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

class options {
   public:
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
    void delete_field(const std::string& table, const std::string& field);

    void table_list();
    void field_list(const std::string& table);
    void list_data(const std::string& table);

    void cli_command(const std::string& cmd);

    [[nodiscard]] sqlite3* get_db() const noexcept;
    [[nodiscard]] const std::vector<std::string>& get_list_tables()
        const noexcept;
    [[nodiscard]] const std::vector<std::map<std::string, std::string>>&
    get_list_data() const noexcept;
    [[nodiscard]] const std::vector<std::map<std::string, std::string>>&
    get_list_cli_info() const noexcept;
    [[nodiscard]] const std::vector<std::map<std::string, std::string>>&
    get_list_fields() const noexcept;

    [[nodiscard]] bool status_check() const noexcept;
    [[nodiscard]] bool check_selected_table() const noexcept;
    [[nodiscard]] bool check_field(const std::string& field,
                                   std::size_t* index = nullptr) const noexcept;

   private:
    std::vector<std::string> list_tables_;
    std::vector<std::map<std::string, std::string>> list_fields_;
    std::vector<std::map<std::string, std::string>> list_data_;
    std::vector<std::map<std::string, std::string>> list_cli_info_;

    std::uint8_t check_default_id;
    std::uint8_t check_autoincrement_;
    std::uint8_t check_primary_key_;
    std::uint8_t check_null_;

    sqlite3* db_;
    char* err_msg_;
    std::int32_t status_;
};

#endif  // OPTIONS_HPP

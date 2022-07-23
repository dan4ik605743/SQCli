#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <map>
#include <string>
#include <vector>

class sqcli;

class options {
   public:
    explicit options(sqcli& sqcli);

    void menu();

    void create_table();
    void select_table();
    void rename_table();
    void delete_table();

    void insert_data();
    void update_data();
    void delete_data();

    void add_field(const bool create_table = false);
    void delete_field();

    void table_list(const bool clear_screen = true);
    void field_list(const bool clear_screen = true);
    void list_data(const bool clear_screen = true);

    void cli_command();

    bool status_check(const bool open_database = false) const;
    bool check_selected_table() const;
    bool check_field(const std::string& field, int* index = nullptr) const;

   private:
    friend class sqcli;

    sqcli& sqcli_;

    std::string name_table_;
    std::string field_name_;
    std::string field_type_;

    std::vector<std::string> list_tables_;
    std::vector<std::map<std::string, std::string>> list_fields_;
    std::vector<std::string> list_data_;

    int check_default_id;
    int check_autoincrement_;
    int check_primary_key_;
    int check_null_;
    int menu_it_;
};

#endif  // OPTIONS_HPP

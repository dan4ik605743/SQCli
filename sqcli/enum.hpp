#include <cstdint>
#include <stdexcept>

#ifndef ENUM_HPP
#define ENUM_HPP

enum class menu_options : std::uint8_t {
    MIN_VALUE = 0,
    SELECT_TABLE = 1,
    CREATE_TABLE = 2,
    RENAME_TABLE = 3,
    ADD_FIELD = 4,
    TABLE_LIST = 5,
    FIELD_LIST = 6,
    INSERT_DATA = 7,
    DATA_LIST = 8,
    UPDATE_DATA = 9,
    DELETE_DATA = 10,
    DELETE_TABLE = 11,
    RENAME_FIELD = 12,
    CLI_COMMAND = 13,
    ON_EXIT = 14,
    MAX_VALUE = 15
};

enum class choice_variant : std::uint8_t {
    MIN_VALUE = 0,
    YES = 1,
    NO = 2,
    MAX_VALUE = 3
};

template <class T>
inline void insert_value_to_enum(std::uint8_t value, T& enum_obj) {
    auto value_cast = static_cast<T>(value);

    if (value_cast > T::MIN_VALUE && value_cast < T::MAX_VALUE) {
        enum_obj = value_cast;
        return;
    }

    throw std::invalid_argument("Unknown choice selected");
}

#endif  // ENUM_HPP

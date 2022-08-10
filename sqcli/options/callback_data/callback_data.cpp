#include "options/callback_data/callback_data.hpp"

void callback_data::add_row() {
    data_.emplace_back();
}

void callback_data::clear() {
    data_.clear();
}

void callback_data::add_data(std::size_t row,
                             const std::string& first,
                             const std::string& second) {
    data_.at(row).emplace_back(first, second);
}

std::size_t callback_data::row_size() const noexcept {
    return data_.size();
}

std::size_t callback_data::col_size() const noexcept {
    if (data_.empty()) {
        return 0;
    }

    return data_.front().size();
}

std::vector<std::pair<std::string, std::string>>&
callback_data::front() noexcept {
    return data_.at(0);
}

std::vector<std::pair<std::string, std::string>>&
callback_data::back() noexcept {
    return data_.at(data_.size() - 1);
}

bool callback_data::find(std::size_t row,
                         std::size_t col,
                         const std::string& str) const noexcept {
    return (data_.at(row).at(col).first == str ||
            data_.at(row).at(col).second == str);
}

bool callback_data::find(std::size_t row,
                         std::size_t col,
                         const std::string& first_str,
                         const std::string& second_str) const noexcept {
    return (data_.at(row).at(col).first == first_str &&
            data_.at(row).at(col).second == second_str);
}

bool callback_data::empty() const noexcept {
    return data_.empty();
}

const std::pair<std::string, std::string>& callback_data::at(
    std::size_t row,
    std::size_t col) const noexcept {
    return data_.at(row).at(col);
}

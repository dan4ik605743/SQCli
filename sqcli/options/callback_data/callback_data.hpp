#ifndef CALLBACK_DATA
#define CALLBACK_DATA

#include <string>
#include <vector>

class callback_data {
   public:
    // Add row
    void add_row();

    // Removing all elements
    void clear();

    // Append data to row with pair<string, string>
    void add_data(std::size_t row,
                  const std::string& first,
                  const std::string& second);

    // Get a reference to the first element
    [[nodiscard]] std::vector<std::pair<std::string, std::string>>&
    front() noexcept;

    // Get a reference to the last element
    [[nodiscard]] std::vector<std::pair<std::string, std::string>>&
    back() noexcept;

    // Get a size to the row
    [[nodiscard]] std::size_t row_size() const noexcept;

    // Get a size to the col
    [[nodiscard]] std::size_t col_size() const noexcept;

    // Find matches in the first or second row of a series
    [[nodiscard]] bool find(std::size_t row,
                            std::size_t col,
                            const std::string& str) const noexcept;

    // Find matches in the first and second row of a series
    [[nodiscard]] bool find(std::size_t row,
                            std::size_t col,
                            const std::string& first_str,
                            const std::string& second_str) const noexcept;

    // Check if created elements are present
    [[nodiscard]] bool empty() const noexcept;

    // Get the reference to the pair
    [[nodiscard]] const std::pair<std::string, std::string>& at(
        std::size_t row,
        std::size_t col) const noexcept;

   private:
    std::vector<std::vector<std::pair<std::string, std::string>>> data_;
};

#endif  // CALLBACK_DATA

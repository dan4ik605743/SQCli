#ifndef SQCLI_HPP
#define SQCLI_HPP

#include "io_controller/io_controller.hpp"
#include "options/options.hpp"

#include <string>

namespace sqcli {
class sqcli {
   public:
    explicit sqcli(const std::string& path_db) noexcept;

    void run();

   private:
    io_controller io_ctrlr_;
    options options_;

    bool exit_;
    std::string path_db_;
};
}  // namespace sqcli
#endif  // SQCLI_HPP

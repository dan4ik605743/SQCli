#include <fmt/core.h>
#include <boost/program_options.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include "sqcli/sqcli.hpp"

int main(int argc, char** argv) {
    std::string db_path;

    try {
        boost::program_options::options_description desc("Allowed Options");
        boost::program_options::variables_map vm;

        desc.add_options()("help,h", "Get allowed options")(
            "database,d",
            boost::program_options::value<std::string>()->required(),
            "Set path to sqlite database file");

        boost::program_options::store(
            boost::program_options::parse_command_line(argc, argv, desc), vm);

        if (vm.contains("help")) {
            std::stringstream ss_desc;
            ss_desc << desc;

            fmt::print("Usage sqcli [options ...]\n\n{}", ss_desc.str());
            return 1;
        }

        if (vm.contains("database")) {
            db_path = vm["database"].as<std::string>();
        }

        boost::program_options::notify(vm);
    } catch (const std::exception& ex) {
        fmt::print(stderr, "Error: {}\n", ex.what());
        return 1;
    }

    sqcli sqcli(db_path);
    sqcli.run();

    return 0;
}

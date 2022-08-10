#include <boost/program_options.hpp>
#include <iostream>

#include "sqcli/sqcli.hpp"

using namespace std;

int main(int argc, char** argv) {
    string db_path;

    try {
        boost::program_options::options_description desc("Allowed Options");
        boost::program_options::variables_map vm;

        desc.add_options()("help,h", "Get allowed options")(
            "database,d", boost::program_options::value<string>()->required(),
            "Set path to sqlite database file");

        boost::program_options::store(
            boost::program_options::parse_command_line(argc, argv, desc), vm);

        if (vm.contains("help")) {
            cout << "Usage: sqcli [options ...]\n\n" << desc;
            return 1;
        }

        if (vm.contains("database")) {
            db_path = vm["database"].as<string>();
        }

        boost::program_options::notify(vm);
    } catch (const exception& ex) {
        cout << "Error: " << ex.what() << '\n';
        return 1;
    }

    sqcli sqcli(db_path);
    sqcli.run();

    return 0;
}

#include <iostream>

#include <resources/resources.hpp>

#include "utils.hpp"

int main(int argc, char const ** argv)
{
    // for now, lets just print all functions
    for (auto const & day : DAYS_ALL) {
        auto const file_content{ read_file(day.default_input_file_path) };
        std::cout << "=========================================\n"
                  << day.name << "\n\n"
                  << day.function(file_content.c_str()) << '\n';
    }

    return 0;
}
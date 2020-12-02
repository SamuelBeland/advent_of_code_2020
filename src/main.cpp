#include <iostream>

#include <resources/resources.hpp>

int main(int argc, char const ** argv)
{
    // for now, lets just print all functions
    for (auto const & day : DAYS) {
        std::cout << "=========================================\n" << day.name << "\n\n" << day.function() << '\n';
    }

    return 0;
}
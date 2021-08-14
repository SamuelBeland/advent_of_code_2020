#include <iostream>

#include <resources.hpp>

//==============================================================================
// TODO : add some cleaner CLI options.
int main(int argc, char const ** argv)
{
    auto const * const DAYS_TO_SKIP = &DAYS[29];

    // for now, lets just print all functions
    for (auto const & day : DAYS) {
        // if (&day == DAYS_TO_SKIP) {
        //    std::cout << "Skipping day " << day.name << "...\n\n";
        //    continue;
        //}

        std::cout << day.name << ":\n\t" << day.function() << "\n\n";
    }

    return 0;
}
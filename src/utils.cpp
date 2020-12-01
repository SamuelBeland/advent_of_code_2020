#include "utils.hpp"

#include <fstream>

std::string read_file(char const * path)
{
    std::ifstream file{ path };
    file.seekg(std::ios::end);
    auto const size{ file.tellg() };
    file.seekg(std::ios::beg);
    std::string result{};
    result.reserve(size);
    result.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return result;
}
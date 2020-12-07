#include <algorithm>
#include <map>
#include <numeric>
#include <set>

#include "utils.hpp"
#include <resources.hpp>

using color_id_t = int;

//==============================================================================
std::string day_7_a(char const * input_file_path)
{
    static constexpr std::string_view TARGET = "shiny gold";
    static constexpr std::string_view SEP_1 = " bags contain ";
    static constexpr std::string_view SEP_2 = " bag";
    static constexpr std::string_view NO_BAGS_TAG = "no other bags.";

    std::set<std::string_view> colors_that_contain_target{};
    std::map<std::string_view, std::vector<std::string_view>> inverse_color_ownership_map{};

    auto const input{ read_file(input_file_path) };
    auto const lines{ split(input) };

    for (auto const & line : lines) {
        // contains no bag - skip
        if (line.find(NO_BAGS_TAG) != std::string_view::npos) {
            continue;
        }

        auto const sep_1_index{ line.find(SEP_1) };
        assert(sep_1_index != std::string::npos);
        std::string_view const owner_color{ line.data(), sep_1_index };
        auto const * parse_ptr{ line.data() + sep_1_index + SEP_1.size() };
        auto const * const line_end{ line.data() + line.size() - 1 };
        do {
            auto const * value_end{ std::find(parse_ptr, line_end, ' ') };
            assert(value_end < line_end);
            int quantity;
            [[maybe_unused]] auto const from_char_result{ std::from_chars(parse_ptr, value_end, quantity) };
            assert(from_char_result.ec == std::errc());
            parse_ptr = value_end + 1;
            auto const * owned_color_end{ std::search(parse_ptr, line_end, SEP_2.cbegin(), SEP_2.cend()) };
            assert(owned_color_end < line_end);
            auto const owned_color_size{ owned_color_end - parse_ptr };
            assert(owned_color_size > 0);
            std::string_view const owned_color{ parse_ptr, static_cast<size_t>(owned_color_size) };

            auto const emplace_result{
                inverse_color_ownership_map.try_emplace(owned_color, std::vector<std::string_view>{ owner_color })
            };
            if (!emplace_result.second) {
                emplace_result.first->second.emplace_back(owner_color);
            }

            parse_ptr += owned_color.size() + SEP_2.size() + 2u;
            if (quantity > 1) {
                parse_ptr += 1u;
            }
        } while (parse_ptr < line_end);
    }

    // now find all bags that own target
    std::function<void(std::string_view const &)> const register_all_owners
        = [&register_all_owners, &colors_that_contain_target, &inverse_color_ownership_map](
              std::string_view const & owned) {
              auto const find_result{ inverse_color_ownership_map.find(owned) };
              if (find_result != inverse_color_ownership_map.cend()) {
                  for (auto const & owner : find_result->second) {
                      auto const emplace_result{ colors_that_contain_target.emplace(owner) };
                      if (emplace_result.second) {
                          // insertion took place
                          register_all_owners(owner);
                      }
                  }
              }
          };
    register_all_owners(TARGET);
    auto const result{ colors_that_contain_target.size() };

    return std::to_string(result);
}

//==============================================================================
std::string day_7_b(char const * input_file_path)
{
    return "day 7 b not implemented";
}
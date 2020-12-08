#include <algorithm>
#include <map>
#include <numeric>
#include <set>

#include "utils.hpp"
#include <resources.hpp>

using color_id_t = unsigned;

template<typename T, size_t MAX_SIZE>
class Static_Vector
{
    std::array<T, MAX_SIZE> m_data;
    size_t m_size{};

public:
    T * begin() { return m_data.data(); }
    T * end() { return m_data.data() + m_size; }
    T const * begin() const { return m_data.data(); }
    T const * end() const { return m_data.data() + m_size; }
    T const * cbegin() const { return m_data.data(); }
    T const * cend() const { return m_data.data() + m_size; }

    T & push_back(T const & new_element)
    {
        assert(m_size < MAX_SIZE);
        return m_data[m_size++] = new_element;
    }
};

template<typename T>
struct Color_Ownership {
    T color;
    unsigned quantity;
};

struct Color_Info {
    color_id_t id;
    Static_Vector<Color_Ownership<color_id_t>, 4> colors_contained_by_me; // quantity?
    Static_Vector<Color_Ownership<color_id_t>, 32> colors_that_contain_me;
};

struct Rule {
    std::string_view color;
    Static_Vector<Color_Ownership<std::string_view>, 4> owned_colors;
};

class Color_Graph
{
    std::map<std::string_view, color_id_t> m_color_names_to_color_ids{};
    std::vector<Color_Info> m_color_infos;
    color_id_t m_next_id{};

public:
    void add_rule(Rule const & rule)
    {
        auto const owner_id{ get_or_add_color_id(rule.color) };

        // add each owned colors on each sides
        for (auto const & owned_color : rule.owned_colors) {
            auto const owned_id{ get_or_add_color_id(owned_color.color) };
            m_color_infos[owner_id].colors_contained_by_me.push_back(
                Color_Ownership<color_id_t>{ owned_id, owned_color.quantity });
            m_color_infos[owned_id].colors_that_contain_me.push_back(
                Color_Ownership<color_id_t>{ owner_id, owned_color.quantity });
        }
    }

    size_t get_number_of_colors_that_contain_color(std::string_view const & target_name) const
    {
        assert(m_color_names_to_color_ids.find(target_name) != m_color_names_to_color_ids.cend());

        std::set<color_id_t> colors_that_own_target{};

        const std::function<void(color_id_t)> const register_owners
            = [this, &colors_that_own_target, &register_owners](color_id_t const target_id) {
                  for (auto const & owner : m_color_infos[target_id].colors_that_contain_me) {
                      auto const emplace_result{ colors_that_own_target.emplace(owner.color) };
                      if (emplace_result.second) {
                          // insertion took place : new owner
                          // register owners' owners
                          register_owners(owner.color);
                      }
                  }
              };

        auto const target_id{ m_color_names_to_color_ids.at(target_name) };
        register_owners(target_id);
        return colors_that_own_target.size();
    }

    size_t get_number_of_bags_contained_by_color(std::string_view const & target_name) const {}

private:
    color_id_t get_or_add_color_id(std::string_view const & color_name)
    {
        auto const emplace_result{ m_color_names_to_color_ids.try_emplace(color_name, m_next_id) };
        if (emplace_result.second) {
            // insertion took place : new color
            m_color_infos.push_back(Color_Info{ m_next_id++, {}, {} });
        }
        return emplace_result.first->second;
    }
};

Rule parse_rule(std::string_view const & line)
{
    static constexpr std::string_view SEP_1 = " bags contain ";
    static constexpr std::string_view SEP_2 = " bag";
    static constexpr std::string_view NO_BAGS_TAG = "no other bags.";

    Rule rule;

    auto const * const color_start{ line.data() };
    auto const color_length{ line.find(SEP_1) };
    rule.color = std::string_view{ color_start, color_length };

    if (line.find(NO_BAGS_TAG) == std::string_view::npos) {
        // there are contained bags
        auto const * parse_ptr{ line.data() + color_length + SEP_1.size() };
        auto const * const line_end{ line.data() + line.size() };
        do {
            auto const * const quantity_begin{ parse_ptr };
            auto const * const quantity_end{ std::find(quantity_begin, line_end, ' ') };
            assert(quantity_end < line_end);
            auto const * const owned_color_begin{ quantity_end + 1 };
            auto const * const owned_color_end{
                std::search(owned_color_begin, line_end, SEP_2.cbegin(), SEP_2.cend())
            };
            assert(owned_color_end < line_end);
            auto const owned_color_length{ static_cast<size_t>(owned_color_end - owned_color_begin) };

            unsigned quantity;
            [[maybe_unused]] auto const from_char_result{ std::from_chars(quantity_begin, quantity_end, quantity) };
            assert(from_char_result.ec == std::errc());

            Color_Ownership<std::string_view> const owned_color{ std::string_view{ owned_color_begin,
                                                                                   owned_color_length },
                                                                 quantity };
            rule.owned_colors.push_back(owned_color);

            parse_ptr = owned_color_end + SEP_2.size() + 2;
            if (quantity > 1) {
                ++parse_ptr;
            }
        } while (parse_ptr < line_end);
    }

    return rule;
}

//==============================================================================
std::string day_7_a_alt(char const * input_file_path)
{
    static constexpr std::string_view TARGET = "shiny gold";

    auto const input{ read_file(input_file_path) };
    auto const lines{ split(input) };

    std::vector<Rule> rules{};
    rules.resize(lines.size());
    std::transform(lines.cbegin(), lines.cend(), rules.begin(), parse_rule);

    Color_Graph color_graph{};
    for (auto const & rule : rules) {
        color_graph.add_rule(rule);
    }

    auto const result{ color_graph.get_number_of_colors_that_contain_color(TARGET) };
    return std::to_string(result);
}

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
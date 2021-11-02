//-- - Day 7: Handy Haversacks-- -
// You land at the regional airport in time for your next flight.In fact, it looks like you'll even have time to grab
// some food: all flights are currently delayed due to issues in luggage processing.
//
// Due to recent aviation regulations, many rules(your puzzle input) are being enforced about bagsand their contents;
// bags must be color - coded and must contain specific quantities of other color - coded bags.Apparently, nobody
// responsible for these regulations considered how long they would take to enforce!
//
// For example, consider the following rules :
//
// light red bags contain 1 bright white bag, 2 muted yellow bags.
// dark orange bags contain 3 bright white bags, 4 muted yellow bags.
// bright white bags contain 1 shiny gold bag.
// muted yellow bags contain 2 shiny gold bags, 9 faded blue bags.
// shiny gold bags contain 1 dark olive bag, 2 vibrant plum bags.
// dark olive bags contain 3 faded blue bags, 4 dotted black bags.
// vibrant plum bags contain 5 faded blue bags, 6 dotted black bags.
// faded blue bags contain no other bags.
// dotted black bags contain no other bags.
//
// These rules specify the required contents for 9 bag types.In this example, every faded blue bag is empty, every
// vibrant plum bag contains 11 bags(5 faded blue and 6 dotted black), and so on.
//
// You have a shiny gold bag.If you wanted to carry it in at least one other bag, how many different bag colors would be
// valid for the outermost bag ? (In other words : how many colors can, eventually, contain at least one shiny gold bag
// ?
// )
//
// In the above rules, the following options would be available to you :
//
// A bright white bag, which can hold your shiny gold bag directly.
// A muted yellow bag, which can hold your shiny gold bag directly, plus some other bags.
// A dark orange bag, which can hold bright whiteand muted yellow bags, either of which could then hold your shiny gold
// bag. A light red bag, which can hold bright whiteand muted yellow bags, either of which could then hold your shiny
// gold bag.
//
// So, in this example, the number of bag colors that can eventually contain at least one shiny gold bag is 4.
//
// How many bag colors can eventually contain at least one shiny gold bag ? (The list of rules is quite long; make sure
// you get all of it.)
//
//-- - Part Two-- -
// It's getting pretty expensive to fly these days - not because of ticket prices, but because of the ridiculous number
// of bags you need to buy!
//
// Consider again your shiny gold bag and the rules from the above example :
//
// faded blue bags contain 0 other bags.
// dotted black bags contain 0 other bags.
// vibrant plum bags contain 11 other bags : 5 faded blue bags and 6 dotted black bags.
// dark olive bags contain 7 other bags : 3 faded blue bags and 4 dotted black bags.
//
// So, a single shiny gold bag must contain 1 dark olive bag(and the 7 bags within it) plus 2 vibrant plum bags(and the
// 11 bags within each of those) : 1 + 1 * 7 + 2 + 2 * 11 = 32 bags!
//
// Of course, the actual rules have a small chance of going several levels deeper than this example; be sure to count
// all of the bags, even if the nesting becomes topologically impractical!
//
// Here's another example:
//
// shiny gold bags contain 2 dark red bags.
// dark red bags contain 2 dark orange bags.
// dark orange bags contain 2 dark yellow bags.
// dark yellow bags contain 2 dark green bags.
// dark green bags contain 2 dark blue bags.
// dark blue bags contain 2 dark violet bags.
// dark violet bags contain no other bags.
//
// In this example, a single shiny gold bag must contain 126 other bags.
//
// How many individual bags are required inside your single shiny gold bag ?

#include <optional>
#include <set>

#include "utils.hpp"
#include <resources.hpp>

namespace
{
using color_id_t = unsigned;

template<typename T>
struct Color_Ownership {
    T color;
    unsigned quantity;
};

constexpr size_t MAX_OWNED = 4;
constexpr size_t MAX_OWNERS = 32;

struct Color_Info {
    color_id_t id;
    aoc::Static_Vector<Color_Ownership<color_id_t>, MAX_OWNED> colors_contained_by_me;
    aoc::Static_Vector<Color_Ownership<color_id_t>, MAX_OWNERS> colors_that_contain_me;
};

struct Rule {
    aoc::StringView color;
    aoc::Static_Vector<Color_Ownership<aoc::StringView>, MAX_OWNED> owned_colors;
    //==============================================================================
    static Rule from_string(aoc::StringView const & string)
    {
        Rule rule;
        aoc::StringView leftover;
        string.scan("{} bags contain {}.", rule.color, leftover);

        if (leftover == "no other bags") {
            return rule;
        }

        aoc::Static_Vector<aoc::StringView, MAX_OWNED> contained_strings;

        leftover.iterate([&](aoc::StringView const & element) { contained_strings.push_back(element); }, ", ");

        for (auto const & contained_string : contained_strings) {
            Color_Ownership<aoc::StringView> ownership;
            contained_string.scan("{} {} bag", ownership.quantity, ownership.color);
            rule.owned_colors.push_back(ownership);
        }

        return rule;
    }
};

//==============================================================================
class Color_Graph
{
    std::unordered_map<aoc::StringView, color_id_t> m_color_names_to_color_ids{};
    std::vector<Color_Info> m_color_infos;
    color_id_t m_next_id{};

public:
    //==============================================================================
    Color_Graph() = delete;
    ~Color_Graph() = default;

    Color_Graph(Color_Graph const &) = default;
    Color_Graph(Color_Graph &&) = default;

    Color_Graph & operator=(Color_Graph const &) = default;
    Color_Graph & operator=(Color_Graph &&) = default;
    //==============================================================================
    Color_Graph(aoc::StringView const & input)
    {
        auto const lines{ split(input, '\n') };

        std::vector<Rule> rules{};
        rules.resize(lines.size());
        aoc::transform(lines, rules, Rule::from_string);

        for (auto const & rule : rules) {
            add_rule(rule);
        }
    }
    //==============================================================================
    size_t get_number_of_colors_that_contain_color(aoc::StringView const & target_name) const
    {
        assert(m_color_names_to_color_ids.find(target_name) != m_color_names_to_color_ids.cend());

        std::set<color_id_t> colors_that_own_target{};

        std::function<void(color_id_t)> const register_owners
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
    //==============================================================================
    size_t get_number_of_bags_contained_by_color(aoc::StringView const & target_name) const
    {
        assert(m_color_names_to_color_ids.find(target_name) != m_color_names_to_color_ids.cend());
        auto const target_id{ m_color_names_to_color_ids.at(target_name) };

        std::vector<std::optional<unsigned>> bags_contained_in_colors{};
        bags_contained_in_colors.resize(m_color_infos.size());

        std::function<void(color_id_t)> compute_number_of_bags_in_bag
            = [this, &bags_contained_in_colors, &compute_number_of_bags_in_bag](color_id_t const target_id) {
                  unsigned count{ 1 };
                  if (m_color_infos[target_id].colors_contained_by_me.size() > 0) {
                      for (auto const contained_colors : m_color_infos[target_id].colors_contained_by_me) {
                          if (!bags_contained_in_colors[contained_colors.color]) {
                              compute_number_of_bags_in_bag(contained_colors.color);
                          }
                          count += contained_colors.quantity * bags_contained_in_colors[contained_colors.color].value();
                      }
                  }
                  bags_contained_in_colors[target_id] = count;
              };

        compute_number_of_bags_in_bag(target_id);
        return bags_contained_in_colors[target_id].value() - 1; // -1 to exclude the current bag
    }

private:
    //==============================================================================
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
    //==============================================================================
    color_id_t get_or_add_color_id(aoc::StringView const & color_name)
    {
        auto const emplace_result{ m_color_names_to_color_ids.try_emplace(color_name, m_next_id) };
        if (emplace_result.second) {
            // insertion took place : new color
            m_color_infos.emplace_back(Color_Info{ m_next_id++, {}, {} });
        }
        return emplace_result.first->second;
    }
};

constexpr aoc::StringView TARGET = "shiny gold";

} // namespace

//==============================================================================
std::string day_7_a(char const * input_file_path)
{
    auto const input{ aoc::read_file(input_file_path) };
    Color_Graph const graph{ input };

    auto const result{ graph.get_number_of_colors_that_contain_color(TARGET) };
    return std::to_string(result);
}

//==============================================================================
std::string day_7_b(char const * input_file_path)
{
    auto const input{ aoc::read_file(input_file_path) };
    Color_Graph const graph{ input };

    auto const result{ graph.get_number_of_bags_contained_by_color(TARGET) };
    return std::to_string(result);
}
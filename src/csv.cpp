#include "include/csv.h"

#include <string>
#include <string_view>
#include <fstream>
#include <ranges>

#include <fmt/format.h>

constexpr std::string_view WHITESPACE = " \n\r\t\f\v";
 
namespace {
std::string ltrim(const std::string &s) {
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string rtrim(const std::string &s) {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
 
std::string trim(const std::string &s) {
    return rtrim(ltrim(s));
}

} // anonymous namespace


std::optional<ConceptContext> readConceptCSV(std::istream& is) {
    constexpr std::string_view delim = ",";
    ConceptContext result;

    std::string line;
    if (!std::getline(is, line))
        return std::nullopt;

    for (const auto word : std::views::split(line, delim) | std::views::drop(1))
        result.attributeNames.emplace_back(&*word.begin(), std::ranges::distance(word));

    int rowLen = -1;
    while (std::getline(is, line)) {
        line += " ";
        int i = 0;
        bitset row{};
        for (const auto word : std::views::split(line, delim)) {
            std::string entry(&*word.begin(), std::ranges::distance(word));
            entry = trim(entry);

            if (i == 0) {
                result.objectNames.push_back(std::move(entry));
                ++i;
                continue;
            }

            if (entry.empty())
                row.push_back(0);
            else
                row.push_back(1);

            ++i;
        }

        if (rowLen == -1)
            rowLen = row.size();

        if (rowLen != static_cast<int>(row.size())) {
            fmt::print("Entry number mismatch in row '{}', found '{}' entries, expected '{}'\n", i, static_cast<int>(row.size()), rowLen);
            return std::nullopt;
        }

        result.rows.push_back(std::move(row));
    }

    return result;
}

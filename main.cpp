#include <fmt/format.h>
#include <vector>
#include <string>
#include <fstream>
#include <ranges>
#include <optional>
#include <set>
#include <cassert>

constexpr std::string_view WHITESPACE = " \n\r\t\f\v";
 
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

using bitset = std::vector<bool>;

struct Data {
    std::vector<std::string> attributeNames;
    std::vector<std::string> objectNames;

    std::vector<bitset> rows;
};

std::optional<Data> readCSV(std::istream& is) {
    constexpr std::string_view delim = ",";
    Data result;

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

bitset set_union(const bitset& lhs, const bitset& rhs) {
    assert(lhs.size() == rhs.size());
    bitset result;
    for (unsigned i = 0; i < lhs.size(); ++i) {
        result.push_back(lhs[i] || rhs[i]);
    }
    return result;
}

bool contains(const bitset& lhs, const bitset& rhs) {
    assert(lhs.size() == rhs.size());
    for (unsigned i = 0; i < lhs.size(); ++i) {
        if (lhs[i] && !rhs[i])
            return false;
    }
    return true;
}

bitset set_intersect(const bitset& lhs, const bitset& rhs) {
    assert(lhs.size() == rhs.size());
    bitset result;
    for (unsigned i = 0; i < lhs.size(); ++i) {
        result.push_back(lhs[i] && rhs[i]);
    }
    return result;
}

bitset set_subtract(const bitset& lhs, const bitset& rhs) {
    assert(lhs.size() == rhs.size());
    bitset result;
    for (unsigned i = 0; i < lhs.size(); ++i) {
        result.push_back(lhs[i] && !rhs[i]);
    }
    return result;
}

bitset closure(const bitset& intents, const Data& context) {
    std::set<int> rows;
    int i = 0;
    for (const auto& r : context.rows) {
        if (contains(intents, r))
            rows.insert(i);
        ++i;
    }

    bitset result(intents.size(), 1);
    for (int r : rows) {
        result = set_intersect(result, context.rows[r]);
    }

    return result;
}

bitset extents(const bitset& intents, const Data& context) {
    bitset exts(context.objectNames.size(), 0);
    int i = 0;
    for (const auto& r : context.rows) {
        if (contains(intents, r))
            exts[i] = 1;
        ++i;
    }
    return exts;
}

std::set<int> to_sparse(const bitset& vec) {
    std::set<int> result;
    for(unsigned i = 0; i < vec.size(); ++i) {
        if (vec[i])
            result.insert(i);
    }
    return result;
}
std::optional<bitset> nextClosure(bitset intents, const Data& data) {
    for (int i = static_cast<int>(data.rows.front().size()) - 1; i >= 0; --i) {
        if (intents[i]) {
            intents[i] = 0;
        } else {
            bitset candidate = intents;
            candidate[i] = 1;
            candidate = closure(candidate, data);
            bitset toCheck = set_subtract(candidate, intents);
            bool success = true;
            for(int j = 0; j < i; ++j) {
                if (toCheck[j]) {
                    success = false;
                    break;
                }
            }
            if (success)
                return candidate;
        }
    }
    return std::nullopt;
}

struct Concept {
    std::set<int> objects;
    std::set<int> attributes;
};

struct Lattice {
    std::vector<std::pair<int, int>> less;
    std::vector<Concept> concepts;
};

Lattice conceptAnalysis(const Data& data) {
    Lattice result;

    bitset empty(data.rows.front().size(), 0);
    std::optional<bitset> next = closure(empty, data);

    while(next) {
        result.concepts.emplace_back(to_sparse(extents(*next, data)), to_sparse(*next));
        
        next = nextClosure(*next, data); 
    }

    return result;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fmt::print("Usage: {} inputfilename.csv\n", argv[0]);
        return 0;
    }
    
    std::ifstream inFile(argv[1]);
    if (!inFile.is_open()) {
        fmt::print("Could not open: {}\n", argv[1]);
        return 1;
    }

    auto data = readCSV(inFile);
    if (!data) {
        fmt::print("Failed to load the content of CSV: {}\n", argv[1]);
        return 1;
    }

    auto lattice = conceptAnalysis(*data);
    for (const auto& conc : lattice.concepts) {
        for (int i : conc.objects) {
            fmt::print("{} ", data->objectNames[i]);
        }
        fmt::print("\n");
        for (int i : conc.attributes) {
            fmt::print("{} ", data->attributeNames[i]);
        }
        fmt::print("\n");
        fmt::print("-----------------\n");
    }
}

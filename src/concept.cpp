#include "include/concept.h"

#include <algorithm>
#include <cassert>

#include "include/bitset.h"
#include "include/csv.h"

bitset closure(const bitset& intents, const ConceptContext& context) {
    bitset result(intents.size(), true);
    int i = 0;
    for (const auto& r : context.rows) {
        if (subset(intents, r))
            result = set_intersect(result, context.rows[i]);
        ++i;
    }

    return result;
}

bitset extents(const bitset& intents, const ConceptContext& context) {
    bitset exts(context.objectNames.size(), false);
    int i = 0;
    for (const auto& r : context.rows) {
        if (subset(intents, r))
            exts[i] = true;
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

std::optional<bitset> nextClosure(bitset intents, const ConceptContext& data) {
    for (int i = static_cast<int>(data.rows.front().size()) - 1; i >= 0; --i) {
        if (intents[i]) {
            intents[i] = false;
        } else {
            bitset candidate = intents;
            candidate[i] = true;
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

std::set<bitset> upperNeighbors(const bitset& conc, const ConceptContext& data) {
    int i = -1;
    bitset n = conc;
    n.flip();
    bitset toCloseWith = n;
    for (bool b : n) {
        ++i;
        if (!b)
            continue;

        bitset candidate = conc;
        candidate[i] = true;
        candidate = closure(candidate, data);
        candidate = set_intersect(candidate, toCloseWith);
        if (candidate[i] && set_bits(candidate) == 1)
            continue;
        toCloseWith[i] = false;
    }

    i = -1;
    std::set<bitset> result;
    for (bool b : toCloseWith) {
        ++i;
        if (!b)
            continue;

        bitset candidate = conc;
        candidate[i] = true;
        result.insert(closure(candidate, data));
    }
    return result;
}

void constructGraph(Lattice& l, const ConceptContext& data) {
    int i = 0;
    for (const auto& conc : l.concepts) {
        auto upper = upperNeighbors(conc.attributes, data);
        for(const auto& bset : upper) {
            auto it = std::lower_bound(l.concepts.begin(), l.concepts.end(), bset, [](const Concept& c, const bitset& bs) { return c.attributes < bs; });
            assert(it != l.concepts.end());
            l.less.emplace_back(i, it - l.concepts.begin());
        }
        ++i;
    }
}

Lattice conceptAnalysis(const ConceptContext& data) {
    Lattice result;

    bitset empty(data.rows.front().size(), false);
    std::optional<bitset> next = closure(empty, data);

    while(next) {
        result.concepts.emplace_back(extents(*next, data), *next);
        
        next = nextClosure(*next, data); 
    }

    constructGraph(result, data);

    return result;
}

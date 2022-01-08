#include "include/concept.h"

#include "include/bitset.h"
#include "include/csv.h"

bitset closure(const bitset& intents, const ConceptContext& context) {
    std::set<int> rows;
    int i = 0;
    for (const auto& r : context.rows) {
        if (subset(intents, r))
            rows.insert(i);
        ++i;
    }

    bitset result(intents.size(), 1);
    for (int r : rows) {
        result = set_intersect(result, context.rows[r]);
    }

    return result;
}

bitset extents(const bitset& intents, const ConceptContext& context) {
    bitset exts(context.objectNames.size(), 0);
    int i = 0;
    for (const auto& r : context.rows) {
        if (subset(intents, r))
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

std::optional<bitset> nextClosure(bitset intents, const ConceptContext& data) {
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

Lattice conceptAnalysis(const ConceptContext& data) {
    Lattice result;

    bitset empty(data.rows.front().size(), 0);
    std::optional<bitset> next = closure(empty, data);

    while(next) {
        result.concepts.emplace_back(to_sparse(extents(*next, data)), to_sparse(*next));
        
        next = nextClosure(*next, data); 
    }

    return result;
}

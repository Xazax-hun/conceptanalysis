#include "include/bitset.h"
#include <cassert>

bool subset(const bitset& lhs, const bitset& rhs) {
    assert(lhs.size() == rhs.size());
    for (unsigned i = 0; i < lhs.size(); ++i) {
        if (lhs[i] && !rhs[i])
            return false;
    }
    return true;
}

bitset set_union(const bitset& lhs, const bitset& rhs) {
    assert(lhs.size() == rhs.size());
    bitset result;
    for (unsigned i = 0; i < lhs.size(); ++i) {
        result.push_back(lhs[i] || rhs[i]);
    }
    return result;
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


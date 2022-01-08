#ifndef INCLUDE_BITSET_H
#define INCLUDE_BITSET_H

#include <vector>

using bitset = std::vector<bool>;

bool subset(const bitset& lhs, const bitset& rhs);
bitset set_union(const bitset& lhs, const bitset& rhs);
bitset set_intersect(const bitset& lhs, const bitset& rhs);
bitset set_subtract(const bitset& lhs, const bitset& rhs);
bitset set_union(const bitset& lhs, const bitset& rhs);
int set_bits(const bitset& s);

#endif
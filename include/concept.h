#ifndef INCLUDE_CONCEPT_H
#define INCLUDE_CONCEPT_H

#include <set>
#include <vector>

#include "include/bitset.h"
#include "include/csv.h"

struct Concept {
    std::set<int> objects;
    std::set<int> attributes;
};

struct Lattice {
    std::vector<std::pair<int, int>> less;
    std::vector<Concept> concepts;
};

bitset closure(const bitset& intents, const ConceptContext& context);
bitset extents(const bitset& intents, const ConceptContext& context);

std::set<int> to_sparse(const bitset& vec);

std::optional<bitset> nextClosure(bitset intents, const ConceptContext& data);

// Bernhard Ganter's Next Closure algorithm.
// Ganter B. (2010) Two Basic Algorithms in Concept Analysis. In: Kwuida L., Sertkaya B. (eds) Formal Concept Analysis.
// ICFCA 2010. Lecture Notes in Computer Science, vol 5986. Springer, Berlin, Heidelberg. https://doi.org/10.1007/978-3-642-11928-6_22
Lattice conceptAnalysis(const ConceptContext& data);

#endif
#ifndef INCLUDE_CSV_H
#define INCLUDE_CSV_H

#include <vector>
#include <string>
#include <optional>
#include <iosfwd>

#include "include/bitset.h"

struct ConceptContext {
    std::vector<std::string> attributeNames; // Intents.
    std::vector<std::string> objectNames;    // Extents.

    std::vector<bitset> rows;
};

std::optional<ConceptContext> readConceptCSV(std::istream& is);

#endif
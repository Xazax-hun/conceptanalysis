#include "include/dot.h"

#include <sstream>

namespace {
Concept minimizedForDisplay(int conc, const Lattice& l, const ConceptContext& data) {
    std::set<int> preds;
    std::set<int> succs;
    for (auto e : l.less) {
        if (conc == e.second)
            preds.insert(e.first);
        if (conc == e.first)
            succs.insert(e.second);
    }

    bitset oldAttrs(data.attributeNames.size(), false);
    for (auto pred : preds)
        oldAttrs = set_union(oldAttrs, l.concepts[pred].attributes);
    bitset newAttrs = set_subtract(l.concepts[conc].attributes, oldAttrs);
    
    bitset oldObjs(data.objectNames.size(), false);
    for (auto succ : succs)
        oldObjs = set_union(oldObjs, l.concepts[succ].objects);
    bitset newObjs = set_subtract(l.concepts[conc].objects, oldObjs);

    return {newObjs, newAttrs};
}
} // anonymous namespace

// TODO: introduce proper escaping.
std::string renderDot(const Lattice& l, const ConceptContext& data) {
    std::stringstream dot;
    dot << "digraph concept_lattice {\n";
    dot << "splines = false\n";
    dot << "edge [dir=none tailport=\"s\" headport=\"n\"]\n";
    dot << "node [shape=record]\n\n";

    for (unsigned i = 0; i < l.concepts.size(); ++i) {
        auto conc = minimizedForDisplay(i, l, data);
        dot << "node" << i << " [label=\"";
        for (int i : to_sparse(conc.objects)) {
            dot << data.objectNames[i] << " ";
        }
        dot << "|";
        for (int i : to_sparse(conc.attributes)) {
            dot << data.attributeNames[i] << " ";
        }
        dot << "\"];\n";
    }

    dot << "\n";

    for (auto e : l.less) {
        dot << "node" << e.first << " -> " << "node" << e.second << ";\n";
    }

    dot << "}\n";

    return std::move(dot).str();
}
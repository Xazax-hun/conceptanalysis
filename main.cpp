#include <fmt/format.h>
#include <fstream>

#include "include/concept.h"
#include "include/dot.h"

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

    auto data = readConceptCSV(inFile);
    if (!data) {
        fmt::print("Failed to load the content of CSV: {}\n", argv[1]);
        return 1;
    }

    auto lattice = conceptAnalysis(*data);
    fmt::print("{}", renderDot(lattice, *data));
}

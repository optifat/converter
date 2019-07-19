#include <iostream>

#include "plotfilesGenerator.h"

int main() {
    std::string filename = "test";
    PlotfileGenerator generator(filename);
    generator.generatePlotfiles();
    return 0;
}
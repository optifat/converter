#ifndef GENERATEFILE_PLOTFILESGENERATOR_H
#define GENERATEFILE_PLOTFILESGENERATOR_H

class PlotfileGenerator{

public:
    PlotfileGenerator(std::string filename);
    ~PlotfileGenerator();
    void generatePlotfiles();

private:
    void generateVTIfile(std::string filename, int number, double* size, double* origin, double spacing);
    void checkFilename(std::string filename);

private:
    std::string filename;
    bool aborted;
};

#endif //GENERATEFILE_PLOTFILESGENERATOR_H

#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <iostream>
#include <filesystem>

#include "plotfilesGenerator.h"

PlotfileGenerator::PlotfileGenerator(std::string filename = "plotfile"){
    aborted = false;
    checkFilename(filename);
    this->filename = filename;
}

PlotfileGenerator::~PlotfileGenerator() = default;

void PlotfileGenerator::generatePlotfiles(){

    if(aborted){
        return;
    }

    std::stringstream f;

    double origin[3] = {-100, -100, -100};
    int number_of_levels = 4;
    int number_of_blocks[number_of_levels] = {1, 1, 1, 1};
    int total_blocks = 0;
    double spacing[number_of_levels] = {1, 0.5, 0.25, 0.125};
    for(auto value: number_of_blocks){
        total_blocks += value;
    }
    double** dimensions = new double* [total_blocks];
    for (int block = 0; block < total_blocks; block++){
        dimensions[block] = new double [6];
    }

    for(int level = 0; level < number_of_levels; level++){
        for(int block = 0; block < number_of_blocks[level]; block++){
            for(int i = 0; i<6; i++){
                if(i < 3){
                    dimensions[block][i] = 0;
                }
                else{
                    dimensions[block][i] = 9;
                }
            }
        }
    }


    f << "<VTKFile type=\"vtkOverlappingAMR\" version=\"1.1\" byte_order=\"LittleEndian\" header_type=\"UInt64\">\n";
    f << "  <vtkOverlappingAMR origin=\"" << origin[0] << " " << origin[1] << " " << origin[2]
      << "\" grid_description=\"XYZ\">\n";

    int files_created = 0;

    for(int level = 0; level < number_of_levels; level++){
        f << "    <Block level=\"" << level <<"\" spacing=\""<< spacing[level] << " "<< spacing[level] << " "
          << spacing[level] << "\">\n";
        for(int block = 0; block < number_of_blocks[level]; block++){
            f << "      <DataSet index=\"" << block << "\" amr_box=\""
              << dimensions[block][0] << " " << dimensions[block][3] << " "
              << dimensions[block][1] << " " << dimensions[block][4] << " "
              << dimensions[block][2] << " " << dimensions[block][5] <<"\" file=\""
              << filename << "/" << filename << "_" << files_created <<".vti\"/>\n";
            generateVTIfile(filename, files_created++, dimensions[block], origin, spacing[level]);
        }
        f << "    </Block>\n";
    }

    f << "  </vtkOverlappingAMR> \n</VTKFile>";

    std::ofstream file(filename+".vthb");
    file << f.rdbuf();
    file.close();

    for (int block = 0; block < total_blocks; block++){
        delete[] dimensions[block];
    }
    delete[] dimensions;
    std::cout << "Success!\n";
}

void PlotfileGenerator::generateVTIfile(std::string filename, int number, double* size, double* origin, double spacing){
    mkdir(filename.data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    std::ofstream file(filename+"/"+filename+"_"+std::to_string(number)+".vti", std::ios::app);
    file << "<VTKFile type=\"ImageData\" version=\"2.1\" byte_order=\"LittleEndian\" header_type=\"UInt64\">\n";
    file << "  <ImageData WholeExtent=\"" << 0 << " " << size[3] - size[0] + 1 << " "
         << 0 << " " << size[4] - size[1] + 1 << " "
         << 0 << " " << size[5] - size[2] + 1
         << "\" Origin=\"" << origin[0]  << " " << origin[1] << " " << origin[2]
         << "\" Spacing=\"" << spacing << " " << spacing << " " << spacing
         << "\" Direction=\"1 0 0 0 1 0 0 0 1\">\n";
    file << "  <Piece Extent=\"" << 0 << " " << size[3] - size[0] + 1 << " "
         << 0 << " " << size[4] - size[0] + 1 << " "
         << 0 << " " << size[5] - size[0] + 1 << "\">\n";
    file << "    <PointData>\n    </PointData>\n";
    file << "    <CellData>\n";
    file << "      <DataArray type=\"Float64\" Name=\"test\" format=\"ascii\" RangeMin=\"0\" RangeMax=\"0\">\n        ";

    for(int i = 0; i<(size[3] - size[0] + 1)*(size[4] - size[1] + 1)*(size[5] - size[2] + 1); i++){
        float data = i/100 + (i - i/100*100)/10 + i%100;
        data *= spacing;
        file << data;
        if(i+1 == (size[3] - size[0] + 1)*(size[4] - size[1] + 1)*(size[5] - size[2] + 1)){
            file << "\n";
        }
        else if((i+1)%6 == 0){
            file << "\n        ";
        }
        else{
            file << " ";
        }
    }

    file << "      </DataArray>\n";
    file << "      <DataArray type=\"UInt8\" Name=\"vtkGhostType\" format=\"ascii\" RangeMin=\"0\" RangeMax=\"0\">\n        ";

    for(int i = 0; i<(size[3] - size[0] + 1)*(size[4] - size[1] + 1)*(size[5] - size[2] + 1); i++){
        float data = 0;
        file << data;
        if(i+1 == (size[3] - size[0] + 1)*(size[4] - size[1] + 1)*(size[5] - size[2] + 1)){
            file << "\n";
        }
        else if((i+1)%6 == 0){
            file << "\n        ";
        }
        else{
            file << " ";
        }
    }

    file << "      </DataArray>\n";
    file << "    </CellData>\n";
    file << "  </Piece>\n  </ImageData>\n</VTKFile>";

    file.close();
}

void PlotfileGenerator::checkFilename(std::string filename) {
    struct stat buffer;
    if(stat (filename.c_str(), &buffer) == 0){
        std::cout << "Plotfiles \"" + filename + "\" already exist. Do you wish me to delete them and write new ones? [y/n]: ";
        char answer;
        std::cin >> answer;
        if((char)tolower(answer) == 'y'){
            std::filesystem::remove_all(filename);
            filename += ".vthb";
            std::remove(filename.c_str());
            std::cout << "Files deleted\n";
        }
        else if ((char)tolower(answer) == 'n'){
            std::cout << "Aborted\n";
            aborted = true;
        }
        else{
            std::cout << "Unknown answer, aborted\n";
            aborted = true;
        }
    }
}
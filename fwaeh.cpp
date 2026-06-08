#include "FileOutputStream.hpp"
#include "StandartSerializers.hpp"
#include <string>


int main(){
    std::string filename = "fwaeh.txt";

    std::ofstream touch(filename, std::ios::app);
    touch.close();
 
    StringSerializer strSerializer;
    FileOutputStream<std::string>* outStream = new FileOutputStream<std::string>(filename, &strSerializer);

    outStream->Open();
    outStream->Write("FWAEH!");
    outStream->Close();

    delete outStream;

    return 0;
}
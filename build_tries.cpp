#include <iostream>
#include "util_functions.hpp"



int main(int argc, char** argv) {
    int mandatory = 3;

    if (argc < argv){
        std::cout   << "collection filename 
                        [--rank v] 
                        [--runs r]
                        [--out output_filename]"
                    <<
        std::endl;
        return 1;
    }

    int rank;
    bool runs;
    std::string output_filename = "";
    for (int i = 2; i < argc; ++i){
        if (std::string(argv[i]) == "--rank") {
            ++i;
            rank = int(argv[i]);
        }
        if (std::string(argv[i]) == "--runs") {
            ++i;
            if (int(argv[i]) > 0)
                runs = true;
            else
                runs = false;
        }
        if (std::string(argv[i]) == "--output_filename") {
            ++i;
            output_filename = std::string(argv[i]);
        }
    }

    // Call function here

}
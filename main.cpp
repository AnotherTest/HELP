/**
 * @file main.cpp
 * Provides the entry point main().
 * @author Tim Beyne
 */
#include "Preprocessor.h"

int main(int argc, char** argv)
{
    try {
        if(argc < 2)
            throw std::runtime_error("invalid usage (input file required)");
        Preprocessor p(argv[1]);
        p.process();
    } catch(std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
    }
}

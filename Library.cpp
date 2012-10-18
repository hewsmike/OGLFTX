#include "Library.h"

#include <iostream>

namespace OGLFTX {
    // The static instance of the FreeType library wrapper.
    Library Library::library;

    // The FreeType library handle itself.
    FT_Library Library::library_;

    // The static instance above causes the constructor below
    // to be called when the object module is loaded.
    Library::Library(void) {
        if(FT_Init_FreeType(&library_) != 0) {
            std::cerr << "Could not initialize the FreeType library. Exiting." << std::endl;
            exit(1);
            }
        }

    Library::~Library(void) {
        if(FT_Done_FreeType(library_) != 0) {
            std::cerr << "Could not terminate the FreeType library." << std::endl;
            }
        }

    // Return the only instance in the process
    FT_Library& Library::instance(void) {
        return library_;
        }
    }

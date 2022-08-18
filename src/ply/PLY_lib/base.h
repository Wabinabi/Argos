// A C++ reader/writer of .ply files.
// Base types.
//
// Adapted to C++ streams by Thijs van Lankveld.
/*
Header for PLY polygon files.

- Greg Turk

A PLY file contains a single polygonal _object_.

An object is composed of lists of _elements_.  Typical elements are
vertices, faces, edges and materials.

Each type of element for a given object has one or more _properties_
associated with the element type.  For instance, a vertex element may
have as properties three floating-point values x,y,z and three unsigned
chars for red, green and blue.

-----------------------------------------------------------------------

Copyright (c) 1998 Georgia Institute of Technology.  All rights reserved.
 
Permission to use, copy, modify and distribute this software and its
documentation for any purpose is hereby granted without fee, provided
that the above copyright notice and this permission notice appear in
all copies of this software and that you do not sell the software.
 
THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef __PLY_BASE_H__
#define __PLY_BASE_H__


#include <iostream>
namespace PLY {
    const char _VERSION[] = "1.2.0";

    enum Stream_type {ASCII =       1,  
                      BINARY_BE =   2,  
                      BINARY_LE =   3   
    };

    enum Scalar_type {StartType =   0,  
                      Int8 =        1,  
                      Int16 =       2,  
                      Int32 =       3,  
                      Uint8 =       4,  
                      Uint16 =      5,  
                      Uint32 =      6,  
                      Float32 =     7,  
                      Float64 =     8,  
                      EndType =     9   
    };
    
    const char* type_names[];
    const char* old_type_names[];

    const size_t ply_type_bytes[] = {0, 1, 2, 4, 1, 2, 4, 4, 8, 0};
    
    const int BIG_STRING = 4096;

    enum Variable_type {SCALAR =    0,  
                        LIST =      1,  
                        STRING =    2   
    };

    enum Verbatim {IGNORE,      
                   WARNING,     
                   EXCEPTION,   
                   ERROR        
    };

    const Verbatim ON_FAULT = EXCEPTION;

#define HANDLE_FAULT(msg) {\
    switch (ON_FAULT) {\
        case IGNORE:\
            break;\
        case WARNING:\
        case EXCEPTION:\
        case ERROR:\
            std::cerr << msg << std::endl;\
            switch (ON_FAULT) {\
            case WARNING:\
                break;\
            case ERROR:\
                exit(1);\
            case EXCEPTION:\
                return false;\
            }\
        }\
    }
} // namespace PLY


#endif // __PLY_BASE_H__
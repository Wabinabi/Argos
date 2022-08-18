// A C++ reader/writer of .ply files.
// The header and its elements.
// These describe how the data is stored in the file/stream.
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


#ifndef __PLY_HEADER_H__
#define __PLY_HEADER_H__


#include "base.h"
#include <iterator>
#include <string>
#include <vector>

namespace PLY {

    struct Property {
        std::string name;       
        Variable_type type;     

        Scalar_type data_type;  
        Scalar_type size_type;  

        bool store;             

        Property(): type(SCALAR),
            data_type(StartType),
            size_type(StartType),
            store(true) {}

        Property(const char* n, const Variable_type& t,
            const Scalar_type& dt, const Scalar_type& st = StartType)
            : name(n), type(t), data_type(dt), size_type(st), store(true) {}
    }; // struct Property



    struct Element {
        std::string name;               
        size_t num;                     
        std::vector<Property> props;    

        bool store;                     

        Element(): num(0), store(true) {}

        Element(const char* n): name(n), num(0), store(true) {}


        inline void add_property(const Property& prop) { props.push_back(prop); }


        bool find_index(const char* name, size_t& index) const;


        Property* find_property(const char* name);
    }; // struct Element

    

    struct Header {
    private:
        Stream_type system_type;            // The binary mode of the system.

        Stream_type is_LE() const;

    public:
        float version;                      
        Stream_type stream_type;            

        std::vector<Element> elements;      
        std::vector<std::string> comments;  
        std::vector<std::string> obj_info;  


        Header(): stream_type(ASCII), version(1.0), system_type(is_LE()) {}


        Stream_type system() const {return system_type;}


        inline void add_element(const Element& elem) {elements.push_back(elem);}
        

        bool find_index(const char* name, size_t& index) const;


        Element* find_element(const char* name);


        void apply_stream_type(char* ptr, size_t n);
    }; // struct Header


    // Checks if a character is white-space.
    struct WhiteSpace {
        WhiteSpace() {}
        bool operator()(char c) const;
    }; // struct WhiteSpace


    /*  The line Tokenizer extracts words from a line based
     *  on white-space. Any type of white-space is considered
     *  to separate different words.
     */
    struct Tokenizer {
        // The current line.
        char* line;

        // Constructor.
        Tokenizer() {}

        // Get the next word.
        bool next(char* word);
    }; // class Tokenizer
} // namespace PLY


#endif // __PLY_HEADER_H__

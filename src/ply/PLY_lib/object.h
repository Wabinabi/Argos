// A C++ reader/writer of .ply files.
// Data objects.
// These contain the actual data.
// Generally, the user will implement subclasses
// for their own object types.
//
//Copyright (C) 2013  INRIA - Sophia Antipolis
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Author(s):      Thijs van Lankveld


#ifndef __PLY_OBJECT_H__
#define __PLY_OBJECT_H__


#include "header.h"

namespace PLY {

    struct Value {


        virtual bool get_scalar(const Property& prop, double& value) const {return false;}


        virtual bool set_scalar(const Property& prop, const double& value) {return false;}


        virtual bool get_size(const Property& prop, size_t& size) const {return false;}


        virtual bool get_item(const Property& prop, const size_t& num, double& value) const {return false;}


        virtual bool set_size(const Property& prop, const size_t& size) {return false;}


        virtual bool set_item(const Property& prop, const size_t& num, const double& value) {return false;}


        virtual bool get_string(const Property& prop, char* str) const {return false;}


        virtual bool set_string(const Property& prop, const char* str) {return false;}

        bool copy(const Property& prop, Value& to, const Property& to_prop) const;
    }; // struct Value



    struct Object {

        virtual void prepare(const Element& elem) {}


        virtual Value* get_value(const Element& elem, const Property& prop) = 0;


        virtual bool make_element(Element& elem) const {return false;}


        bool describe_element(Header& header) const;


        bool storage_test(const Element& elem);
    }; // struct Object


    struct Array {

        virtual size_t size() = 0;
        

        virtual void prepare(const size_t& size) = 0;
        

        void prepare(const Element& elem) { prepare(elem.props.size()); }

        virtual void clear() = 0;

        virtual void restart() = 0;


        virtual Object& next_object() = 0;
        

        template < class T >
        T& next() { return dynamic_cast<T&>(next_object()); }
    }; // struct Array


    struct Storage {
        Array** collect;                    
        
        Storage(): collect(0), num(0), unknown(0) {}

        Storage(const Header& header);
        ~Storage() { deinit(); }        
        

        void prepare(const Header& header);


        Array* get_collection(const Header& header, const Element& elem);


        bool set_collection(const Header& header, const Element& elem, Array& coll);

    private:
        Storage(const Storage&) {}      // Private copy constructor to protect the collections.
        
        void init(size_t size);         // Initialize the arrays.
        void deinit();                  // Deinitialize the arrays.

        size_t num;                     // The number of collections.
        bool* unknown;                  // The unknown Object collections.
    }; // struct Storage
} // namespace PLY


#endif // __PLY_OBJECT_H__
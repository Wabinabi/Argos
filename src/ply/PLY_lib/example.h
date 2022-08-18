//Example elements for the C++ ply reader/writer.
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
//
//
// TODO: doxygen


#ifndef __PLY_EXAMPLES_H__
#define __PLY_EXAMPLES_H__


#include "unknown.h"

namespace PLY {
    // Example object classes.

    struct FloatValue: public Value {
        float val;      

        FloatValue(): val(0) {}

        FloatValue(const float& v): val(v) {}
    
        // Get the scalar value.
        bool get_scalar(const Property& prop, double& value) const {
            if (prop.type != SCALAR) return false;
            value = val; return true;
        }

        // Set the scalar value.
        bool set_scalar(const Property& prop, const double& value) {
            if (prop.type != SCALAR) return false;
            val = (float)value; return true;
        }
    }; // struct FloatValue



    struct Vertex: public Object {

        FloatValue value_x, value_y, value_z;
        Vertex(): value_x(0), value_y(0), value_z(0) {}

        Vertex(float x, float y, float z): value_x(x), value_y(y), value_z(z) {}

        // Get a Value.
        Value* get_value(const Element& elem, const Property& prop) {
            if (prop.name.compare(prop_x.name.c_str()) == 0)        return &value_x;
            else if (prop.name.compare(prop_y.name.c_str()) == 0)   return &value_y;
            else if (prop.name.compare(prop_z.name.c_str()) == 0)   return &value_z;
            return 0;
        }

        // Construct an Element describing this Object.
        bool make_element(Element& elem) const {
            elem.name = name;
            elem.props.push_back(prop_x);
            elem.props.push_back(prop_y);
            elem.props.push_back(prop_z);
            return true;
        }
        


        float x() const { double dval; value_x.get_scalar(prop_x, dval); return (float)dval; }


        float y() const { double dval; value_y.get_scalar(prop_y, dval); return (float)dval; }


        float z() const { double dval; value_z.get_scalar(prop_z, dval); return (float)dval; }


        void x(float coord) { value_x.set_scalar(prop_x, coord); }
        

        void y(float coord) { value_y.set_scalar(prop_y, coord); }
        

        void z(float coord) { value_z.set_scalar(prop_z, coord); }
        static const char* name;

        static const Property prop_x;
        static const Property prop_y;
        static const Property prop_z;
    }; // struct Vertex


    struct Face: public Object {
        AnyValue indices;   

        Face() {}

        Face(const size_t& size) { indices.set_size(prop_ind, size); }

        Face(const Face& f) {
            if (f.indices.data != 0) {
                size_t size;
                double val;
                f.indices.get_size(prop_ind, size);
                indices.set_size(prop_ind, size);
                for (size_t n = 0; n < size; ++n) {
                    f.indices.get_item(prop_ind, n, val);
                    indices.set_item(prop_ind, n, val);
                }
            }
        }

        // Get a Value.
        Value* get_value(const Element& elem, const Property& prop) {
            if (prop.name.compare(prop_ind.name.c_str()) == 0) return &indices;
            return 0;
        }

        // Construct an Element describing this Object.
        bool make_element(Element& elem) const {
            elem.name = name;
            elem.props.push_back(prop_ind);
            return true;
        }



        size_t size() const { size_t s; indices.get_size(prop_ind, s); return s; }


        size_t vertex(const size_t& num) const { double index; indices.get_item(prop_ind, num, index); return (size_t)index; }


        void size(const size_t& size) { indices.set_size(prop_ind, size); }


        void vertex(const size_t& num, const size_t& index) { indices.set_item(prop_ind, num, (double)index); }
        static const char* name;

        static const Property prop_ind;
    }; // struct Face


    struct VertexArray: public AnyArray {
        VertexArray(): AnyArray() {}
    
        // Get the next Object.
        Object& next_object() {
            if (objects[incr] == 0)
                objects[incr] = new Vertex;
            return *objects[incr++];
        }
    }; // struct VertexArray


    struct FaceArray: public AnyArray {
        FaceArray(): AnyArray() {}
    
        // Get the next Object.
        Object& next_object() {
            if (objects[incr] == 0)
                objects[incr] = new Face;
            return *objects[incr++];
        }
    }; // struct FaceArray
    


    struct VertexExternal: public Array {
        std::vector<Vertex>& vertices;  
        size_t incr;                    


        VertexExternal(std::vector<Vertex>& v): Array(), vertices(v), incr(0) {}

        size_t size() { return vertices.size(); }
        void prepare(const size_t& size) { vertices.reserve(size); restart(); }
        void clear() { vertices.clear(); }
        void restart() { incr = 0; }

        // Get the next Object.
        Object& next_object() {
            if (vertices.size() <= incr)
                vertices.resize(incr+1);
            return vertices[incr++];
        }
    }; // struct VertexExternal
    


    struct FaceExternal: public Array {
        std::vector<Face>& faces;   
        size_t incr;                


        FaceExternal(std::vector<Face>& f): Array(), faces(f), incr(0) {}

        size_t size() { return faces.size(); }
        void prepare(const size_t& size) { faces.reserve(size); restart(); }
        void clear() { faces.clear(); }
        void restart() { incr = 0; }

        // Get the next Object.
        Object& next_object() {
            if (faces.size() <= incr)
                faces.resize(incr+1);
            return faces[incr++];
        }
    }; // struct FaceExternal
} // namespace PLY


#endif // __PLY_EXAMPLES_H__
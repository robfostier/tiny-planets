#ifndef VECTOR_DATA_HPP
#define VECTOR_DATA_HPP

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

struct VertexData {
    Vector3 position;
    Vector3 normal;
    Vector2 uv;
};

#endif
#ifndef PLATE_TECTONIC_HPP
#define PLATE_TECTONIC_HPP

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

enum PlateType { OCEANIC, CONTINENTAL };

struct Plate {
    uint32_t id;
    Vector3 center;
    Vector3 velocity;
    PlateType type;
};

#endif
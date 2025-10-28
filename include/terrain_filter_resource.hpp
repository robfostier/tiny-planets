#ifndef TERRAIN_FILTER_RESOURCE_HPP
#define TERRAIN_FILTER_RESOURCE_HPP

#include <vector>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include "vertex_data.hpp"

class TerrainFilterResource : public Resource
{
    GDCLASS(TerrainFilterResource, Resource);

protected:
    static void _bind_methods();

public:
    virtual ~TerrainFilterResource() = default;

    virtual void apply(std::vector<VertexData> &vertices, std::vector<int> &indices) = 0;
};


#endif
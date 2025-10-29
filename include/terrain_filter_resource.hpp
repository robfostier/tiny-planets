#ifndef TERRAIN_FILTER_RESOURCE_HPP
#define TERRAIN_FILTER_RESOURCE_HPP

#include "planet_vertex.hpp"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <vector>

class TerrainFilterResource : public Resource {
    GDCLASS(TerrainFilterResource, Resource);

  protected:
    bool b_is_active = true;
    static void _bind_methods();

  public:
    virtual ~TerrainFilterResource() = default;

    void set_activity(const bool &activity);
    bool get_activity(void) const { return b_is_active; }

    virtual void apply(std::vector<PlanetVertex> &vertices,
                       std::vector<int> &indices) = 0;
};

#endif
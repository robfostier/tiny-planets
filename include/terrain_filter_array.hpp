#ifndef TERRAIN_FILTER_ARRAY_HPP
#define TERRAIN_FILTER_ARRAY_HPP

#include "terrain_filter_resource.hpp"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <vector>

class TerrainFilterArray : public Resource {
    GDCLASS(TerrainFilterArray, Resource);

  public:
    Array filters;

  protected:
    static void _bind_methods();

  public:
    void set_filters(const Array &arr);
    Array get_filters() const { return filters; }

    void add_filter(Ref<TerrainFilterResource> f);

    int size() const { return filters.size(); }

    void connect_filter_signals(void);
};

#endif

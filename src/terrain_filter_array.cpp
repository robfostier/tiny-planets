#include "terrain_filter_array.hpp"

using namespace godot;

void TerrainFilterArray::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_filters", "arr"), &TerrainFilterArray::set_filters);
    ClassDB::bind_method(D_METHOD("get_filters"), &TerrainFilterArray::get_filters);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "filters", PROPERTY_HINT_RESOURCE_TYPE, "TerrainFilterResource"), "set_filters", "get_filters");

    ClassDB::bind_method(D_METHOD("add_filter", "filter"), &TerrainFilterArray::add_filter);
}

void TerrainFilterArray::set_filters(const Array &arr) {
    filters = arr;
    connect_filter_signals();
    emit_changed();
}

void TerrainFilterArray::add_filter(Ref<TerrainFilterResource> f) {
    if (!f.is_valid())
        return;

    filters.push_back(f);

    if (!f->is_connected("changed", Callable(this, "emit_signal").bind("changed")))
        f->connect("changed", Callable(this, "emit_signal").bind("changed"));

    emit_changed();
}

void TerrainFilterArray::connect_filter_signals() {
    for (int i = 0; i < filters.size(); i++) {
        Ref<TerrainFilterResource> f = filters[i];
        if (f.is_valid() && !f->is_connected("changed", Callable(this, "emit_signal").bind("changed")))
            f->connect("changed", Callable(this, "emit_signal").bind("changed"));
    }
}
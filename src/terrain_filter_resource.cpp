#include "terrain_filter_resource.hpp"

void TerrainFilterResource::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_activity", "value"), &TerrainFilterResource::set_activity);
    ClassDB::bind_method(D_METHOD("get_activity"), &TerrainFilterResource::get_activity);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_active"), "set_activity", "get_activity");
}

void TerrainFilterResource::set_activity(const bool &activity) {
    b_is_active = activity;
    emit_changed();
}

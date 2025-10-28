#include "planet.hpp"

void Planet::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_radius", "radius"), &Planet::set_radius);
    ClassDB::bind_method(D_METHOD("get_radius"), &Planet::get_radius);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radius"), "set_radius", "get_radius");

    ClassDB::bind_method(D_METHOD("set_resolution", "resolution"), &Planet::set_resolution);
    ClassDB::bind_method(D_METHOD("get_resolution"), &Planet::get_resolution);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "resolution", PROPERTY_HINT_RANGE, "2,256,1"), "set_resolution", "get_resolution");

    ClassDB::bind_method( D_METHOD("set_terrain_filter_array", "terrain_filter_array"), &Planet::set_terrain_filter_array);
    ClassDB::bind_method(D_METHOD("get_terrain_filter_array"), &Planet::get_terrain_filter_array);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "terrain_filter_array", PROPERTY_HINT_RESOURCE_TYPE, "TerrainFilterArray"), "set_terrain_filter_array", "get_terrain_filter_array");

    ClassDB::bind_method(D_METHOD("set_material", "material"), &Planet::set_material);
    ClassDB::bind_method(D_METHOD("get_material"), &Planet::get_material);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "material", PROPERTY_HINT_RESOURCE_TYPE, "StandardMaterial3D"), "set_material", "get_material");

    ClassDB::bind_method(D_METHOD("generate"), &Planet::generate);
}

Planet::Planet() {
    mesh.instantiate();
    mesh_instance = memnew(godot::MeshInstance3D);
    mesh_instance->set_mesh(mesh);
    add_child(mesh_instance);
}

void Planet::_notification(int what) {
    switch (what) {
    case NOTIFICATION_READY: {
        generate();
        break;
    }
    default:
        break;
    }
}

void Planet::set_radius(float r) {
    radius = r;
    generate();
}

void Planet::set_resolution(int r) {
    resolution = MAX(2, r);
    generate();
}

void Planet::set_terrain_filter_array(const Ref<TerrainFilterArray> &arr) {
    if (terrain_filters_array.is_valid()) {
        terrain_filters_array->disconnect("changed", Callable(this, "generate"));
    }

    terrain_filters_array = arr;

    if (terrain_filters_array.is_valid()) {
        terrain_filters_array->connect("changed", Callable(this, "generate"));

        terrain_filters_array->connect_filter_signals();
    }

    generate();
}

void Planet::set_material(const Ref<StandardMaterial3D> &mat) {
    material = mat;
    if (mesh_instance && material.is_valid() && mesh->get_surface_count() > 0)
        mesh->surface_set_material(0, material);
}

void Planet::generate() {
    CubeSphereGenerator::generate(radius, resolution, vertices, indices);

    // Apply terrain filters
    if (terrain_filters_array.is_valid()) {
        Array arr = terrain_filters_array->get_filters();
        for (int i = 0; i < arr.size(); i++) {
            Ref<TerrainFilterResource> f = arr[i];
            if (f.is_valid() && f->get_activity() == true)
                f->apply(vertices, indices);
        }
    }

    // Generate MeshArray for Godot
    PackedVector3Array vertices_array;
    PackedInt32Array indices_array;
    PackedVector2Array uvs_array;
    PackedVector3Array normals_array;

    for (auto &v : vertices) {
        vertices_array.push_back(v.position);
        uvs_array.push_back(v.uv);
        normals_array.push_back(v.normal);
    }
    for (auto &i : indices)
        indices_array.push_back(i);

    PackedColorArray colors_array;
    colors_array.resize(vertices.size());

    for (int i = 0; i < vertices.size(); i++) {
        int plate_id = vertices[i].plate_id;
        
        float r = Math::fmod(Math::sin(plate_id * 12.9898f) * 43758.5453f, 1.0f);
        float g = Math::fmod(Math::sin(plate_id * 78.233f) * 96234.5453f, 1.0f);
        float b = Math::fmod(Math::sin(plate_id * 45.164f) * 12345.6789f, 1.0f);

        colors_array[i] = Color(r, g, b);
    }

    Array meshArrays;
    meshArrays.resize(Mesh::ARRAY_MAX);
    meshArrays[Mesh::ARRAY_VERTEX] = vertices_array;
    meshArrays[Mesh::ARRAY_INDEX] = indices_array;
    meshArrays[Mesh::ARRAY_TEX_UV] = uvs_array;
    meshArrays[Mesh::ARRAY_NORMAL] = normals_array;
    meshArrays[Mesh::ARRAY_COLOR] = colors_array;

    if (mesh.is_null()) {
        mesh.instantiate();
        mesh_instance = memnew(MeshInstance3D);
        mesh_instance->set_mesh(mesh);
        add_child(mesh_instance);
    }

    mesh->clear_surfaces();
    mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, meshArrays);

    if (material.is_valid())
        mesh->surface_set_material(0, material);
}
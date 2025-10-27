#include "planet.hpp"

void Planet::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_radius", "radius"), &Planet::set_radius);
    ClassDB::bind_method(D_METHOD("get_radius"), &Planet::get_radius);
    ClassDB::bind_method(D_METHOD("set_resolution", "resolution"), &Planet::set_resolution);
    ClassDB::bind_method(D_METHOD("get_resolution"), &Planet::get_resolution);
    ClassDB::bind_method(D_METHOD("set_material", "material"), &Planet::set_material);
    ClassDB::bind_method(D_METHOD("get_material"), &Planet::get_material);
    ClassDB::bind_method(D_METHOD("set_filters", "filters"), &Planet::set_filters);
    ClassDB::bind_method(D_METHOD("get_filters"), &Planet::get_filters);
    ClassDB::bind_method(D_METHOD("generate"), &Planet::generate);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radius"), "set_radius", "get_radius");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "resolution", PROPERTY_HINT_RANGE, "2,256,1"), "set_resolution", "get_resolution");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "filters"), "set_filters", "get_filters");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "material", PROPERTY_HINT_RESOURCE_TYPE, "StandardMaterial3D"), "set_material", "get_material");
}

Planet::Planet()
{
    mesh.instantiate();
    mesh_instance = memnew(godot::MeshInstance3D);
    mesh_instance->set_mesh(mesh);
    add_child(mesh_instance);
}

void Planet::_notification(int what) {
    switch (what) {
        case NOTIFICATION_READY:
        {
            generate();
            break;
        }
        default: break;
    }
}

void Planet::set_radius(float r)
{
    radius = r;
    generate();
}

void Planet::set_resolution(int r)
{
    resolution = MAX(2, r);
    generate();
}

void Planet::set_material(const Ref<StandardMaterial3D>& mat)
{
    material = mat;
    if (mesh_instance && material.is_valid() && mesh->get_surface_count() > 0) {
        mesh->surface_set_material(0, material);
    }
}

void Planet::set_filters(const Array &arr) {
    terrain_filters.clear();
    for (int i = 0; i < arr.size(); i++) {
        Ref<TerrainFilterResource> f = arr[i];
        if (f.is_valid()) terrain_filters.push_back(f);
    }
}

Array Planet::get_filters() const {
    Array arr;
    for (auto &f : terrain_filters) arr.append(f);
    return arr;
}

void Planet::generate()
{
    CubeSphereGenerator::generate(radius, resolution, vertices, indices);

    for (auto &f : terrain_filters) {
        f->apply(vertices);
    }

    PackedVector3Array vertices_array;
    PackedInt32Array indices_array;
    PackedVector2Array uvs_array;
    PackedVector3Array normals_array;

    for (auto &v : vertices)
    {
        vertices_array.push_back(v.position);
        uvs_array.push_back(v.uv);
        normals_array.push_back(v.normal);
    }
    for (auto &i : indices) indices_array.push_back(i);

    Array meshArrays;
    meshArrays.resize(Mesh::ARRAY_MAX);
    meshArrays[Mesh::ARRAY_VERTEX] = vertices_array;
    meshArrays[Mesh::ARRAY_INDEX] = indices_array;
    meshArrays[Mesh::ARRAY_TEX_UV] = uvs_array;
    meshArrays[Mesh::ARRAY_NORMAL] = normals_array;

    if (mesh.is_null()) {
        mesh.instantiate();
        mesh_instance = memnew(MeshInstance3D);
        mesh_instance->set_mesh(mesh);
        add_child(mesh_instance);
    }

    mesh->clear_surfaces();
    mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, meshArrays);

    if (material.is_valid()) mesh->surface_set_material(0, material);
}
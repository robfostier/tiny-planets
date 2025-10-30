#include "planet.hpp"
#include <algorithm>

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

    compute_neighbours();

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

    vertices_array.resize(vertices.size());
    uvs_array.resize(vertices.size());
    normals_array.resize(vertices.size());

    for (size_t i = 0; i < vertices.size(); ++i) {
        vertices_array[i] = vertices[i].get_position();
        uvs_array[i] = vertices[i].get_uv();
        normals_array[i] = vertices[i].get_normal();
    }

    indices_array.resize(indices.size());
    for (size_t i = 0; i < indices.size(); ++i)
        indices_array[i] = indices[i];

    PackedColorArray colors_array;
    colors_array.resize(vertices.size());

    // VERTEX CONTINENTALNESS VIZ
    // for (size_t i = 0; i < vertices.size(); ++i) {
    //     auto plate_ptr = vertices[i].get_plate();
    //     if (plate_ptr) {
    //         float continentalness = vertices[i].get_continentalness();
    //         colors_array[i] = Color(continentalness, continentalness, continentalness);
    //     } else {
    //         colors_array[i] = Color(0.0f, 1.0f, 0.0f);
    //     }
    // }

    // PLATE ID VIZ
    // for (size_t i = 0; i < vertices.size(); ++i) {
    //     auto plate_ptr = vertices[i].get_plate();
    //     if (plate_ptr) {
    //         int plate_id = plate_ptr->id;
    //         float r = Math::fmod(Math::sin(plate_id * 12.9898f) * 43758.5453f, 1.0f);
    //         float g = Math::fmod(Math::sin(plate_id * 78.233f) * 96234.5453f, 1.0f);
    //         float b = Math::fmod(Math::sin(plate_id * 45.164f) * 12345.6789f, 1.0f);
    //         colors_array[i] = Color(r, g, b);
    //     } else {
    //         colors_array[i] = Color(0.5f, 0.5f, 0.5f);
    //     }
    // }

    // ELEVATION VIZ
    for (size_t i = 0; i < vertices.size(); ++i) {
        float dist = vertices[i].get_position().length();

        // Normalise grossièrement la distance autour du rayon moyen
        float t = std::clamp((dist - radius * 0.98f) / (radius * 0.04f), 0.0f, 1.0f);

        Color color;
        if (t < 0.3f) {
            // Océan → sable
            color = Color(0.0f + t, 0.2f + t * 0.5f, 0.5f);
        } else if (t < 0.6f) {
            // Sable → terre
            color = Color(0.6f, 0.45f + (t - 0.3f) * 0.3f, 0.2f);
        } else if (t < 0.9f) {
            // Terre → pierre
            color = Color(0.4f + (t - 0.6f) * 0.3f, 0.4f + (t - 0.6f) * 0.3f, 0.4f);
        } else {
            // Neige
            color = Color(0.9f, 0.9f, 0.95f);
        }

        colors_array[i] = color;
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

void Planet::compute_neighbours() {
    const float epsilon = 1e-6f; // vertex fusion tolerance

    // add every neighbour
    for (size_t i = 0; i < indices.size(); i += 3) {
        int a = indices[i];
        int b = indices[i + 1];
        int c = indices[i + 2];

        vertices[a].add_neighbour(&vertices[b]);
        vertices[a].add_neighbour(&vertices[c]);

        vertices[b].add_neighbour(&vertices[a]);
        vertices[b].add_neighbour(&vertices[c]);

        vertices[c].add_neighbour(&vertices[a]);
        vertices[c].add_neighbour(&vertices[b]);
    }

    // add neighbours on unwelded wedges EXPENSIIIIIIVE O(n²)
    for (size_t i = 0; i < vertices.size(); ++i) {
        PlanetVertex &v1 = vertices[i];
        for (size_t j = i + 1; j < vertices.size(); ++j) {
            PlanetVertex &v2 = vertices[j];
            if ((v1.get_position() - v2.get_position()).length_squared() < epsilon * epsilon) {
                v1.add_neighbour(&v2);
                v2.add_neighbour(&v1);
            }
        }
    }

    // rm dupes (vector -> set -> vector)
    for (auto &vertex : vertices) {
        std::unordered_set<PlanetVertex*> unique_set(vertex.get_neighbours().begin(), vertex.get_neighbours().end());
        std::vector<PlanetVertex*> neighbours(unique_set.begin(), unique_set.end());
        vertex.set_neighbours(neighbours);
    }
}
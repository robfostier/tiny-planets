#include "planet.hpp"

void Planet::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_radius", "radius"), &Planet::set_radius);
    ClassDB::bind_method(D_METHOD("get_radius"), &Planet::get_radius);
    ClassDB::bind_method(D_METHOD("set_resolution", "resolution"), &Planet::set_resolution);
    ClassDB::bind_method(D_METHOD("get_resolution"), &Planet::get_resolution);
    ClassDB::bind_method(D_METHOD("set_material", "material"), &Planet::set_material);
    ClassDB::bind_method(D_METHOD("get_material"), &Planet::get_material);
    ClassDB::bind_method(D_METHOD("get_mesh"), &Planet::get_mesh);
    ClassDB::bind_method(D_METHOD("generate"), &Planet::generate);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radius"), "set_radius", "get_radius");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "resolution"), "set_resolution", "get_resolution");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "material", PROPERTY_HINT_RESOURCE_TYPE, "StandardMaterial3D"), "set_material", "get_material");
}

Planet::Planet()
{
    mesh.instantiate();
    mesh_instance = memnew(godot::MeshInstance3D);
    mesh_instance->set_mesh(mesh);
    add_child(mesh_instance);
}

void Planet::set_radius(float r)
{
    radius = r;
    generate();
}

void Planet::set_resolution(int r)
{
    resolution = r;
    generate();
}

void Planet::set_material(const Ref<StandardMaterial3D>& mat)
{
    material = mat;
    if (mesh_instance && material.is_valid() && mesh->get_surface_count() > 0) {
        mesh->surface_set_material(0, material);
    }
}

void Planet::generate()
{
    PackedVector3Array vertices;
    PackedInt32Array indices;
    PackedVector2Array uvs;
    PackedVector3Array normals;

    for (int face = 0; face < 6; face++)
    {
        Vector3 localUp;
        switch (face)
        {
            case 0: localUp = Vector3(0, 1, 0); break;
            case 1: localUp = Vector3(0, -1, 0); break;
            case 2: localUp = Vector3(1, 0, 0); break;
            case 3: localUp = Vector3(-1, 0, 0); break;
            case 4: localUp = Vector3(0, 0, 1); break;
            case 5: localUp = Vector3(0, 0, -1); break;
        }

        Vector3 axisA = Vector3(localUp.y, localUp.z, localUp.x);
        Vector3 axisB = localUp.cross(axisA);

        for (int y = 0 ; y < resolution; y++)
        {
            for (int x = 0; x < resolution ; x++)
            {
                Vector2 percent(float(x) / (resolution - 1), float(y) / (resolution - 1));
                Vector3 positionOnCube = localUp + (percent.x - .5f) * 2 * axisA + (percent.y - .5f) * 2 * axisB;
                Vector3 positionOnSphere = positionOnCube.normalized() * radius;

                vertices.push_back(positionOnSphere);
                uvs.push_back(percent);
            
                if (x != resolution - 1 && y != resolution - 1)
                {
                    int faceOffset = face * resolution * resolution;
                    int i = x + y * resolution + faceOffset;
                    indices.push_back(i);
                    indices.push_back(i + resolution);
                    indices.push_back(i + resolution + 1);
                    indices.push_back(i);
                    indices.push_back(i + resolution + 1);
                    indices.push_back(i + 1);
                }
            }
        }
    }

    Array meshArrays;
    meshArrays.resize(Mesh::ARRAY_MAX);
    meshArrays[Mesh::ARRAY_VERTEX] = vertices;
    meshArrays[Mesh::ARRAY_INDEX] = indices;
    meshArrays[Mesh::ARRAY_TEX_UV] = uvs;

    normals.resize(vertices.size());
    meshArrays[Mesh::ARRAY_NORMAL] = normals;

    mesh->clear_surfaces();
    mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, meshArrays);
    mesh->regen_normal_maps();

    if (material.is_valid()) mesh->surface_set_material(0, material);
}
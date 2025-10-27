#ifndef PLANET_HPP
#define PLANET_HPP

#include <vector>
#include <memory>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/core/class_db.hpp>

#include "vertex_data.hpp"
#include "terrain_filter_resource.hpp"
#include "cube_sphere_generator.hpp"

using namespace godot;

class Planet : public Node3D
{
    GDCLASS(Planet, Node3D);

private:
    Ref<ArrayMesh> mesh;
    MeshInstance3D* mesh_instance = nullptr;
    Ref<StandardMaterial3D> material;

    std::vector<VertexData> vertices;
    std::vector<int> indices;
    std::vector<Ref<TerrainFilterResource>> terrain_filters;

    float radius = 1.0f;
    int resolution = 10;

protected:
    static void _bind_methods();

public:
    Planet();

    void _notification(int what);

    void set_radius(float r);
    float get_radius(void) const { return radius; }
    void set_resolution(int r);
    int get_resolution(void) const { return resolution; }
    void Planet::set_material(const Ref<StandardMaterial3D>& mat);
    Ref<StandardMaterial3D> Planet::get_material() const { return material; }

    void Planet::set_filters(const Array &arr);
    Array Planet::get_filters() const;

    void generate();
};

#endif
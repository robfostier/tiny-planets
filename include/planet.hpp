#ifndef PLANET_HPP
#define PLANET_HPP

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <memory>
#include <vector>

#include "cube_sphere_generator.hpp"
#include "terrain_filter_array.hpp"
#include "vertex_data.hpp"

using namespace godot;

class Planet : public Node3D {
    GDCLASS(Planet, Node3D);

  private:
    Ref<ArrayMesh> mesh;
    MeshInstance3D *mesh_instance = nullptr;

    std::vector<VertexData> vertices;
    std::vector<int> indices;

    float radius = 1.0f;
    int resolution = 10;

    Ref<TerrainFilterArray> terrain_filters_array;

    Ref<StandardMaterial3D> material;

  protected:
    static void _bind_methods();

  public:
    Planet();

    void _notification(int what);

    void set_radius(float r);
    float get_radius(void) const { return radius; }

    void set_resolution(int r);
    int get_resolution(void) const { return resolution; }

    void set_terrain_filter_array(const Ref<TerrainFilterArray> &arr);
    Ref<TerrainFilterArray> get_terrain_filter_array() const {
        return terrain_filters_array;
    }

    void set_material(const Ref<StandardMaterial3D> &mat);
    Ref<StandardMaterial3D> get_material() const { return material; }

    void generate();
};

#endif
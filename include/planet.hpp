#ifndef PLANET_HPP
#define PLANET_HPP

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class Planet : public Node3D
{
    GDCLASS(Planet, Node3D);

private:
    Ref<ArrayMesh> mesh;
    MeshInstance3D* mesh_instance = nullptr;

    float radius = 1.0f;
    int resolution = 10;
    Ref<StandardMaterial3D> material;

protected:
    static void _bind_methods();

public:
    Planet();

    void _ready() override;

    void set_radius(float r);
    float get_radius(void) const { return radius; }
    void set_resolution(int r);
    int get_resolution(void) const { return resolution; }
    Ref<ArrayMesh> get_mesh(void) const { return mesh; }
    void Planet::set_material(const Ref<StandardMaterial3D>& mat);
    Ref<StandardMaterial3D> Planet::get_material() const { return material; }

    void generate();
};

#endif
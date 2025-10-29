#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class Vertex {
  protected:
    Vector3 position;
    Vector3 normal;
    Vector2 uv;

  public:
    Vertex() = default;
    Vertex(const Vector3 &position, const Vector3 &normal, const Vector2 &uv)
        : position(position), normal(normal), uv(uv) {}

    void set_position(const Vector3 &value) { position = value; }
    Vector3 get_position(void) const { return position; }
    void set_normal(const Vector3 &value) { normal = value; }
    Vector3 get_normal(void) const { return normal; }
    void set_uv(const Vector2 &value) { uv = value; }
    Vector2 get_uv(void) const { return uv; }
};

#endif
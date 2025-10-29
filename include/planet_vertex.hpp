#ifndef PLANET_VERTEX_HPP
#define PLANET_VERTEX_HPP

#include "tectonic_plate.hpp"
#include "vertex.hpp"
#include <memory>

class PlanetVertex : public Vertex {
  private:
    std::weak_ptr<Plate> plate;

  public:
    PlanetVertex(const Vector3 &position, const Vector3 &normal,
                 const Vector2 &uv)
        : Vertex(position, normal, uv) {}

    PlanetVertex(const Vector3 &position, const Vector3 &normal,
                 const Vector2 &uv, std::weak_ptr<Plate> plate)
        : Vertex(position, normal, uv), plate(std::move(plate)) {}

    void set_plate(const std::shared_ptr<Plate> &new_plate) { plate = new_plate; }
    std::shared_ptr<Plate> get_plate() const { return plate.lock(); }
};

#endif
#ifndef PLANET_VERTEX_HPP
#define PLANET_VERTEX_HPP

#include "tectonic_plate.hpp"
#include "vertex.hpp"
#include <memory>
#include <vector>

class PlanetVertex : public Vertex {
  private:
    std::weak_ptr<Plate> plate;
    float continentalness = 0.0f; // distance to closest ocean, 0 for oceans 1 for inland

    std::vector<PlanetVertex*> neighbours;

  public:
    PlanetVertex(const Vector3 &position, const Vector3 &normal,
                 const Vector2 &uv)
        : Vertex(position, normal, uv) {}

    PlanetVertex(const Vector3 &position, const Vector3 &normal,
                 const Vector2 &uv, std::weak_ptr<Plate> plate,
                 float continentalness)
        : Vertex(position, normal, uv), plate(std::move(plate)),
          continentalness(continentalness) {}

    void set_plate(const std::shared_ptr<Plate> &new_plate) {
        plate = new_plate;
    }
    std::shared_ptr<Plate> get_plate() const { return plate.lock(); }

    void set_continentalness(float value) { continentalness = value; }
    float get_continentalness() const { return continentalness; }

    void set_neighbours(const std::vector<PlanetVertex *> &new_neighbours) { neighbours = new_neighbours; }
    const std::vector<PlanetVertex*>& get_neighbours() const { return neighbours; }

    void add_neighbour(PlanetVertex* neighbour) { neighbours.push_back(neighbour); }

    Vector3 get_velocity() const {
        auto plate_ptr = plate.lock();
        if (!plate_ptr)
            return Vector3(0, 0, 0);
        return plate_ptr->angular_velocity.cross(get_position());
    }
};

#endif
#ifndef PLATE_TECTONIC_FILTER_HPP
#define PLATE_TECTONIC_FILTER_HPP

#include "fastnoiselite/FastNoiseLite.h"
#include "terrain_filter_resource.hpp"
#include "vertex_data.hpp"
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <vector>

class PlateTectonicFilter : public TerrainFilterResource {
    GDCLASS(PlateTectonicFilter, TerrainFilterResource);

  private:
    Ref<RandomNumberGenerator> rng;
    uint32_t seed = 0;

    int plate_count = 12;
    float plate_jitter_strength = 0.0f;
    FastNoiseLite plate_noise;
    float plate_noise_amplitude = 0.01f;
    float plate_noise_frequency = 0.01f;
    int plate_noise_octaves = 1;

    float elevation_scale = 1.0f;
    float divergence_strength = 1.0f;
    float convergence_strength = 1.0f;

    std::vector<Vector3> plate_centers;
    std::vector<Vector3> plate_velocities;

    Vector3 random_unit_vector();

  protected:
    static void _bind_methods();

  public:
    PlateTectonicFilter();
    
    void set_seed(uint32_t value);
    uint32_t get_seed() const { return seed; }

    void set_plate_count(int value);
    int get_plate_count() const { return plate_count; }
    void set_plate_jitter_strength(float value);
    float get_plate_jitter_strength() const { return plate_jitter_strength; }
    void set_plate_noise_amplitude(float value);
    float get_plate_noise_amplitude() const { return plate_noise_amplitude; }
    void set_plate_noise_frequency(float value);
    float get_plate_noise_frequency() const { return plate_noise_frequency; }
    void set_plate_noise_octaves(int value);
    int get_plate_noise_octaves() const { return plate_noise_octaves; }

    void set_elevation_scale(float value);
    float get_elevation_scale() const { return elevation_scale; }
    void set_divergence_strength(float value);
    float get_divergence_strength() const { return divergence_strength; }
    void set_convergence_strength(float value);
    float get_convergence_strength() const { return convergence_strength; }

    virtual void apply(std::vector<VertexData> &vertices,
                       std::vector<int> &indices) override;
};

#endif
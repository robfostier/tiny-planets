#ifndef PLATE_TECTONIC_FILTER_HPP
#define PLATE_TECTONIC_FILTER_HPP

#include "fastnoiselite/FastNoiseLite.h"
#include "terrain_filter_resource.hpp"
#include "tectonic_plate.hpp"
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <memory>
#include <vector>
#include <unordered_set>
#include <cmath>
#include <algorithm>



class PlateTectonicFilter : public TerrainFilterResource {
    GDCLASS(PlateTectonicFilter, TerrainFilterResource);

  private:
    Ref<RandomNumberGenerator> rng;
    uint32_t seed = 0;

    int plate_count = 12;
    FastNoiseLite plate_noise;
    float plate_noise_amplitude = 0.01f;
    float plate_noise_frequency = 0.01f;
    int plate_noise_octaves = 1;

    float continental_plate_ratio = 0.5f;

    float continentalness_threshold = 0.0f;

    float elevation_scale = 1.0f;
    float divergence_strength = 1.0f;
    float convergence_strength = 1.0f;

    // This filter stores shared pointers, and each PlanetVertex stores a weak ptr to its plate
    std::vector<std::shared_ptr<Plate>> plates;

    Vector3 random_unit_vector();

  protected:
    static void _bind_methods();

  public:
    PlateTectonicFilter();
    
    void set_seed(uint32_t value);
    uint32_t get_seed() const { return seed; }

    void set_plate_count(int value);
    int get_plate_count() const { return plate_count; }
    void set_plate_noise_amplitude(float value);
    float get_plate_noise_amplitude() const { return plate_noise_amplitude; }
    void set_plate_noise_frequency(float value);
    float get_plate_noise_frequency() const { return plate_noise_frequency; }
    void set_plate_noise_octaves(int value);
    int get_plate_noise_octaves() const { return plate_noise_octaves; }

    void set_continental_plate_ratio(float value);
    float get_continental_plate_ratio() const { return continental_plate_ratio; }

    void set_continentalness_threshold(float value);
    float get_continentalness_threshold() const { return continentalness_threshold; }

    void set_elevation_scale(float value);
    float get_elevation_scale() const { return elevation_scale; }
    void set_divergence_strength(float value);
    float get_divergence_strength() const { return divergence_strength; }
    void set_convergence_strength(float value);
    float get_convergence_strength() const { return convergence_strength; }

    virtual void apply(std::vector<PlanetVertex> &vertices,
                       std::vector<int> &indices) override;

    std::shared_ptr<Plate> find_closest_plate(const PlanetVertex &vertex);
};

#endif
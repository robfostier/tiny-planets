#ifndef SIMPLEX_NOISE_FILTER_HPP
#define SIMPLEX_NOISE_FILTER_HPP

#include "fastnoiselite/FastNoiseLite.h"
#include "terrain_filter_resource.hpp"
#include "vertex_data.hpp"
#include <godot_cpp/classes/resource.hpp>
#include <vector>

class SimplexNoiseFilter : public TerrainFilterResource {
    GDCLASS(SimplexNoiseFilter, TerrainFilterResource);

  private:
    float amplitude = 1.0f;
    float frequency = 1.0f;
    int octaves = 1;

  protected:
    static void _bind_methods();

  public:
    void set_amplitude(float value);
    float get_amplitude() const { return amplitude; }
    void set_frequency(float value);
    float get_frequency() const { return frequency; }
    void set_octaves(int value);
    float get_octaves() const { return octaves; }

    virtual void apply(std::vector<VertexData> &vertices,
                       std::vector<int> &indices) override;
};

#endif

#include "simplex_noise_filter.hpp"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void SimplexNoiseFilter::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_amplitude", "value"), &SimplexNoiseFilter::set_amplitude);
    ClassDB::bind_method(D_METHOD("get_amplitude"), &SimplexNoiseFilter::get_amplitude);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "amplitude"), "set_amplitude", "get_amplitude");

    ClassDB::bind_method(D_METHOD("set_frequency", "value"), &SimplexNoiseFilter::set_frequency);
    ClassDB::bind_method(D_METHOD("get_frequency"), &SimplexNoiseFilter::get_frequency);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "frequency"), "set_frequency", "get_frequency");
}

void SimplexNoiseFilter::set_amplitude(float value)
{
    amplitude = value;
    emit_changed();
}

void SimplexNoiseFilter::set_frequency(float value)
{
    frequency = value;
    emit_changed();
}

void SimplexNoiseFilter::apply(std::vector<VertexData> &vertices, std::vector<int> &indices) {
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(frequency);

    for (auto& v : vertices) {
        float n = noise.GetNoise(v.position.x, v.position.y, v.position.z);
        v.position += v.normal * n * amplitude;
        v.normal = v.position.normalized();
    }
}
#include "simplex_noise_filter.hpp"

#include <cmath>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void SimplexNoiseFilter::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_amplitude", "value"), &SimplexNoiseFilter::set_amplitude);
    ClassDB::bind_method(D_METHOD("get_amplitude"), &SimplexNoiseFilter::get_amplitude);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "amplitude"), "set_amplitude", "get_amplitude");

    ClassDB::bind_method(D_METHOD("set_frequency", "value"), &SimplexNoiseFilter::set_frequency);
    ClassDB::bind_method(D_METHOD("get_frequency"), &SimplexNoiseFilter::get_frequency);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "frequency"), "set_frequency", "get_frequency");

    ClassDB::bind_method(D_METHOD("set_octaves", "value"), &SimplexNoiseFilter::set_octaves);
    ClassDB::bind_method(D_METHOD("get_octaves"), &SimplexNoiseFilter::get_octaves);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "octaves", PROPERTY_HINT_RANGE, "1,8,1"), "set_octaves", "get_octaves");
}

void SimplexNoiseFilter::set_amplitude(float value) {
    amplitude = value;
    emit_changed();
}

void SimplexNoiseFilter::set_frequency(float value) {
    frequency = value;
    emit_changed();
}

void SimplexNoiseFilter::set_octaves(int value) {
    octaves = value;
    emit_changed();
}

void SimplexNoiseFilter::apply(std::vector<VertexData> &vertices,
                               std::vector<int> &indices) {
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    for (int i = 0; i < octaves; i++) {
        noise.SetFrequency(frequency * pow(2, i));

        for (auto &v : vertices) {
            float n = noise.GetNoise(v.position.x, v.position.y, v.position.z);
            v.position += v.normal * n * amplitude / pow(2, i);
            v.normal = v.position.normalized();
        }
    }
}
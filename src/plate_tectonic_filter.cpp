#include "plate_tectonic_filter.hpp"

#include <cmath>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void PlateTectonicFilter::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_seed", "value"), &PlateTectonicFilter::set_seed);
    ClassDB::bind_method(D_METHOD("get_seed"), &PlateTectonicFilter::get_seed);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");

    ClassDB::bind_method(D_METHOD("set_plate_count", "value"), &PlateTectonicFilter::set_plate_count);
    ClassDB::bind_method(D_METHOD("get_plate_count"), &PlateTectonicFilter::get_plate_count);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "plate_count", PROPERTY_HINT_RANGE, "1,32,1"), "set_plate_count", "get_plate_count");

    ClassDB::bind_method(D_METHOD("set_plate_jitter_strength", "value"), &PlateTectonicFilter::set_plate_jitter_strength);
    ClassDB::bind_method(D_METHOD("get_plate_jitter_strength"), &PlateTectonicFilter::get_plate_jitter_strength);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "plate_jitter_strength", PROPERTY_HINT_RANGE, "0,10,0.01"), "set_plate_jitter_strength", "get_plate_jitter_strength");

    ClassDB::bind_method(D_METHOD("set_plate_noise_amplitude", "value"), &PlateTectonicFilter::set_plate_noise_amplitude);
    ClassDB::bind_method(D_METHOD("get_plate_noise_amplitude"), &PlateTectonicFilter::get_plate_noise_amplitude);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "plate_noise_amplitude"), "set_plate_noise_amplitude", "get_plate_noise_amplitude");

    ClassDB::bind_method(D_METHOD("set_plate_noise_frequency", "value"), &PlateTectonicFilter::set_plate_noise_frequency);
    ClassDB::bind_method(D_METHOD("get_plate_noise_frequency"), &PlateTectonicFilter::get_plate_noise_frequency);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "plate_noise_frequency"), "set_plate_noise_frequency", "get_plate_noise_frequency");

    ClassDB::bind_method(D_METHOD("set_plate_noise_octaves", "value"), &PlateTectonicFilter::set_plate_noise_octaves);
    ClassDB::bind_method(D_METHOD("get_plate_noise_octaves"), &PlateTectonicFilter::get_plate_noise_octaves);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "octaves", PROPERTY_HINT_RANGE, "1,8,1"), "set_plate_noise_octaves", "get_plate_noise_octaves");

    ClassDB::bind_method(D_METHOD("set_elevation_scale", "value"), &PlateTectonicFilter::set_elevation_scale);
    ClassDB::bind_method(D_METHOD("get_elevation_scale"), &PlateTectonicFilter::get_elevation_scale);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "elevation_scale"), "set_elevation_scale", "get_elevation_scale");

    ClassDB::bind_method(D_METHOD("set_divergence_strength", "value"), &PlateTectonicFilter::set_divergence_strength);
    ClassDB::bind_method(D_METHOD("get_divergence_strength"), &PlateTectonicFilter::get_divergence_strength);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "divergence_strength"), "set_divergence_strength", "get_divergence_strength");
    
    ClassDB::bind_method(D_METHOD("set_convergence_strength", "value"), &PlateTectonicFilter::set_convergence_strength);
    ClassDB::bind_method(D_METHOD("get_convergence_strength"), &PlateTectonicFilter::get_convergence_strength);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "convergence_strength"), "set_convergence_strength", "get_convergence_strength");
}

PlateTectonicFilter::PlateTectonicFilter() {
    rng.instantiate();
    rng->set_seed(seed);
    plate_noise.SetSeed(seed);
    plate_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    plate_noise.SetFrequency(plate_noise_frequency);
}

void PlateTectonicFilter::set_seed(uint32_t value) {
    seed = value;
    if (rng.is_valid())
        rng->set_seed(seed);
    plate_noise.SetSeed(seed);
    emit_changed();
}

void PlateTectonicFilter::set_plate_count(int value) {
    plate_count = value;
    emit_changed();
}

void PlateTectonicFilter::set_plate_jitter_strength(float value) {
    plate_jitter_strength = Math::max(value, 0.0f);
    emit_changed();
}

void PlateTectonicFilter::set_plate_noise_amplitude(float value) {
    plate_noise_amplitude = value;
    emit_changed();
}

void PlateTectonicFilter::set_plate_noise_frequency(float value) {
    plate_noise_frequency = value;
    plate_noise.SetFrequency(value);
    emit_changed();
}

void PlateTectonicFilter::set_plate_noise_octaves(int value) {
    plate_noise_octaves = value;
    emit_changed();
}

void PlateTectonicFilter::set_elevation_scale(float value) {
    elevation_scale = value;
    emit_changed();
}

void PlateTectonicFilter::set_divergence_strength(float value) {
    divergence_strength = value;
    emit_changed();
}

void PlateTectonicFilter::set_convergence_strength(float value) {
    convergence_strength = value;
    emit_changed();
}

Vector3 PlateTectonicFilter::random_unit_vector() {
    float u = rng->randf();
    float v = rng->randf();
    float theta = 2.0f * Math_PI * u;  // longitude
    float phi = acos(2.0f * v - 1.0f); // latitude
    float x = sin(phi) * cos(theta);
    float y = sin(phi) * sin(theta);
    float z = cos(phi);
    return Vector3(x, y, z);
}

void PlateTectonicFilter::apply(std::vector<VertexData> &vertices,
                                std::vector<int> &indices) {
    rng->set_seed(seed);
    plate_centers.clear();
    plate_velocities.clear();

    // First create plate_count random points on unit sphere (plate centers)
    plate_centers.reserve(plate_count);
    float offset = 2.0f / plate_count;
    float increment = Math_PI * (3.0f - sqrt(5.0f)); // golden angle

    for (int i = 0; i < plate_count; ++i) {
        float y = ((i * offset) - 1.0f) + (offset / 2.0f);
        float r = sqrt(1.0f - y*y);
        float phi = i * increment;

        float x = cos(phi) * r;
        float z = sin(phi) * r;

        Vector3 pos(x, y, z);

        if (plate_jitter_strength > 0.0f) {
            Vector3 jitter = random_unit_vector() * plate_jitter_strength;
            pos += jitter;
            pos = pos.normalized();
        }

        plate_centers.push_back(pos);
    }

    // Then grow voronoi regions + noise
    for (auto &v : vertices) {
        Vector3 dir = v.position.normalized();
        float max_dot = -1.0f; // -1 is the minimum dot value possible (on opposite side of sphere)
        int closest_plate = 0;

        for (int i = 0; i < plate_centers.size(); i++) {
            Vector3 noised_dir = dir;

            // Store noise cumul
            float total_amp = 0.0f;
            Vector3 total_noise(0,0,0);

            for (int i = 0; i < plate_noise_octaves; i++) {
                float freq = plate_noise_frequency * pow(2.0f, i);
                float amp = 1.0f / pow(2.0f, i);

                Vector3 scaled_pos = dir * freq;

                Vector3 noise_vec(
                    plate_noise.GetNoise(scaled_pos.x, scaled_pos.y, scaled_pos.z),
                    plate_noise.GetNoise(scaled_pos.y, scaled_pos.z, scaled_pos.x),
                    plate_noise.GetNoise(scaled_pos.z, scaled_pos.x, scaled_pos.y)
                );

                total_noise += noise_vec * amp;
                total_amp += amp;
            }

            noised_dir = (dir + total_noise * plate_noise_amplitude).normalized();
            float dot = noised_dir.dot(plate_centers[i]);
            
            // if found closer plate
            if (dot > max_dot) { 
                max_dot = dot;
                closest_plate = i;
            }
        }
        v.plate_id = closest_plate;
    }

    // Calculate plate velocities

    // Compare velocity to neighbour plates and adjust height
}
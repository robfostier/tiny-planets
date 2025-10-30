#include "plate_tectonic_filter.hpp"

using namespace godot;

void PlateTectonicFilter::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_seed", "value"), &PlateTectonicFilter::set_seed);
    ClassDB::bind_method(D_METHOD("get_seed"), &PlateTectonicFilter::get_seed);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");

    ClassDB::bind_method(D_METHOD("set_plate_count", "value"), &PlateTectonicFilter::set_plate_count);
    ClassDB::bind_method(D_METHOD("get_plate_count"), &PlateTectonicFilter::get_plate_count);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "plate_count", PROPERTY_HINT_RANGE, "1, 32, 1"), "set_plate_count", "get_plate_count");

    ClassDB::bind_method(D_METHOD("set_plate_noise_amplitude", "value"), &PlateTectonicFilter::set_plate_noise_amplitude);
    ClassDB::bind_method(D_METHOD("get_plate_noise_amplitude"), &PlateTectonicFilter::get_plate_noise_amplitude);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "plate_noise_amplitude"), "set_plate_noise_amplitude", "get_plate_noise_amplitude");

    ClassDB::bind_method(D_METHOD("set_plate_noise_frequency", "value"), &PlateTectonicFilter::set_plate_noise_frequency);
    ClassDB::bind_method(D_METHOD("get_plate_noise_frequency"), &PlateTectonicFilter::get_plate_noise_frequency);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "plate_noise_frequency"), "set_plate_noise_frequency", "get_plate_noise_frequency");

    ClassDB::bind_method(D_METHOD("set_plate_noise_octaves", "value"), &PlateTectonicFilter::set_plate_noise_octaves);
    ClassDB::bind_method(D_METHOD("get_plate_noise_octaves"), &PlateTectonicFilter::get_plate_noise_octaves);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "plate_noise_octaves", PROPERTY_HINT_RANGE, "1, 8, 1"), "set_plate_noise_octaves", "get_plate_noise_octaves");

    ClassDB::bind_method(D_METHOD("set_continental_plate_ratio", "value"), &PlateTectonicFilter::set_continental_plate_ratio);
    ClassDB::bind_method(D_METHOD("get_continental_plate_ratio"), &PlateTectonicFilter::get_continental_plate_ratio);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "continental_plate_ratio", PROPERTY_HINT_RANGE, "0.0, 1.0, 0.01"), "set_continental_plate_ratio", "get_continental_plate_ratio");

    ClassDB::bind_method(D_METHOD("set_continentalness_threshold", "value"), &PlateTectonicFilter::set_continentalness_threshold);
    ClassDB::bind_method(D_METHOD("get_continentalness_threshold"), &PlateTectonicFilter::get_continentalness_threshold);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "continentalness_threshold", PROPERTY_HINT_RANGE, "0.01, 1.0, 0.01"), "set_continentalness_threshold", "get_continentalness_threshold");

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
    if (seed == value) return;
    seed = value;
    if (rng.is_valid())
        rng->set_seed(seed);
    plate_noise.SetSeed(seed);
    emit_changed();
}

void PlateTectonicFilter::set_plate_count(int value) {
    if (plate_count == value) return;
    plate_count = value;
    emit_changed();
}

void PlateTectonicFilter::set_plate_noise_amplitude(float value) {
    if (Math::is_equal_approx(plate_noise_amplitude, value)) return;
    plate_noise_amplitude = value;
    emit_changed();
}

void PlateTectonicFilter::set_plate_noise_frequency(float value) {
    if (Math::is_equal_approx(plate_noise_frequency, value)) return;
    plate_noise_frequency = value;
    plate_noise.SetFrequency(value);
    emit_changed();
}

void PlateTectonicFilter::set_plate_noise_octaves(int value) {
    if (plate_noise_octaves == value) return;
    plate_noise_octaves = value;
    emit_changed();
}

void PlateTectonicFilter::set_continental_plate_ratio(float value) {
    if (Math::is_equal_approx(continental_plate_ratio, value)) return;
    continental_plate_ratio = std::clamp(value, 0.0f, 1.0f);
    emit_changed();
}

void PlateTectonicFilter::set_continentalness_threshold(float value) {
    if (Math::is_equal_approx(continentalness_threshold, value)) return;
    continentalness_threshold = std::clamp(value, 0.01f, 1.0f);
    emit_changed();
}

void PlateTectonicFilter::set_elevation_scale(float value) {
    if (Math::is_equal_approx(elevation_scale, value)) return;
    elevation_scale = value;
    emit_changed();
}

void PlateTectonicFilter::set_divergence_strength(float value) {
    if (Math::is_equal_approx(divergence_strength, value)) return;
    divergence_strength = value;
    emit_changed();
}

void PlateTectonicFilter::set_convergence_strength(float value) {
    if (Math::is_equal_approx(convergence_strength, value)) return;
    convergence_strength = value;
    emit_changed();
}

Vector3 PlateTectonicFilter::random_unit_vector() {
    float u = rng->randf();
    float v = rng->randf();
    float theta = 2.0f * Math_PI * u;  // longitude
    float z = 2.0f * v - 1.0f;
    float r = sqrt(1.0f - z * z);
    float x = r * cos(theta);
    float y = r * sin(theta);
    return Vector3(x, y, z);
}

void PlateTectonicFilter::apply(std::vector<PlanetVertex> &vertices,
                                std::vector<int> &indices) {
    rng->set_seed(seed);
    plates.clear();

    // Create plates
    plates.reserve(plate_count);
    float offset = 2.0f / plate_count;
    float increment = Math_PI * (3.0f - sqrt(5.0f)); // golden angle

    for (size_t i = 0; i < plate_count; ++i) {
        float y = ((i * offset) - 1.0f) + (offset / 2.0f);
        float r = sqrt(1.0f - y*y);
        float phi = i * increment;

        float x = cos(phi) * r;
        float z = sin(phi) * r;

        auto plate = std::make_shared<Plate>();
        plate->id = i;
        plate->center = Vector3(x, y, z);
        plate->type = rng->randf() <= continental_plate_ratio ? CONTINENTAL : OCEANIC;

        plates.push_back(plate);
    }

    // Then grow voronoise (voronoi + noise hehe)
    for (auto &vertex : vertices) {
        Vector3 dir = vertex.get_normal();
        Vector3 noisy_dir = dir;

        Vector3 total_noise(0, 0, 0);
        for (size_t i = 0; i < plate_noise_octaves; i++) {
            float freq = plate_noise_frequency * pow(2.0f, i);
            float amp = 1.0f / pow(2.0f, i);
            Vector3 scaled_pos = dir * freq;

            total_noise +=
                Vector3(plate_noise.GetNoise(scaled_pos.x, scaled_pos.y,
                                             scaled_pos.z),
                        plate_noise.GetNoise(scaled_pos.y, scaled_pos.z,
                                             scaled_pos.x),
                        plate_noise.GetNoise(scaled_pos.z, scaled_pos.x,
                                             scaled_pos.y)) *
                amp;
        }
        noisy_dir += total_noise * plate_noise_amplitude;
        noisy_dir = noisy_dir.normalized();

        // assign plate
        float max_dot = -1.0f;
        int closest_index = 0;
        for (int i = 0; i < plate_count; i++) {
            float dot = noisy_dir.dot(plates[i]->center);
            if (dot > max_dot) {
                max_dot = dot;
                closest_index = i;
            }
        }
        vertex.set_plate(plates[closest_index]);
    }

    // Compute continentalness with BFS rings
    std::unordered_set<PlanetVertex *> current_ring;
    for (auto &vertex : vertices) {
        if (vertex.get_plate()->type == OCEANIC)
            current_ring.insert(&vertex);
    }

    for (size_t ring = 1; !current_ring.empty(); ++ring) {
        std::unordered_set<PlanetVertex *> next_ring;

        for (PlanetVertex *vertex : current_ring) {
            float current_value = vertex->get_continentalness();

            for (PlanetVertex *neighbour : vertex->get_neighbours()) {
                if (neighbour->get_plate()->type == OCEANIC)
                    continue;

                float edge_length = (vertex->get_position() - neighbour->get_position()).length();
                float next_value = current_value + edge_length / continentalness_threshold;
                float neighbour_value = neighbour->get_continentalness();

                if (neighbour_value == 0 || (neighbour_value > next_value)) {
                    neighbour->set_continentalness(next_value);
                    next_ring.insert(neighbour);
                }
            }
        }

        if (next_ring.empty())
            break;
        current_ring = std::move(next_ring);
    }

    float max_distance = 0.0f;
    for (auto &v : vertices) {
        if (v.get_plate()->type == CONTINENTAL)
            max_distance = std::max(max_distance, v.get_continentalness());
    }

    for (auto &v : vertices) {
        if (v.get_plate()->type == OCEANIC)
            v.set_continentalness(0.0f);
        else
            v.set_continentalness(std::min(v.get_continentalness() / max_distance, 1.0f));
    }

    //Assign height based on plate type
    // for (auto &vertex : vertices) {
    //     auto plate_ptr = vertex.get_plate();
    //     if (plate_ptr) {
    //         float base_elevation =
    //             (plate_ptr->type == CONTINENTAL) ? 0.5f : -0.5f;
    //         float elevation = base_elevation * elevation_scale;

    //         Vector3 current_pos = vertex.get_position();
    //         Vector3 dir = current_pos.normalized();
    //         vertex.set_position(current_pos + dir * elevation);
    //     }
    // }

    for (auto &vertex : vertices) {
        auto plate_ptr = vertex.get_plate();
        if (plate_ptr) {
            float elevation = vertex.get_continentalness() * elevation_scale;
            Vector3 current_pos = vertex.get_position();
            Vector3 dir = current_pos.normalized();
            vertex.set_position(current_pos + dir * elevation);
        }
    }

    // Calculate plate velocities
    const float delta_time = 1.0f;
    const float base_speed = 0.01f;

    for (auto &plate : plates) {
        Vector3 random_axis = random_unit_vector();
        plate->angular_velocity = random_axis * base_speed;
    }

    // Compare velocity to neighbour plates and adjust height
    // for (auto &vertex : vertices) {
    //     auto plate_ptr = vertex.get_plate();
    //     if (!plate_ptr) continue;

    //     Vector3 vertex_velocity = vertex.get_velocity();

    //     auto neighbour_plate = find_closest_plate(vertex);
    //     if (neighbour_plate && neighbour_plate != plate_ptr) {
    //         Vector3 dir = (neighbour_plate->center - vertex.get_position().normalized());
    //         Vector3 relative_vel = neighbour_plate->angular_velocity.cross(vertex.get_position() - vertex_velocity);

    //         float proj = relative_vel.dot(dir);

    //         if (proj > 0)
    //             vertex_velocity += dir * divergence_strength * delta_time;
    //         else
    //             vertex_velocity += dir * convergence_strength * delta_time;
    //     }

    //     Vector3 radial_dir = vertex.get_position().normalized();
    //     float delta_h = vertex_velocity.dot(radial_dir);
    //     vertex.set_position(vertex.get_position() + radial_dir * delta_h);
    // }
}

std::shared_ptr<Plate> PlateTectonicFilter::find_closest_plate(const PlanetVertex &vertex) {
    auto self_plate = vertex.get_plate();
    if (!self_plate) return nullptr;

    std::shared_ptr<Plate> closest = nullptr;
    float min_dist = std::numeric_limits<float>::max(); // huge number as dist ceiling

    for (auto &plate : plates) {
        if (plate == self_plate) continue;

        float distance = (plate->center - vertex.get_position()).length_squared(); // distance = sqrt((v2 - v1)²), but sqrt not needed if we're just looking for min
        if (distance < min_dist) {
            min_dist = distance;
            closest = plate;
        }
    }
    return closest;
}
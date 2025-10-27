#ifndef CUBE_SPHERE_GENERATOR_HPP
#define CUBE_SPHERE_GENERATOR_HPP

#include <vector>
#include "vertex_data.hpp"

class CubeSphereGenerator {
public:
    static void generate(float radius, int resolution, std::vector<VertexData> &vertices, std::vector<int> &indices);
};

#endif
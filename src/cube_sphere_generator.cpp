#include "cube_sphere_generator.hpp"

void CubeSphereGenerator::generate(float radius, int resolution, std::vector<VertexData> &vertices, std::vector<int> &indices)
{
    vertices.clear();
    indices.clear();

    for (int face = 0; face < 6; face++)
    {
        Vector3 localUp;
        switch (face)
        {
            case 0: localUp = Vector3(0, 1, 0); break;
            case 1: localUp = Vector3(0, -1, 0); break;
            case 2: localUp = Vector3(1, 0, 0); break;
            case 3: localUp = Vector3(-1, 0, 0); break;
            case 4: localUp = Vector3(0, 0, 1); break;
            case 5: localUp = Vector3(0, 0, -1); break;
        }

        Vector3 axisA = Vector3(localUp.y, localUp.z, localUp.x);
        Vector3 axisB = localUp.cross(axisA);

        for (int y = 0 ; y < resolution; y++)
        {
            for (int x = 0; x < resolution ; x++)
            {
                Vector2 percent(float(x) / (resolution - 1), float(y) / (resolution - 1));
                Vector3 positionOnCube = localUp + (percent.x - .5f) * 2 * axisA + (percent.y - .5f) * 2 * axisB;
                Vector3 positionOnSphere = positionOnCube.normalized() * radius;

                VertexData vertex_data;

                vertex_data.position = positionOnSphere;
                vertex_data.normal = positionOnSphere.normalized();
                vertex_data.uv = percent;

                vertices.push_back(vertex_data);
            
                if (x != resolution - 1 && y != resolution - 1)
                {
                    int faceOffset = face * resolution * resolution;
                    int i = x + y * resolution + faceOffset;
                    indices.push_back(i);
                    indices.push_back(i + resolution);
                    indices.push_back(i + resolution + 1);
                    indices.push_back(i);
                    indices.push_back(i + resolution + 1);
                    indices.push_back(i + 1);
                }
            }
        }
    }
}
#ifndef MESH_H
#define MESH_H

#include <kanima/linalg/vec3.h>
#include <kanima/core/triangle.h>
#include <kanima/core/color.h>
#include <kanima/core/material.h>
#include <kanima/core/baryCoord.h>
#include <kanima/core/aabb.h>
#include <vector>

namespace krt
{
class Mesh
{
public:
    Mesh();
    void insertVertex(float v0, float v1, float v2);
    void insertTriangleIndex(int i0, int i1, int i2);
    Triangle getTriangleByIndex(const int index) const;
    std::vector<Triangle> generateTriangleList() const;
    std::vector<Triangle> generateTriangleWithCentroidList(int objectId) const;
    void setUniformColor(const Color &color);
    void setRandomColors();
    void setMaterial(const Material& material);
    void computeTriangleNormals();
    void computeVertexNormals();
    BaryCoord findBaryCentricCoords(vec3& point, int triangleIndex) const;
    vec3 findInterpolatedVertNormal(BaryCoord& baryCentricCoord, int triangleIndex) const;
    double intersectRay(const Ray& r, int& hitTriangleIndex, vec3& hitPoint, vec3& hitNormal, bool cullBackFaces) const;
    Color getAlbedo(BaryCoord& baryPoint, int triangleIndex);
    void insertVectorUVs(float u, float v, float w);
    void computeAABB();

    Color uniformColor;
    bool randomizeColors;
    std::vector<vec3> vertices;
    std::vector<int> triangleVertIndices;
    std::vector<vec3> triangleNormals;
    std::vector<vec3> vertexNormals;
    Material material;
    std::vector<vec3> vertexUVs;
    AABB boundingBox;
};

}

#endif // MESH_H

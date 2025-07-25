#include <kanima/core/mesh.h>
#include <cstdlib>
#include <vector>
#include <cassert>
#include <limits>

namespace krt
{
Mesh::Mesh() : uniformColor(Color(1.0f, 1.0f, 1.0f)), randomizeColors(false)
{}

void Mesh::setUniformColor(const Color &color)
{
    this->uniformColor = color;
}

void Mesh::setRandomColors()
{
    this->randomizeColors = true;
}

void Mesh::insertVertex(float v0, float v1, float v2)
{
    vertices.push_back(vec3(v0, v1, v2));
}

void Mesh::insertTriangleIndex(int i0, int i1, int i2)
{
    assert((i0 != i1 && i1 != i2 && i2 != i0) && "All three vertex indices must be different");

    triangleVertIndices.push_back(i0);
    triangleVertIndices.push_back(i1);
    triangleVertIndices.push_back(i2);
}

Triangle Mesh::getTriangleByIndex(const int index) const
{
    assert(index >= 0 && (unsigned long long)index < triangleVertIndices.size() / 3);

    const vec3& v0 = vertices[triangleVertIndices[index*3]];
    const vec3& v1 = vertices[triangleVertIndices[index*3 + 1]];
    const vec3& v2 = vertices[triangleVertIndices[index*3 + 2]];

    return Triangle(v0, v1, v2, uniformColor);
}

std::vector<Triangle> Mesh::generateTriangleList() const
{
    assert((triangleVertIndices.size() % 3 == 0) && "Missing indices for triangles");
    std::vector<Triangle> triangles;

    for(size_t i=0; i+2 < triangleVertIndices.size(); i += 3)
    {
        int idx0 = triangleVertIndices[i];
        int idx1 = triangleVertIndices[i + 1];
        int idx2 = triangleVertIndices[i + 2];

        const vec3& v0 = vertices[idx0];
        const vec3& v1 = vertices[idx1];
        const vec3& v2 = vertices[idx2];

        Color color = this->uniformColor;

        if (this->randomizeColors)
        {
            float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);//rand() % 256;
            float g = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);//rand() % 256;
            float b = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);//rand() % 256;

            color = Color(r, g, b);
        }

        triangles.push_back(Triangle(v0, v1, v2, color));
    }

    return triangles;
}


std::vector<Triangle> Mesh::generateTriangleWithCentroidList(int objectId) const
{
    assert((triangleVertIndices.size() % 3 == 0) && "Missing indices for triangles");
    std::vector<Triangle> triangles;
    triangles.reserve(triangleVertIndices.size() / 3);

    for(size_t i=0; i+2 < triangleVertIndices.size(); i += 3)
    {
        int idx0 = triangleVertIndices[i];
        int idx1 = triangleVertIndices[i + 1];
        int idx2 = triangleVertIndices[i + 2];

        const vec3& v0 = vertices[idx0];
        const vec3& v1 = vertices[idx1];
        const vec3& v2 = vertices[idx2];

        triangles.push_back(Triangle(v0, v1, v2, objectId, i/3));
    }

    return triangles;
}


void Mesh::setMaterial(const Material& material)
{
    this->material = material;
}

void Mesh::computeTriangleNormals()
{
    triangleNormals.clear();
    triangleNormals.reserve(triangleVertIndices.size()/3);

    for (size_t i = 0; i < triangleVertIndices.size(); i += 3)
    {
        vec3 v0 = vertices[triangleVertIndices[i]];
        vec3 v1 = vertices[triangleVertIndices[i + 1]];
        vec3 v2 = vertices[triangleVertIndices[i + 2]];

        vec3 edge1 = v1 - v0;
        vec3 edge2 = v2 - v0;

        vec3 normal = (edge1.cross(edge2)).normalized();
        triangleNormals.push_back(normal);
    }
}

void Mesh::computeVertexNormals()
{
    // init vector's normals to 0,0,0
    vertexNormals = std::vector<vec3>(vertices.size(), vec3(0, 0, 0));

    // for each vertex v of the triangle
    //  add the triangle t's normal to v
    for (size_t i = 0; i < triangleVertIndices.size(); i += 3)
    {
        const int i0 = triangleVertIndices[i];
        const int i1 = triangleVertIndices[i + 1];
        const int i2 = triangleVertIndices[i + 2];

        const vec3& triangleNormal = triangleNormals[i / 3];

        vertexNormals[i0] = vertexNormals[i0] + triangleNormal;
        vertexNormals[i1] = vertexNormals[i1] + triangleNormal;
        vertexNormals[i2] = vertexNormals[i2] + triangleNormal;
    }

    // normalize all vector normals
    for (vec3& norm : vertexNormals)
    {
        norm = norm.normalized();
    }
}

double Mesh::intersectRay(const Ray& r, int& hitTriangleIndex, vec3& hitPoint, vec3& hitNormal, bool cullBackFaces) const
{
    double minT = 1/EPSILON;
    hitTriangleIndex = -1;

    // if the mesh's material is refractive, all the triangles in it can be ignored for shadow ray
    if (r.type == RayType::shadow && this->material.type == MaterialType::Refractive)
        return -1.0;

    // AABB intersection test
    if (!this->boundingBox.rayIntersectBox(r))
        return -1.0;

    for (size_t i = 0; i < triangleVertIndices.size(); i += 3)
    {
        const vec3& v0 = vertices[triangleVertIndices[i]];
        const vec3& v1 = vertices[triangleVertIndices[i + 1]];
        const vec3& v2 = vertices[triangleVertIndices[i + 2]];
        vec3 normal = triangleNormals[i / 3];

        if (cullBackFaces && normal.dot(r.d) > EPSILON) continue; // backface culling

        // proj is negative if the normal and ray are in opposite direction. positive if the directions are same
        double proj = normal.dot(r.d);
        if (std::abs(proj) < EPSILON) continue; // parallel (normal is perpendicular to ray)

        double t = normal.dot(v0 - r.o) / proj;
        if (t < 0 || t > minT) continue; // opposite direction

        vec3 p = r.o + r.d * t;

        vec3 e01 = v1 - v0;
        vec3 e12 = v2 - v1;
        vec3 e20 = v0 - v2;


        if (normal.dot(e01.cross(p - v0)) < -EPSILON) continue;
        if (normal.dot(e12.cross(p - v1)) < -EPSILON) continue;
        if (normal.dot(e20.cross(p - v2)) < -EPSILON) continue;


        minT = t;
        hitPoint = p;
        hitNormal = normal;
        hitTriangleIndex = static_cast<int>(i / 3);
    }

    return (hitTriangleIndex != -1) ? minT : -1.0;
}


BaryCoord Mesh::findBaryCentricCoords(vec3& point, int triangleIndex) const
{
    assert((size_t)(triangleIndex*3 + 2) < this->triangleVertIndices.size());
    const vec3& v0 = this->vertices[this->triangleVertIndices[triangleIndex*3]];
    const vec3& v1 = this->vertices[this->triangleVertIndices[triangleIndex*3 + 1]];
    const vec3& v2 = this->vertices[this->triangleVertIndices[triangleIndex*3 + 2]];
    vec3 e01 = v1 - v0;
    vec3 e02 = v2 - v0;

    vec3 e0p = point - v0;

    float area_tri = e01.cross(e02).length() / 2.0f;
    float area_m = e0p.cross(e02).length() / 2.0f;
    float area_n = e01.cross(e0p).length() / 2.0f;

    float u = area_m / area_tri;
    float v = area_n / area_tri;
    float w = 1 - u - v;

    return BaryCoord(u, v, w);
}


vec3 Mesh::findInterpolatedVertNormal(BaryCoord& baryCentCoords, int triangleIndex) const
{
    assert((size_t)(triangleIndex*3 + 2) < this->triangleVertIndices.size());
    const vec3& n0 = this->vertexNormals[this->triangleVertIndices[triangleIndex*3]];
    const vec3& n1 = this->vertexNormals[this->triangleVertIndices[triangleIndex*3 + 1]];
    const vec3& n2 = this->vertexNormals[this->triangleVertIndices[triangleIndex*3 + 2]];

    return n0*baryCentCoords.w + n1*baryCentCoords.u + n2*baryCentCoords.v;
}


void Mesh::insertVectorUVs(float u, float v, float w)
{
    const vec3 vertex_uv =  vec3(u, v, w);
    this->vertexUVs.push_back(vertex_uv);
}


Color Mesh::getAlbedo(BaryCoord& baryPoint, int triangleIndex)
{
    int vertId0 = this->triangleVertIndices[triangleIndex*3];
    int vertId1 = this->triangleVertIndices[triangleIndex*3 + 1];
    int vertId2 = this->triangleVertIndices[triangleIndex*3 + 2];

    // in older programs vertexUVs are empty

    vec3 uv0 = vec3(0.f, 0.f, 0.f);
    vec3 uv1 = vec3(0.f, 0.f, 0.f);
    vec3 uv2 = vec3(0.f, 0.f, 0.f);

    if (!this->vertexUVs.empty())
    {
        uv0 = this->vertexUVs[vertId0];
        uv1 = this->vertexUVs[vertId1];
        uv2 = this->vertexUVs[vertId2];
    }

    // only u and v needed for texture coordinate
    float u = baryPoint.u * uv1.x + baryPoint.v * uv2.x + baryPoint.w * uv0.x;
    float v = baryPoint.u * uv1.y + baryPoint.v * uv2.y + baryPoint.w * uv0.y;

    return this->material.albedoTex->getTextureAlbedo(u, v, baryPoint);
}


void Mesh::computeAABB()
{
    float minx, miny, minz, maxx, maxy, maxz;
    minx = miny = minz = std::numeric_limits<float>::infinity();
    maxx = maxy = maxz = -std::numeric_limits<float>::infinity();

    for (vec3 vertex : this->vertices)
    {
        minx = std::min(vertex.x, minx);
        miny = std::min(vertex.y, miny);
        minz = std::min(vertex.z, minz);

        maxx = std::max(vertex.x, maxx);
        maxy = std::max(vertex.y, maxy);
        maxz = std::max(vertex.z, maxz);
    }

    vec3 minv = vec3(minx, miny, minz);
    vec3 maxv = vec3(maxx, maxy, maxz);

    this->boundingBox = AABB(minv, maxv);
}

}

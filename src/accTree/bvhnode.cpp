#include <kanima/accTree/bvhnode.h>

namespace krt
{
BVHNode::BVHNode()
{
//    leftChildIdx = -1;
//    rightChildIdx = -1;
}

void BVHNode::createBB(std::vector<Triangle>& trianglesInNode)
{
    float minx = std::numeric_limits<float>::infinity();
    float miny = std::numeric_limits<float>::infinity();
    float minz = std::numeric_limits<float>::infinity();

    float maxx = -std::numeric_limits<float>::infinity();
    float maxy = -std::numeric_limits<float>::infinity();
    float maxz = -std::numeric_limits<float>::infinity();

    for (auto& tri : trianglesInNode)
    {
        const vec3& v0 = tri.v0;
        const vec3& v1 = tri.v1;
        const vec3& v2 = tri.v2;

        float minx_tri = std::min(v0.x, std::min(v1.x, v2.x));
        float miny_tri = std::min(v0.y, std::min(v1.y, v2.y));
        float minz_tri = std::min(v0.z, std::min(v1.z, v2.z));

        float maxx_tri = std::max(v0.x, std::max(v1.x, v2.x));
        float maxy_tri = std::max(v0.y, std::max(v1.y, v2.y));
        float maxz_tri = std::max(v0.z, std::max(v1.z, v2.z));

        minx = std::min(minx, minx_tri);
        miny = std::min(miny, miny_tri);
        minz = std::min(minz, minz_tri);

        maxx = std::max(maxx, maxx_tri);
        maxy = std::max(maxy, maxy_tri);
        maxz = std::max(maxz, maxz_tri);

    }

    vec3 minV = vec3(minx, miny, minz);
    vec3 maxV = vec3(maxx, maxy, maxz);

    this->boundingBox = AABB(minV, maxV);
}

}

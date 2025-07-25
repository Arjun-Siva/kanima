#ifndef BVHNODE_H
#define BVHNODE_H

#include <kanima/core/aabb.h>
#include <kanima/core/triangle.h>
#include <kanima/core/ray.h>

#include <vector>
#include <memory>

namespace krt
{
class BVHNode
{
private:

public:
    AABB boundingBox;
    std::unique_ptr<BVHNode> left = nullptr;
    std::unique_ptr<BVHNode> right = nullptr;
    std::vector<std::pair<int, int>> triangleIndices; // object idx and triangle idx

    BVHNode();
    void createBB(std::vector<Triangle>& trianglesInNode);
};

}
#endif // BVHNODE_H

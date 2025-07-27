#ifndef SCENE_H
#define SCENE_H

#include <kanima/core/camera.h>
#include <kanima/core/mesh.h>
#include <kanima/core/color.h>
#include <kanima/core/light.h>
#include <kanima/core/material.h>
#include <kanima/core/intersectionData.h>
#include <kanima/texture/texture.h>
#include <kanima/texture/albedoTexture.h>
#include <kanima/texture/bitmapTexture.h>
#include <kanima/texture/checkerTexture.h>
#include <kanima/texture/edgeTexture.h>
#include <kanima/core/triangle.h>
#include <kanima/accTree/bvhnode.h>

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

namespace krt
{
class Scene
{
private:

public:
    Camera camera;
    int height;
    int width;
    Color bgColor;
    std::vector<Mesh> geometryObjects;
    std::vector<Light> lights;
    std::vector<Material> meshMaterials;
    std::unordered_map<std::string, std::shared_ptr<Texture>> textureMap;
    int bucketSize = 24;
    std::unique_ptr<BVHNode> bvhRoot = nullptr;
    int max_bvhtree_depth = 24;
    int min_triangles_per_bvhnode = 4;
    bool useBVH = false;
    int gi_ray_count = 0;


    Scene();
    Scene(const std::string& sceneFileName);
    void addMesh(Mesh& mesh);
    std::vector<Mesh> getMeshes();
    void parseSceneFile(const std::string& sceneFileName);
    void addLight(Light& light);
    void addMaterial(Material& material);
    void addTexture(std::string& name, std::shared_ptr<Texture> texture);
    IntersectionData traceRay(const Ray& ray);
    std::vector<Triangle> getAllTrianglesInScene();
    double shortestIntersectionInNode(BVHNode* node, const Ray &ray, int &hitTriangleIdx, int &hitObjectIdx, vec3 &hitPoint, vec3 &hitNormal);
    IntersectionData traceRayBVH(const Ray& ray);
    std::unique_ptr<BVHNode> buildBVHTree(std::vector<Triangle>& allTrianglesInParent, int depth);

};
}
#endif // SCENE_H

#include <kanima/core/scene.h>

#include <vector>
#include <fstream>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <memory>

#include <kanima/rapidjson/rapidjson/document.h>
#include <kanima/rapidjson/rapidjson/istreamwrapper.h>

namespace krt
{
using namespace rapidjson;

Scene::Scene() : camera(1920.0f/1080.0f)
{
    this->bgColor = Color(0, 0, 0);
    this->height = 1080;
    this->width = 1920;
    this->bucketSize = 1;
}

Scene::Scene(const std::string& sceneFileName) : camera(1920.0f/1080.0f)
{
    parseSceneFile(sceneFileName);
}

void Scene::addMesh(Mesh& mesh)
{
    geometryObjects.push_back(mesh);
}

std::vector<Mesh> Scene::getMeshes()
{
    return geometryObjects;
}

void Scene::addMaterial(Material &material)
{
    meshMaterials.push_back(material);
}

void Scene::parseSceneFile(const std::string &sceneFileName)
{
    // default values for scene, camera
    this->bgColor = Color(0, 0, 0);
    this->height = 1080;
    this->width = 1920;
    this->bucketSize = 24;
    this->max_bvhtree_depth = 24;
    this->min_triangles_per_bvhnode = 4;

    vec3 eye(0, 0, 0);
    vec3 right(1, 0, 0);
    vec3 up(0, 1, 0);
    vec3 forward(0, 0, -1);

    this->camera = Camera(eye, right, up, forward, (float)width/(float)height, 1);

    // Read file
    std::ifstream ifs(sceneFileName);
    assert(ifs.is_open());

    IStreamWrapper isw(ifs);
    Document doc;
    doc.ParseStream(isw);

    if (doc.HasParseError()) {
        std::cerr << "Parse error: " << doc.GetParseError() << std::endl;
    }

    // Settings

    if (doc.HasMember("settings"))
    {
        const Value& settingsVal = doc.FindMember("settings")->value;

        if (!settingsVal.IsNull() && settingsVal.IsObject())
        {
            if (settingsVal.HasMember("background_color"))
            {
                const Value& bgColorVal = settingsVal.FindMember("background_color")->value;
                if(!bgColorVal.IsNull() && bgColorVal.IsArray())
                {
                    assert(bgColorVal.Size() == 3);
                    // color values are in the range 0.0 to 1.0
                    this->bgColor = Color(static_cast<float>(bgColorVal[0].GetDouble()),
                            static_cast<float>(bgColorVal[1].GetDouble()),
                            static_cast<float>(bgColorVal[2].GetDouble())
                            );
                }
            }

            if (settingsVal.HasMember("image_settings"))
            {
                const Value& imageDim = settingsVal.FindMember("image_settings")->value;
                if(!imageDim.IsNull() && imageDim.IsObject())
                {
                    if (imageDim.HasMember("height"))
                    {
                        const Value& hv = imageDim.FindMember("height")->value;
                        if (!hv.IsNull() && hv.IsNumber()) this->height = static_cast<int>(hv.GetDouble());
                    }

                    if (imageDim.HasMember("width"))
                    {
                        const Value& wv = imageDim.FindMember("width")->value;
                        if (!wv.IsNull() && wv.IsNumber()) this->width = static_cast<int>(wv.GetDouble());
                    }

                    if (imageDim.HasMember("bucket_size"))
                    {
                        const Value& bucket_size = imageDim.FindMember("bucket_size")->value;
                        if (!bucket_size.IsNull() && bucket_size.IsNumber()) this->bucketSize = static_cast<int>(bucket_size.GetDouble());
                    }
                }
            } // image_settings
        }
    } // settings

    // camera

    if (doc.HasMember("camera"))
    {
        const Value& cameraVal = doc.FindMember("camera")->value;

        if (!cameraVal.IsNull() && cameraVal.IsObject())
        {
            const Value& positionVal = cameraVal.FindMember("position")->value;
            if(!positionVal.IsNull() && positionVal.IsArray())
            {
                assert(positionVal.Size() == 3);
                eye = vec3(static_cast<float>(positionVal[0].GetDouble()),
                        static_cast<float>(positionVal[1].GetDouble()),
                        static_cast<float>(positionVal[2].GetDouble())
                        );

                this->camera = Camera(eye, right, up, forward, (float)this->width/(float)this->height, 1);
            }

            if (cameraVal.HasMember("matrix"))
            {
                const Value& matrixVal = cameraVal.FindMember("matrix")->value;

                if(!matrixVal.IsNull() && matrixVal.IsArray())
                {
                    vec3 r0 = vec3(static_cast<float>(matrixVal[0].GetDouble()),
                            static_cast<float>(matrixVal[1].GetDouble()),
                            static_cast<float>(matrixVal[2].GetDouble())
                            );

                    vec3 r1 = vec3(static_cast<float>(matrixVal[3].GetDouble()),
                            static_cast<float>(matrixVal[4].GetDouble()),
                            static_cast<float>(matrixVal[5].GetDouble())
                            );

                    vec3 r2 = vec3(static_cast<float>(matrixVal[6].GetDouble()),
                            static_cast<float>(matrixVal[7].GetDouble()),
                            static_cast<float>(matrixVal[8].GetDouble())
                            );

                    mat3 rotationMatrix = mat3(r0, r1, r2);
                    this->camera.setOrientation(rotationMatrix);
                }
            } // matrix

        }
    } // camera

    // Textures
    if (doc.HasMember("textures") && doc["textures"].IsArray())
    {
        const auto& textures = doc.FindMember("textures")->value;
        for (const auto& texture: textures.GetArray())
        {
            if (texture.HasMember("type") && texture.HasMember("name"))
            {
                std::string texType = texture["type"].GetString();
                std::string name = texture["name"].GetString();

                if (texType == "albedo")
                {
                    if (texture.HasMember("albedo") && texture["albedo"].IsArray())
                    {
                        const auto& albedoArr = texture["albedo"].GetArray();
                        assert(albedoArr.Size() == 3);

                        float a0 = static_cast<float>(albedoArr[0].GetDouble());
                        float a1 = static_cast<float>(albedoArr[1].GetDouble());
                        float a2 = static_cast<float>(albedoArr[2].GetDouble());

                        const auto& albedo = Color(a0, a1, a2);

                        this->textureMap[name] = std::make_shared<AlbedoTexture>(name, albedo);
                    }
                }

                else if (texType == "edges")
                {
                    if (texture.HasMember("edge_color") && texture.HasMember("inner_color") & texture["edge_color"].IsArray() & texture["inner_color"].IsArray())
                    {
                        const auto& edgeColor = texture["edge_color"].GetArray();
                        assert(edgeColor.Size() == 3);

                        float ec0 = static_cast<float>(edgeColor[0].GetDouble());
                        float ec1 = static_cast<float>(edgeColor[1].GetDouble());
                        float ec2 = static_cast<float>(edgeColor[2].GetDouble());

                        const auto& edge_color = Color(ec0, ec1, ec2);

                        const auto& innerColor = texture["inner_color"].GetArray();
                        assert(innerColor.Size() == 3);

                        float ic0 = static_cast<float>(innerColor[0].GetDouble());
                        float ic1 = static_cast<float>(innerColor[1].GetDouble());
                        float ic2 = static_cast<float>(innerColor[2].GetDouble());

                        const auto& inner_color = Color(ic0, ic1, ic2);

                        if (texture.HasMember("edge_width") && texture["edge_width"].IsFloat())
                        {
                            float edge_width = texture["edge_width"].GetFloat();

                            this->textureMap[name] = std::make_shared<EdgeTexture>(name, inner_color, edge_color, edge_width);
                        }
                    }
                }

                else if (texType == "checker")
                {
                    if (texture.HasMember("color_A") && texture.HasMember("color_B") & texture["color_A"].IsArray() & texture["color_B"].IsArray())
                    {
                        const auto& colorA = texture["color_A"].GetArray();
                        assert(colorA.Size() == 3);

                        float a0 = static_cast<float>(colorA[0].GetDouble());
                        float a1 = static_cast<float>(colorA[1].GetDouble());
                        float a2 = static_cast<float>(colorA[2].GetDouble());

                        const auto& color_a = Color(a0, a1, a2);

                        const auto& colorB = texture["color_B"].GetArray();
                        assert(colorB.Size() == 3);

                        float b0 = static_cast<float>(colorB[0].GetDouble());
                        float b1 = static_cast<float>(colorB[1].GetDouble());
                        float b2 = static_cast<float>(colorB[2].GetDouble());

                        const auto& color_b = Color(b0, b1, b2);

                        float square_size = 0.5; // default

                        if (texture.HasMember("square_size") && texture["square_size"].IsFloat())
                        {
                            square_size = texture["square_size"].GetFloat();
                        }

                        this->textureMap[name] = std::make_shared<CheckerTexture>(name, color_a, color_b, square_size);
                    }
                }

                else if (texType == "bitmap")
                {
                    if (texture.HasMember("file_path") && texture["file_path"].IsString())
                    {
                        const std::string filePath = texture["file_path"].GetString();

                        assert(filePath.size() > 1 && "File path empty");
                        this->textureMap[name] = std::make_shared<BitmapTexture>(name, filePath);
                    }
                }
            }
        }
    } // textures

    // Materials
    // Materials must be loaded before creating meshes
    if (doc.HasMember("materials") && doc["materials"].IsArray())
    {
        int default_count = 0;
        const auto& materials = doc.FindMember("materials")->value;
        for (const auto& material: materials.GetArray())
        {
            Material currentMaterial;

            if (material.HasMember("type"))
            {
                std::string matType = material["type"].GetString();

                if (matType == "diffuse")
                {
                    currentMaterial.type = MaterialType::Diffuse;
                }
                else if (matType == "reflective")
                {
                    currentMaterial.type = MaterialType::Reflective;
                }
                else if (matType == "refractive")
                {
                    currentMaterial.type = MaterialType::Refractive;
                }
                else if (matType == "constant")
                {
                    currentMaterial.type = MaterialType::Constant;
                }

            }

            if (material.HasMember("albedo"))
            {
                if (material["albedo"].IsArray())
                {
                    const auto& albedoArr = material["albedo"].GetArray();
                    assert(albedoArr.Size() == 3);

                    float a0 = static_cast<float>(albedoArr[0].GetDouble());
                    float a1 = static_cast<float>(albedoArr[1].GetDouble());
                    float a2 = static_cast<float>(albedoArr[2].GetDouble());

                    // create a default texture (For backwards compatibility)
                    std::string texName = "default" + std::to_string(default_count++);
                    const auto& albedo = Color(a0, a1, a2);
                    auto defaultTex = std::make_shared<AlbedoTexture>(texName, albedo);
                    this->textureMap[texName] = defaultTex;

                    currentMaterial.albedoTex = defaultTex;
                }
                else if (material["albedo"].IsString())
                {
                    const auto& name = material["albedo"].GetString();
                    currentMaterial.albedoTex = this->textureMap[name];
                }
            }

            if (material.HasMember("smooth_shading") && material["smooth_shading"].IsBool())
            {
                currentMaterial.smoothShading = material["smooth_shading"].GetBool();
            }

            if (material.HasMember("ior") && material["ior"].IsDouble())
            {
                currentMaterial.ior = static_cast<float>(material["ior"].GetDouble());
            }

            this->addMaterial(currentMaterial);

        }
    }



    // Objects
    if (doc.HasMember("objects") && doc["objects"].IsArray())
    {
        const auto& objs = doc.FindMember("objects")->value;
        for (const auto& obj : objs.GetArray())
        {
            Mesh mesh;
            // vertices
            if (obj.HasMember("vertices") && obj["vertices"].IsArray())
            {
                const auto& vertexArray = obj["vertices"].GetArray();
                assert(vertexArray.Size() % 3 == 0);
                mesh.vertices.reserve(vertexArray.Size() / 3);

                for (unsigned int i = 0; i + 2 < vertexArray.Size(); i += 3)
                {
                    mesh.insertVertex(static_cast<float>(vertexArray[i].GetDouble()),
                                      static_cast<float>(vertexArray[i + 1].GetDouble()),
                                      static_cast<float>(vertexArray[i + 2].GetDouble())
                                    );
                }

            }

            if (obj.HasMember("triangles") && obj["triangles"].IsArray())
            {
                const auto& triangleArray = obj["triangles"].GetArray();
                assert(triangleArray.Size() % 3 == 0);

                mesh.triangleVertIndices.reserve(triangleArray.Size());

                for (unsigned int i = 0; i + 2 < triangleArray.Size(); i += 3)
                {
                    mesh.insertTriangleIndex(static_cast<int>(triangleArray[i].GetDouble()),
                                      static_cast<int>(triangleArray[i + 1].GetDouble()),
                                      static_cast<int>(triangleArray[i + 2].GetDouble())
                                    );
                }
            }

            if (obj.HasMember("material_index") && obj["material_index"].IsInt())
            {
                mesh.setMaterial(this->meshMaterials[obj["material_index"].GetInt()]);
            }

            if (obj.HasMember("uvs") && obj["uvs"].IsArray())
            {
                const auto& uvArray = obj["uvs"].GetArray();
                assert(uvArray.Size() % 3 == 0);
                mesh.vertexUVs.reserve(uvArray.Size() / 3);

                for (unsigned int i = 0; i + 2 < uvArray.Size(); i += 3)
                {
                    mesh.insertVectorUVs(static_cast<float>(uvArray[i].GetDouble()),
                                      static_cast<float>(uvArray[i + 1].GetDouble()),
                                      static_cast<float>(uvArray[i + 2].GetDouble())
                                    );
                }
            }

            mesh.computeTriangleNormals();
            mesh.computeVertexNormals();
            mesh.computeAABB();

            this->addMesh(mesh);
        }

    } //objects

    // Lights
    if (doc.HasMember("lights") && doc["lights"].IsArray())
    {
        const auto& lights = doc.FindMember("lights")->value;
        std::vector<Light> sceneLights;
        this->lights.reserve(lights.GetArray().Size());

        for (const auto& light : lights.GetArray())
        {
            if(!light.IsNull() && light.IsObject())
            {
                if(light.HasMember("intensity") && light.HasMember("position"))
                {
                    float intensity = light["intensity"].GetFloat();
                    assert(light["position"].IsArray());
                    const auto& positionArr = light["position"].GetArray();
                    assert(positionArr.Size() == 3);
                    vec3 position = vec3(static_cast<float>(positionArr[0].GetDouble()),
                            static_cast<float>(positionArr[1].GetDouble()),
                            static_cast<float>(positionArr[2].GetDouble())
                            );

                    this->lights.push_back(Light(position, intensity));
                }
            }
        }
    } // lights
}


IntersectionData Scene::traceRay(const Ray &ray)
{
    // find shortest intersecting triangle
    // find the point of intersection
    // get the normal of triangle

    IntersectionData iData;

    vec3 hitPoint;
    vec3 hitNormal;
    int hitTriangleIdx;
    int hitMeshIdx;
    Material* hitMaterial = nullptr;
    bool missedAllMeshes = true;
    float shortestIntersection = std::numeric_limits<float>::max();

    int meshIndex = 0;
    for (Mesh& mesh : this->geometryObjects)
    {
        int meshHitTriIndex;
        vec3 meshHitPoint;
        vec3 meshHitNormal;

        bool cullBackfaces = mesh.material.type == MaterialType::Refractive ? false : true;

        double t = mesh.intersectRay(ray, meshHitTriIndex, meshHitPoint, meshHitNormal, cullBackfaces);

        // shorter hit than previous and not miss (not -1)
        if (t < shortestIntersection && t > -EPSILON)
        {
            shortestIntersection = t;
            hitPoint = meshHitPoint;
            hitNormal = meshHitNormal;
            hitMaterial = &mesh.material;
            hitTriangleIdx = meshHitTriIndex;
            missedAllMeshes = false;
            hitMeshIdx = meshIndex;
        }
        meshIndex++;
    }

    if (!missedAllMeshes)
    {
        iData.hitPoint = hitPoint;
        iData.hitPointNormal = hitNormal;
        iData.material = hitMaterial;
        iData.objectIdx = hitMeshIdx;
        iData.triangleIdx = hitTriangleIdx;

        iData.baryCentricCoords = this->geometryObjects[hitMeshIdx].findBaryCentricCoords(hitPoint, hitTriangleIdx);
        iData.interpolatedVertNormal = this->geometryObjects[hitMeshIdx].findInterpolatedVertNormal(iData.baryCentricCoords, hitTriangleIdx);

    }

    return iData;
}


std::vector<Triangle> Scene::getAllTrianglesInScene()
{
    std::vector<Triangle> sceneTriangles;
    for (size_t oid = 0; oid < this->geometryObjects.size(); oid++)
    {
        Mesh& mesh = this->geometryObjects[oid];
        std::vector<Triangle> objectTriangles = mesh.generateTriangleWithCentroidList(oid);
        sceneTriangles.insert(sceneTriangles.end(), objectTriangles.begin(), objectTriangles.end());
    }

    return sceneTriangles;
}


double Scene::shortestIntersectionInNode(BVHNode* node,const Ray &ray, int &hitTriangleIdx, int &hitObjectIdx, vec3 &hitPoint, vec3 &hitNormal)
{
    double minT = 1/EPSILON;
    hitTriangleIdx = -1;

    assert(node != nullptr);

    // AABB intersection test
    if (!node->boundingBox.rayIntersectBox(ray))
        return -1.0;

    // leaf node
    if (node->left == nullptr && node->right == nullptr)
    {
        for (auto& trianglePair : node->triangleIndices)
        {
            int meshIdx = trianglePair.first;
            int triangleIdx = trianglePair.second;

            Mesh& triangleMesh = this->geometryObjects[meshIdx];
            // if the mesh's material is refractive, all the triangles in it can be ignored for shadow ray
            if (ray.type == RayType::shadow && triangleMesh.material.type == MaterialType::Refractive)
                continue;

            bool cullBackfaces = (triangleMesh.material.type == MaterialType::Refractive || ray.type == RayType::shadow) ? false : true;

            const vec3& v0 = triangleMesh.vertices[triangleMesh.triangleVertIndices[triangleIdx*3]];
            const vec3& v1 = triangleMesh.vertices[triangleMesh.triangleVertIndices[triangleIdx*3 + 1]];
            const vec3& v2 = triangleMesh.vertices[triangleMesh.triangleVertIndices[triangleIdx*3 + 2]];
            const vec3& normal = triangleMesh.triangleNormals[triangleIdx];

            if (cullBackfaces && normal.dot(ray.d) > EPSILON) continue; // backface culling

            // proj is negative if the normal and ray are in opposite direction. positive if the directions are same
            double proj = normal.dot(ray.d);
            if (std::abs(proj) < EPSILON) continue; // parallel (normal is perpendicular to ray)

            double t = normal.dot(v0 - ray.o) / proj;
            if (t < EPSILON || t > minT) continue; // opposite direction

            vec3 p = ray.o + ray.d * t;

            vec3 e01 = v1 - v0;
            vec3 e12 = v2 - v1;
            vec3 e20 = v0 - v2;

            if (normal.dot(e01.cross(p - v0)) < -EPSILON) continue;
            if (normal.dot(e12.cross(p - v1)) < -EPSILON) continue;
            if (normal.dot(e20.cross(p - v2)) < -EPSILON) continue;

            if (t < minT)
            {
                minT = t;
                hitPoint = p;
                hitNormal = normal;
                hitObjectIdx = meshIdx;
                hitTriangleIdx = triangleIdx;
            }

        }

        return (hitTriangleIdx != -1) ? minT : -1.0;
    }
    else
    {
        double minDist = 1/EPSILON;
        double t = -1.0;
        int leftChildHitTriangleIdx = -1;
        int leftChildHitObjectIdx = -1;
        vec3 leftChildHitPoint;
        vec3 leftChildHitNormal;
        int rightChildHitTriangleIdx = -1;
        int rightChildHitObjectIdx = -1;
        vec3 rightChildHitPoint;
        vec3 rightChildHitNormal;

        if (node->left != nullptr)
        {
            t = this->shortestIntersectionInNode(node->left.get(), ray, leftChildHitTriangleIdx, leftChildHitObjectIdx, leftChildHitPoint, leftChildHitNormal);

            if (t > -EPSILON)
            {
                hitTriangleIdx = leftChildHitTriangleIdx;
                hitObjectIdx = leftChildHitObjectIdx;
                hitNormal = leftChildHitNormal;
                hitPoint = leftChildHitPoint;
                minDist = t;
            }
        }

        if (node->right != nullptr)
        {
            t = this->shortestIntersectionInNode(node->right.get(), ray, rightChildHitTriangleIdx, rightChildHitObjectIdx, rightChildHitPoint, rightChildHitNormal);

            if (t > -EPSILON && t < minDist)
            {
                hitTriangleIdx = rightChildHitTriangleIdx;
                hitObjectIdx = rightChildHitObjectIdx;
                hitNormal = rightChildHitNormal;
                hitPoint = rightChildHitPoint;
                minDist = t;
            }
        }

        return minDist;
    }
}


IntersectionData Scene::traceRayBVH(const Ray &ray)
{
    // find shortest intersecting triangle
    // find the point of intersection
    // get the normal of triangle

    IntersectionData iData;

    vec3 hitPoint;
    vec3 hitNormal;
    int hitTriangleIdx = -1;
    int hitObjectIdx = -1;
    Material* hitMaterial = nullptr;
    float shortestIntersection = -1.0;

    shortestIntersection = this->shortestIntersectionInNode(this->bvhRoot.get(), ray, hitTriangleIdx, hitObjectIdx, hitPoint, hitNormal);

    if (shortestIntersection > -EPSILON && hitObjectIdx > -1)
    {
        hitMaterial = &(this->geometryObjects[hitObjectIdx].material);
        iData.hitPoint = hitPoint;
        iData.hitPointNormal = hitNormal;
        iData.material = hitMaterial;
        iData.objectIdx = hitObjectIdx;
        iData.triangleIdx = hitTriangleIdx;

        iData.baryCentricCoords = this->geometryObjects[hitObjectIdx].findBaryCentricCoords(hitPoint, hitTriangleIdx);
        iData.interpolatedVertNormal = this->geometryObjects[hitObjectIdx].findInterpolatedVertNormal(iData.baryCentricCoords, hitTriangleIdx);
    }

    return iData;
}


std::unique_ptr<BVHNode> Scene::buildBVHTree(std::vector<Triangle>& allTrianglesInParent, int depth = 0)
{
    assert(!allTrianglesInParent.empty() && "No triangles to build a tree");

    auto node = std::unique_ptr<BVHNode>(new BVHNode());
    node->createBB(allTrianglesInParent);

    if ((int)allTrianglesInParent.size() <= min_triangles_per_bvhnode || depth >= max_bvhtree_depth)
    {
        node->triangleIndices.reserve(allTrianglesInParent.size());
        for (const auto& tri : allTrianglesInParent)
            node->triangleIndices.emplace_back(tri.meshIdx, tri.triangleIdx);

        return node;
    }

    int axis = depth % 3;
    std::sort(allTrianglesInParent.begin(), allTrianglesInParent.end(),
        [axis](const Triangle& a, const Triangle& b) {
            if (axis == 0) return a.centroid.x < b.centroid.x;
            if (axis == 1) return a.centroid.y < b.centroid.y;
            return a.centroid.z < b.centroid.z;
        });

    size_t mid = allTrianglesInParent.size() / 2;
    std::vector<Triangle> left(allTrianglesInParent.begin(), allTrianglesInParent.begin() + mid);
    std::vector<Triangle> right(allTrianglesInParent.begin() + mid, allTrianglesInParent.end());

    assert(!left.empty() && !right.empty());
    node->left = buildBVHTree(left, depth + 1);
    node->right = buildBVHTree(right, depth + 1);

    return node;
}

}

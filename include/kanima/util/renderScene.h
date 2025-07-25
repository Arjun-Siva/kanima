#ifndef RENDERSCENE_H
#define RENDERSCENE_H

#include <kanima/util/pixelBuffer.h>
#include <kanima/core/scene.h>
#include <kanima/shader/recursiveShader.h>

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>


// helper functions not exposed outside
namespace
{

using namespace krt;

struct Bucket
{
    int x, y, width, height;
};

// Shared queue and mutex
std::queue<Bucket> renderQueue;
std::mutex queueMutex;

void createBuckets(int imageWidth, int imageHeight, int bucketSize)
{
    for (int y = 0; y < imageHeight; y += bucketSize)
    {
        for (int x = 0; x < imageWidth; x += bucketSize)
        {
            int w = std::min(bucketSize, imageWidth - x);
            int h = std::min(bucketSize, imageHeight - y);
           renderQueue.push({x, y, w, h});
        }
    }
}

void renderRegion(Scene& scene, PixelBuffer& buffer, int startX, int startY, int region_width, int region_height, int ray_depth)
{
    Camera& camera = scene.camera;

    for (int y = startY; y < startY + region_height; ++y)
    {
        for (int x = startX; x < startX + region_width; ++x)
        {
            float u = (x + 0.5f) / buffer.getWidth();
            float v = (y + 0.5f) / buffer.getHeight();

            Ray ray = camera.generateRay(u, v);

            Color pixelColor = recursiveShader(ray, scene, ray_depth);
            buffer.setColor(x, y, pixelColor);
        }
    }
}

// Thread function
void workerThread(Scene& scene, PixelBuffer& buffer, int ray_depth)
{
    while (true)
    {
        Bucket region;
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (renderQueue.empty())
                break;

            region = renderQueue.front();
            renderQueue.pop();

        }
        renderRegion(scene, buffer, region.x, region.y, region.width, region.height, ray_depth);
    }
}

void bucketRender(Scene& scene, PixelBuffer& buffer, int numThreads, int bucketSize, int rayDepth)
{
   createBuckets(scene.width, scene.height, bucketSize);

   std::vector<std::thread> threads;
   for (int i = 0; i < numThreads; ++i)
   {
       threads.emplace_back(workerThread, std::ref(scene), std::ref(buffer), rayDepth);
   }

   for (auto& t : threads)
   {
       t.join();
   }
}

void buildBVHTree(Scene& scene, int min_triangles_per_bvhnode, int max_bvhtree_depth)
{
    std::vector<Triangle> ts = scene.getAllTrianglesInScene();
    scene.min_triangles_per_bvhnode = min_triangles_per_bvhnode;
    scene.max_bvhtree_depth = max_bvhtree_depth;
    scene.useBVH = true;
    std::unique_ptr<BVHNode> root = scene.buildBVHTree(ts, 0);
    scene.bvhRoot = std::move(root);
}


}

namespace krt
{

struct RenderConfig
{
    bool use_BVH = true;
    bool print_info = false;
    int max_tree_depth = 24;
    int min_triangles_per_leaf = 4;
    int buffer_width = 1280;
    int buffer_height = 720;
    int num_threads = 8;
    int bucket_size = 24;
    int ray_depth = 5;
};


PixelBuffer renderSceneToBuffer(Scene& scene, RenderConfig& config)
{
    PixelBuffer buffer(config.buffer_width, config.buffer_height);

    // config
    if (config.print_info)
    {
        std::cout<<"Config:"<<std::endl;
        if(config.use_BVH)
            std::cout<<"Using BVH tree optimization"<<std::endl;
        else
            std::cout<<"Not using BVH tree optimization"<<std::endl;
        std::cout<<"max_tree_depth:"<<config.max_tree_depth<<std::endl;
        std::cout<<"min_triangles_per_leaf:"<<config.min_triangles_per_leaf<<std::endl;
        std::cout<<"buffer_width:"<<config.buffer_width<<std::endl;
        std::cout<<"buffer_height:"<<config.buffer_height<<std::endl;
        std::cout<<"num_threads:"<<config.num_threads<<std::endl;
        std::cout<<"bucket_size:"<<config.bucket_size<<std::endl;
        std::cout<<"ray_depth:"<<config.ray_depth<<std::endl;
    }


    if (config.use_BVH)
    {
        if (config.print_info)
            std::cout<<"Building BVH tree start"<<std::endl;
        buildBVHTree(scene, config.min_triangles_per_leaf, config.max_tree_depth);
        if (config.print_info)
            std::cout<<"Building BVH tree completed"<<std::endl;
    }

    if (config.print_info)
        std::cout<<"Starting pixel-wise render"<<std::endl;

    // Start timer
    auto start = std::chrono::high_resolution_clock::now();

    bucketRender(scene, buffer, config.num_threads, config.bucket_size, config.ray_depth);

//    // End timer
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate duration
    std::chrono::duration<double> duration = end - start;
    if (config.print_info)
    {
        std::cout<<"Completed pixel-wise render"<<std::endl;
        std::cout << "Time taken: " << duration.count() << " seconds\n";
    }

    return buffer;
}



}
#endif // RENDERSCENE_H

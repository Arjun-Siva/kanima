#include <kanima/util/renderScene.h>

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
int numberOfBuckets;
bool printinfo = false;

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
    numberOfBuckets = renderQueue.size();
}

void renderRegion(Scene& scene, PixelBuffer& buffer, int startX, int startY, int region_width, int region_height, int ray_depth, int sample_per_pixel)
{
    Camera& camera = scene.camera;

    for (int y = startY; y < startY + region_height; ++y)
    {
        for (int x = startX; x < startX + region_width; ++x)
        {
            Color pixelColor = Color(0, 0, 0);
            for (int n = 0; n < sample_per_pixel; ++n)
            { //AA
                float x_offset = static_cast<float>(rand()) / RAND_MAX;
                float y_offset = static_cast<float>(rand()) / RAND_MAX;
                float u = (x + x_offset) / buffer.getWidth();
                float v = (y + y_offset) / buffer.getHeight();
                Ray ray = camera.generateRay(u, v);

                pixelColor = pixelColor + recursiveShader(ray, scene, ray_depth);
            }
            buffer.setColor(x, y, pixelColor * (1.0f / sample_per_pixel));
        }
    }
}

// Thread function
void workerThread(Scene& scene, PixelBuffer& buffer, int ray_depth, int sample_per_pixel)
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
        renderRegion(scene, buffer, region.x, region.y, region.width, region.height, ray_depth, sample_per_pixel);
        if (printinfo)
            std::cout<< (1.0f - static_cast<float>(renderQueue.size()) / numberOfBuckets) * 100 <<"% completed."<<std::endl;
    }
}

void bucketRender(Scene& scene, PixelBuffer& buffer, int numThreads, int bucketSize, int rayDepth, int sample_per_pixel)
{
   createBuckets(scene.width, scene.height, bucketSize);
   scene.bucketSize = bucketSize;

   std::vector<std::thread> threads;
   for (int i = 0; i < numThreads; ++i)
   {
       threads.emplace_back(workerThread, std::ref(scene), std::ref(buffer), rayDepth, sample_per_pixel);
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
PixelBuffer renderSceneToBuffer(Scene& scene, RenderConfig& config)
{
    PixelBuffer buffer(config.buffer_width, config.buffer_height);
    scene.height = config.buffer_height;
    scene.width = config.buffer_width;
    scene.gi_ray_count = config.gi_ray_count;
    printinfo = config.print_info;

    // config
    if (printinfo)
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
        std::cout<<"gi_ray_count:"<<config.gi_ray_count<<std::endl;
        std::cout<<"sample_per_pixel:"<<config.sample_per_pixel<<std::endl;
    }


    if (config.use_BVH)
    {
        if (scene.bvhRoot == nullptr || config.rebuild_BVH)
        {
            if (printinfo)
                std::cout<<"Building BVH tree start"<<std::endl;
            buildBVHTree(scene, config.min_triangles_per_leaf, config.max_tree_depth);
            if (printinfo)
                std::cout<<"Building BVH tree completed"<<std::endl;
        }
        else
        {
            if (printinfo)
                std::cout<<"Tree already built"<<std::endl;
        }
    }

    if (printinfo)
        std::cout<<"Starting pixel-wise render"<<std::endl;

    // Start timer
    auto start = std::chrono::high_resolution_clock::now();

    bucketRender(scene, buffer, config.num_threads, config.bucket_size, config.ray_depth, config.sample_per_pixel);

//    // End timer
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate duration
    std::chrono::duration<double> duration = end - start;
    if (printinfo)
    {
        std::cout<<"Completed pixel-wise render"<<std::endl;
        std::cout << "Time taken: " << duration.count() << " seconds\n";
    }

    return buffer;
}



}

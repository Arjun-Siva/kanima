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


PixelBuffer renderSceneToBuffer(Scene& scene, RenderConfig& config);

}
#endif // RENDERSCENE_H

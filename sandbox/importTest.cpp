#include <kanima/core/scene.h>
#include <kanima/util/renderScene.h>
#include <kanima/util/pixelBuffer.h>

int main()
{
    std::string sceneFileName = "dragon.crtscene";

    krt::Scene scene(sceneFileName);

    RenderConfig config;
    config.bucket_size = scene.bucketSize;
    config.buffer_height = scene.height;
    config.buffer_width = scene.width;
    config.max_tree_depth = 24;
    config.min_triangles_per_leaf = 4;
    config.num_threads = 8;
    config.use_BVH = true;
    config.ray_depth = 5;
    config.print_info = true;

    PixelBuffer output_buff = renderSceneToBuffer(scene, config);

    output_buff.writeToPPM("dragon.ppm");

}

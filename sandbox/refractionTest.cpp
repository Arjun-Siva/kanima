#include <kanima/core/scene.h>
#include <kanima/util/renderScene.h>
#include <kanima/util/pixelBuffer.h>

int main()
{
    std::string sceneFileName = "glassball.crtscene";

    krt::Scene scene(sceneFileName);

    krt::RenderConfig config;
    config.bucket_size = scene.bucketSize;
    config.buffer_height = scene.height;
    config.buffer_width = scene.width;
    config.max_tree_depth = 24;
    config.min_triangles_per_leaf = 4;
    config.num_threads = 8;
    config.use_BVH = true;
    config.ray_depth = 5;
    config.print_info = true;
    config.gi_ray_count = 0;
    config.sample_per_pixel = 4;

    krt::PixelBuffer output_buff = renderSceneToBuffer(scene, config);

    output_buff.writeToPPM("glassball.ppm");

}

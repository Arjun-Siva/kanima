# Kanima - Ray Tracer library

Kanima is a 3D ray tracer library built from scratch in C++11 with minimal dependencies (rapidjson and stb_image). It was developed in a 2 month sprint during [Chaos](https://www.chaos.com) Ray Tracing Bootcamp.
The name 'Kanima' is inspired from a popular Tamil song '[Kanimaa](https://youtu.be/5CEXL8kfGL4?feature=shared)' from the movie 'Retro'.

<img width="1122" height="616" alt="dragon" src="https://github.com/user-attachments/assets/c5d0c1a7-f538-4885-b9e9-ab092d5f73c3" />

Currently the library supports:

 - 2D rasterization
 - 3D ray tracing
 - Shading - Diffusive, Reflective, Refractive
 - Textures - Checkered, Barycentric-interpolated, Bitmap from images
 - Camera movements
 - Loading scene from a JSON file
 - Multithreading
 - BVH and Bounding Box optimizations
 - Anti-aliasing
 - Global illumination rays

The outputs are saved to a PPM file normally. However, the ray tracer can be extended with other graphics such as SDL (c.f [kanima-examples](https://github.com/Arjun-Siva/kanima-examples/tree/master/sdl2_gui)).

## Usage directions
 `Scene`object stores all the information related to the scene. `renderSceneToBuffer()`in *util* takes in a Scene object and a `Config`object and renders the scene in a multithreaded fashion and stores the result in a `PixelBuffer`. The buffer can be saved to a **.ppm** file.

`Mesh` objects store a set of triangles forming a mesh. The triangles' vertices are stored in a list of `vec3` objects and the triangle indices are stored in a list of integers. `Material` objects store the type of materials of a mesh - diffusive, reflective, refractive or constant. `Texture` objects store the information about various types of textures - Albedo, Checker, Bitmap and Edge textures. Each Material object has a pointer to a Texture object. Each Mesh object has a Material object.

A sample program to render a pyramid:

    #include <kanima/util/renderScene.h>
    #include <kanima/core/scene.h>
    #include <kanima/util/pixelBuffer.h>
    
    
    int main()
    {
       krt::Scene scene;
       krt::Mesh pyramid;
    
	   pyramid.insertVertex(-3.0, 0.0, -5.0);
	   pyramid.insertVertex(3.0, 0.0, -5.0);
	   pyramid.insertVertex(0.0, 4.0, -8.0);
	   pyramid.insertVertex(3.0, 0.0, -11.0);
	   pyramid.insertVertex(-3.0, 0.0, -11.0);
	   
	   pyramid.insertTriangleIndex(0, 1, 2); //front
	   pyramid.insertTriangleIndex(1, 3, 2); //right
	   pyramid.insertTriangleIndex(0, 2, 4); //left
	   pyramid.insertTriangleIndex(3, 4, 2); //back
	   
	   std::string name = "pyramid";
	   
	   krt::AlbedoTexture albedo = krt::AlbedoTexture("albedo", krt::Color(1, 1, 1));
	   std::shared_ptr<krt::AlbedoTexture> albedoPtr = std::make_shared<krt::AlbedoTexture>(albedo);
	   scene.addTexture(name, albedoPtr);
	   
	   krt::Material pyramidMaterial;
	   pyramidMaterial.type = krt::MaterialType::Diffuse;
	   pyramidMaterial.albedoTex = albedoPtr;
	   
	   pyramid.setMaterial(pyramidMaterial);
	   pyramid.computeAABB();
	   pyramid.computeTriangleNormals();
	   pyramid.computeVertexNormals();
	   
	   scene.addMaterial(pyramidMaterial);
	   scene.addMesh(pyramid);
	   
	   krt::vec3 lightpos(2, 5, -4);
	   krt::Light sun(lightpos, 500);
	   
	   scene.addLight(sun);
	   
	   scene.camera.dolly(-1);
	   scene.camera.boom(0.25);
	   
	   scene.bgColor = krt::Color(170.f/255.0, 231.f/255.0, 250.f/255.0);
	   
	   krt::RenderConfig config;
	   
	   config.buffer_height = 720;
	   config.buffer_width = 1280;
	   config.print_info = true;
	   config.bucket_size = 24;
	   config.max_tree_depth = 24;
	   config.min_triangles_per_leaf = 4;
	   config.num_threads = 8;
	   config.ray_depth = 2;
	   config.use_BVH = true;
	   
	   krt::PixelBuffer buffer = krt::renderSceneToBuffer(scene, config);
	   
	   buffer.writeToPPM("pyramid.ppm");
	   
	   return 0;
    
    }

The preferred way to create scenes is to load the information from a JSON file that contains all the meta-data such as resolution, ray depth etc., as well as the vertex and triangles data.

A sample JSON file:

    {
      "settings": {
              "background_color": [
                      0.7, 0.9, 1
              ],
              "image_settings": {
                      "width": 1920,
                      "height": 1080
              }
      },
    
      "camera": {
              "matrix": [
                      1, 0, 0,
                      0, 1, 0,
                      0, 0, 1
              ],
    
              "position": [
                      0, 0, 0
              ]
      },
    
      "lights": [
              {
                "intensity": 500,
                "position": [
                        0, 2, -2
                ]
              }
      ],
    
      "textures": [
              {
                "name": "red White Checker",
                "type": "checker",
                "color_A": [
                        0, 0, 0
                ],
                "color_B": [
                        1, 0, 0
                ],
                "square_size": 0.0125
              }
    
      ],
    
      "materials": [
              {
                "type": "diffuse",
                "albedo": "red White Checker",
                "smooth_shading": false
              }
    
      ],
    
      "objects": [
    
              {
                "material_index": 0,
                "vertices": [
                -4.5, -0.5, 1.5,
                4.5, -0.5, 1.5,
                -4.5, -0.5, -8.5,
                4.5, -0.5, -8.5
    
                ],
                "uvs":[
                    0, 0, 0,
                    1, 0, 0,
                    0, 1, 0,
                    1, 1, 0
                ],
                "triangles": [
                        0, 1, 2,
                        3, 2, 1
                ]
              }
      ]
    }

The JSON files in the sandbox and the ones used to generate the sample images are from Chaos Ray Tracing course.

More examples using the Kanima library can be found at : [kanima-examples](https://github.com/Arjun-Siva/kanima-examples/tree/master)

## Gallery
![cube_sph_0](https://github.com/user-attachments/assets/5b7997e1-decc-4a51-9b85-4e74235447d7)
![ball_12](https://github.com/user-attachments/assets/135c4999-6398-4f2a-845e-30edb32fcbd5)
![scene8_refract](https://github.com/user-attachments/assets/7074d003-5b0d-4861-aa40-41da5f455b38)
![scene2_scene](https://github.com/user-attachments/assets/a1a40267-97f3-4e04-ae94-0e7cd64df2ca)

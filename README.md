# Simple Snake 3D game made in OpenGL

### Project for Poznań University of Technology

#### 1. Contents
* apple.obj - apple model
* apple.png - apple texture
* cylinder.obj - snake body fragment model
* floor.h - floor vertices, normals and texture coordinates
* standard_fs.glsl - fragment shader implementing Lambertian shading
* lodepng.cpp, lodepng.h - library for loading images into GPU
* main_file.cpp - main application
* Makefile - makefile for easy compilation
* objloader.cpp, objloader.h - library for converting OBJ files into OpenGL format
* shderprogram.cpp, shaderprogram.h - shader program definition
* snake_head.obj - snake's head model
* snake.png - snake skin texture
* standard_vs.glsl - vertex shader implementing Lambertian shading
* wood.png - floor texture
* inverted_vs.glsl - vertex shader implementing Lambertian shading with inverted texture coordinates
* phong_fs.glsl - fragment shader implementing Phong shading
* phong_vs.glsl - vertex shader implementing Phong shading
* libs.h - libraries required in order to run the project
* libs.cpp - definitions for all classes and methods used in project

#### 2. Compilation
* **Using Makefile**:
```
$ make
$ ./main_file
```
* **Manually**:
```
$ g++ *.cpp *.h -lGL -lglfw -lGLEW -I .
$ ./a.out
```
* **Without compilation** (works only on Linux):
```
$ ./main_file
```

#### 3. Contributing
If you want to contribute to this repository, fork it and adapt it to your needs. This repository is mainly for educational purposes so it probably won't be updated. But if you see some glazing error in the code, feel free to submit pull request and it will be merged (if it will be logical). Don't expect Issue tickets to be solved though.

#### 4. Sources
* PUT IiT sources - Witold Andrzejewski, PhD. - for Shader Program
* [LodePNG](https://lodev.org/lodepng/) - for loading texture images to GPU
* [Suraj Sharma](https://www.youtube.com/channel/UC2i39AOpDSlO1Mrn1jQ8Xkg) - for OBJ loader
* [OpenGL Tutorials](https://learnopengl.com/) - for OpenGL basics
* [3dfood](https://www.turbosquid.com/3d-models/3d-apple-1199076) - for apple model
* [nudelkopf](https://www.cgtrader.com/free-3d-print-models/house/decor/hardsurface-padding-cylinder) - for snake body model
* [glm](https://github.com/g-truc/glm) - for simplifying model calculations

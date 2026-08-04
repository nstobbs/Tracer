# Tracer

#### What is Tracer?

Tracer is a C++ Ray Tracer Engine with an GUI Application for previewing renders and controlling the engine.

### Main Features:

* Mesh File Reading
* Bounding Volume Hierarchy - Surface Area Heuristic and Object Median Algorithms
* Object Instancer
* Surface Shaders - Solid Color, Textures, Wireframe, Surface Normals, Geometric Normals and Merge Surfaces.
* Materials - Diffuse, Glass and Metal
* Lights - Area Light and Dome Light
* Rendering Thread Pool
* EXR Writing Images
* HUDs - Tiles Crosshair and Bounding Box Overlay.

### Roadmap:

* Multiple Importance Sampling

### Dependencies:

* [Tiny EXR](https://github.com/syoyo/tinyexr)
* [stb](https://github.com/nothings/stb)
* [GLEW](https://github.com/nigels-com/glew)
* [GLM](https://github.com/g-truc/glm)
* [assimp](https://github.com/assimp/assimp)
* [SDL2](https://github.com/libsdl-org/SDL/tree/SDL2)
* [Dear ImGui](https://github.com/ocornut/imgui)

### Build:

###### Building Debug on Windows:

Clone the Tracer Github Repository:
`git clone git@github.com:nstobbs/Tracer.git`

Switch to the Tracer Folder:
`cd ./Tracer`

Create a Virtual Python Enviorment:
`python -m venv devenv`

Active the Python Enviorment:
`./devenv/Scripts/Activate.ps1`

Install Conan and CMake via pip:
`pip install -r ./requirements.txt`

Pull Git Submodules:
`git submodule update --init --recursive`

Install Conan Dependenceis:
`conan install . --build=missing`

Switch to the Build Folder:
`cd ./build`

Confirgure CMake:
`cmake .. -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -G "Visual Studio 17 2022"`

Build:
`cmake --build`

### TODO List:

* [ ] Write a better README.mb
* [ ] Build a Cornell Box Scene.
* [ ] Build a "Cool" Demo Scene.
* [ ] Move to using [Tiny](https://github.com/nstobbs/Tiny) instead of GLM.


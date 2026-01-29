add_library(Tracer STATIC
    Mesh.cpp
    Image.cpp
    Engine.cpp
    ThreadPool.cpp
    Tasker.cpp
    Scene.cpp
    Surface.cpp
    Camera.cpp
    Vertex.cpp
)

target_include_directories(Tracer PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

find_package(glm REQUIRED)
find_package(assimp)

target_link_libraries(Tracer
    glm::glm
    assimp::assimp
)

add_library(Tracer STATIC
    Image.cpp
    Scene.cpp
    Camera.cpp
)

add_subdirectory(Object)
add_subdirectory(Engine)
add_subdirectory(Surface)

target_include_directories(Tracer PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

find_package(glm REQUIRED)
find_package(assimp)

target_link_libraries(Tracer
    Object
    Engine
    Surface
    glm::glm
    assimp::assimp
    stb
    Window
)

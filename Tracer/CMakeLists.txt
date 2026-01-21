add_library(Tracer STATIC
    Object.cpp
    Mesh.cpp
    Image.cpp
    Engine.cpp
    ThreadPool.cpp
    Scene.cpp
    Surface.cpp
)

target_include_directories(Tracer PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

find_package(glm REQUIRED)

target_link_libraries(Tracer
    glm::glm
)

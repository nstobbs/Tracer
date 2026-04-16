add_library(Object STATIC
    BoundingBox.cpp
    BoundingVolumeHierarchy.cpp
    Mesh.cpp
    Vertex.cpp
)

target_include_directories(Object PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

find_package(glm REQUIRED)
find_package(assimp)

target_link_libraries(Object
    Tracer
    glm::glm
    assimp::assimp
)

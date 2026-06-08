add_library(Object STATIC
    BoundingBox.cpp
    BoundingVolumeHierarchy.cpp
    Mesh.cpp
    Transform.cpp
)

target_include_directories(Object PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

find_package(glm REQUIRED)
find_package(assimp REQUIRED)

target_link_libraries(Object
    Tracer::Core
    glm::glm
    assimp::assimp
)

add_library(Tracer::Object ALIAS Object)

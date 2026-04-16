add_library(Surface STATIC
    Surface.cpp
    GeometricNormals.cpp
    MergeSurface.cpp
    SolidColor.cpp
    SurfaceNormals.cpp
    UVTexture.cpp
    VertexColor.cpp
    Wireframe.cpp
)

target_include_directories(Surface PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

find_package(glm REQUIRED)
find_package(assimp)

target_link_libraries(Surface
    glm::glm
    assimp::assimp
    stb
    Window
)

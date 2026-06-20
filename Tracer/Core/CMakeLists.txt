add_library(Core STATIC
    Camera.cpp
    Image.cpp
    Scene.cpp
    StatusMessage.cpp
)

target_include_directories(Core PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

find_package(glm REQUIRED)

target_link_libraries(Core
    stb
    glm::glm
    tinyexr::tinyexr
    Tracer::Object
)

add_library(Tracer::Core ALIAS Core)
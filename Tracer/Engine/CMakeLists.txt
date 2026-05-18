add_library(Engine STATIC
    Engine.cpp
    ThreadPool.cpp
    Tasker.cpp
)

target_include_directories(Engine PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

find_package(glm REQUIRED)
find_package(assimp)

target_link_libraries(Engine
    glm::glm
    Application::Window
    Tracer::Material
)

add_library(Tracer::Engine ALIAS Engine)
add_library(Window STATIC
    Window.cpp
    OpenGlHelper.cpp
    DisplayLayerRenderPass.cpp
    TileCrosshairRenderPass.cpp
    BBoxRenderPass.cpp
)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(GLEW REQUIRED)

target_include_directories(Window PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_link_libraries(Window
    Tracer
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_image::SDL2_image
    GLEW::GLEW
)
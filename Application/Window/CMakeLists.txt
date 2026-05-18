add_library(Window STATIC
    Window.cpp
    OpenGlHelper.cpp
    DisplayLayerRenderPass.cpp
    TileCrosshairRenderPass.cpp
    BBoxRenderPass.cpp
    
    # ImGui Backend Bindings
    bindings/imgui_impl_sdl2.cpp
    bindings/imgui_impl_opengl3.cpp
)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(GLEW REQUIRED)
find_package(imgui REQUIRED)

target_include_directories(Window PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/bindings
)

target_link_libraries(Window
    Tracer::Core
    Tracer::Engine
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_image::SDL2_image
    GLEW::GLEW
    imgui::imgui
)

add_library(Application::Window ALIAS Window)
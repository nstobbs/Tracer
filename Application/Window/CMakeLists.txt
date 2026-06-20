add_library(Window STATIC
    Window.cpp
    OpenGlHelper.cpp
    DisplayLayerRenderPass.cpp
    TileCrosshairRenderPass.cpp
    BBoxRenderPass.cpp
    
    # ImGui Backend Bindings
    imgui_bindings/imgui_impl_sdl2.cpp
    imgui_bindings/imgui_impl_opengl3.cpp
    # ImGui Misc 
    imgui_misc/imgui_stdlib.cpp
)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(GLEW REQUIRED)
find_package(imgui REQUIRED)

target_include_directories(Window PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_bindings
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_misc
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
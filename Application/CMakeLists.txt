add_subdirectory(Window)

add_executable(Application
        main.cpp
        Application.cpp
)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(GLEW REQUIRED)

target_include_directories(Application PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Application PUBLIC
# Tracer Libraries 
    Tracer
    Object
    Surface
    Window
# Conan Libraries
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_image::SDL2_image
    GLEW::GLEW
)
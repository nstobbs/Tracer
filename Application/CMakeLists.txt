add_executable(Application
        main.cpp
        Application.cpp
)

find_package(raylib REQUIRED)

target_include_directories(Application PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Application PUBLIC
    Tracer
    raylib
)
add_library(Tracer STATIC
    Object.cpp
)

target_include_directories(Tracer PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

find_package(glm REQUIRED)

target_link_libraries(Tracer
    glm::glm
)

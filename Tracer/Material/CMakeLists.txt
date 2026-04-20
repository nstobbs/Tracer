add_library(Material INTERFACE)

target_include_directories(Material INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_link_libraries(Material INTERFACE
    Tracer
    Object
    Surface
)

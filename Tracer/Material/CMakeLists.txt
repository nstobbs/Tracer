add_library(Material INTERFACE)

target_include_directories(Material INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_link_libraries(Material INTERFACE
    Tracer::Core
    Tracer::Object
    Tracer::Surface
)

add_library(Tracer::Material ALIAS Material)

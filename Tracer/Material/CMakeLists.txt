add_library(Material STATIC
    Material.cpp)

target_include_directories(Material INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_link_libraries(Material PUBLIC
    Tracer::Core
    Tracer::Object
    Tracer::Surface
)

add_library(Tracer::Material ALIAS Material)

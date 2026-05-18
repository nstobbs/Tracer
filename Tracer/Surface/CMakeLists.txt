add_library(Surface INTERFACE
)

target_include_directories(Surface INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_link_libraries(Surface INTERFACE
    Tracer::Core
    Application::Window
)

add_library(Tracer::Surface ALIAS Surface)
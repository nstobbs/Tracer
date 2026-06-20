add_library(tinyexr INTERFACE)

target_include_directories(tinyexr INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/tinyexr
	${CMAKE_CURRENT_SOURCE_DIR}/tinyexr/deps/miniz
)

add_library(tinyexr::tinyexr ALIAS tinyexr)
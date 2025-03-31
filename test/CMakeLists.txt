
add_executable(CradleTest
	cradle_test.cpp
)
target_link_libraries(CradleTest
	libtomcrypt
	SDL3::SDL3-static
	SDL3_image::SDL3_image
)


target_include_directories(CradleTest PUBLIC
 ".."
)

if (LINUX)
	target_link_libraries(CradleTest
	GL
)
endif()

if (WIN32)
	target_link_libraries(CradleTest 
		opengl32
		glu32
		wsock32
		openal32 )
endif()

if (APPLE)
target_link_libraries (CradleTest
	"-framework OpenGL" 
	"-framework OpenAL"
)

if (EMSCRIPTEN)
    message("got here")
    set_target_properties(CradleTest PROPERTIES LINK_FLAGS "-lGL -sLEGACY_GL_EMULATION -sSTACK_SIZE=2000000 --preload-file ../../exe/images@images/ --preload-file ../../exe/sfx@sfx/ --preload-file ../../exe/levels@levels/ --preload-file ../../exe/models@models/ --bind")

#    set_target_properties(CradleTest PROPERTIES LINK_FLAGS "-lGL -sLEGACY_GL_EMULATION -sEXIT_RUNTIME=1 -sTOTAL_MEMORY=33554432 --preload-file ../../exe/examples@examples/ -sSTACK_SIZE=2000000")

endif()

set_target_properties(CradleTest PROPERTIES
  XCODE_GENERATE_SCHEME TRUE
  XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test"
)
endif()
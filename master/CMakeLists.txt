add_executable(master
	master.cpp
)

target_include_directories(master PUBLIC "..")
target_link_libraries(master libtomcrypt)

add_executable(test_game
    test_game.cpp
)

target_include_directories(test_game PUBLIC "..")
target_link_libraries(test_game libtomcrypt)

if (WIN32)
	target_link_libraries(master 
		wsock32
		winmm
		 )
	target_link_libraries(test_game 
		wsock32
		winmm
		 )
endif()

if(APPLE)
set_target_properties(master PROPERTIES
  XCODE_GENERATE_SCHEME TRUE
  XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/master"
)
set_target_properties(test_game PROPERTIES
  XCODE_GENERATE_SCHEME TRUE
)

endif()

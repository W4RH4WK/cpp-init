add_executable(example_app example_app.cpp)
example_compile_options(example_app)
target_link_libraries(example_app PUBLIC example fmt)

if(WIN32)
	# This will copy DLLs to the target's output directory such that the
	# executable can be launched directly.
	#add_custom_command(TARGET example_app POST_BUILD
	#	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:example_app> $<TARGET_FILE_DIR:example_app>
	#	COMMAND_EXPAND_LISTS)
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT example_app)

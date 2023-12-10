cmake_minimum_required(VERSION 3.22)
cmake_policy(SET CMP0091 NEW) # MSVC_RUNTIME_LIBRARY

project(example LANGUAGES C CXX)
set(CMAKE_CONFIGURATION_TYPES Debug;Release)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# CMake usually adds a pragma system_header to the precompiled header. We don't
# want that as we suppresses warnings from system headers.
set(CMAKE_PCH_PROLOGUE "")

function(example_compile_options target)
	set_target_properties(${target} PROPERTIES
		CXX_STANDARD 20
		CXX_EXTENSIONS OFF)

	target_compile_options(${target} PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/MP /W4 /external:W0 /bigobj /utf-8>
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RELEASE>>:/Oi>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Wtype-limits -Wconversion -Wsign-conversion -Wdouble-promotion -Wundef -g>

		# Enable MSVC debug symbols and edit-and-continue support
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:DEBUG>>:/ZI>
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RELEASE>>:/Zi>

		# Colorize compiler output
		$<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color>
		$<$<CXX_COMPILER_ID:Clang,AppleClang>:-fcolor-diagnostics>)

	get_target_property(target_type ${target} TYPE)
	if(target_type STREQUAL EXECUTABLE)
		target_link_options(${target} PRIVATE
			$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RELEASE>>:/OPT:REF /OPT:ICF /DEBUG>)
		set_target_properties(${target} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
	endif()

	get_target_property(target_srcs ${target} SOURCES)
	source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${target_srcs})
endfunction()

# Include all external dependencies
file(GLOB externals CONFIGURE_DEPENDS LIST_DIRECTORIES TRUE external/*)
foreach(external ${externals})
	add_subdirectory(${external})
endforeach()

enable_testing()

add_subdirectory(code/example)
add_subdirectory(code/example_app)

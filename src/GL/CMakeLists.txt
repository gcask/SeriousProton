cmake_minimum_required(VERSION 3.7)
project(glad)

#---------------------------------File lists-----------------------------------
set(source_files_relative
	glad.c
)
set(source_files "")
foreach(source_file_relative ${source_files_relative})
	list(APPEND source_files ${CMAKE_CURRENT_SOURCE_DIR}/${source_file_relative})
endforeach()

#----------------------------------Compiling-----------------------------------
add_library(glad STATIC ${source_files})

target_include_directories(glad
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)

#--------------------------------Installation----------------------------------
install(
	TARGETS glad
	EXPORT glad
	DESTINATION lib
)
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
install(
	EXPORT glad
	DESTINATION share/glad
)

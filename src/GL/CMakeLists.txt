cmake_minimum_required(VERSION 3.7)
project(glad)

# GlAD link used:
# Release: http://glad.dav1d.de/#profile=compatibility&api=gl%3Dnone&api=gles1%3Dnone&api=gles2%3D2.0&api=glsc2%3Dnone&extensions=GL_AMD_performance_monitor&extensions=GL_EXT_blend_minmax&extensions=GL_EXT_draw_elements_base_vertex&extensions=GL_EXT_map_buffer_range&extensions=GL_EXT_multi_draw_arrays&extensions=GL_EXT_separate_shader_objects&extensions=GL_EXT_texture_border_clamp&extensions=GL_EXT_texture_compression_s3tc&extensions=GL_KHR_context_flush_control&extensions=GL_KHR_debug&extensions=GL_KHR_texture_compression_astc_ldr&extensions=GL_NV_draw_buffers&extensions=GL_NV_pixel_buffer_object&extensions=GL_OES_get_program_binary&extensions=GL_OES_mapbuffer&extensions=GL_OES_required_internalformat&extensions=GL_OES_texture_npot&extensions=GL_OES_vertex_array_object&extensions=GL_OES_vertex_half_float&language=c&specification=gl&localfiles=on
# Debug: http://glad.dav1d.de/#profile=compatibility&api=gl%3Dnone&api=gles1%3Dnone&api=gles2%3D2.0&api=glsc2%3Dnone&extensions=GL_AMD_performance_monitor&extensions=GL_EXT_blend_minmax&extensions=GL_EXT_draw_elements_base_vertex&extensions=GL_EXT_map_buffer_range&extensions=GL_EXT_multi_draw_arrays&extensions=GL_EXT_separate_shader_objects&extensions=GL_EXT_texture_border_clamp&extensions=GL_EXT_texture_compression_s3tc&extensions=GL_KHR_context_flush_control&extensions=GL_KHR_debug&extensions=GL_KHR_texture_compression_astc_ldr&extensions=GL_NV_draw_buffers&extensions=GL_NV_pixel_buffer_object&extensions=GL_OES_get_program_binary&extensions=GL_OES_mapbuffer&extensions=GL_OES_required_internalformat&extensions=GL_OES_texture_npot&extensions=GL_OES_vertex_array_object&extensions=GL_OES_vertex_half_float&language=c-debug&specification=gl&localfiles=on


#---------------------------------File lists-----------------------------------
set(source_files_relative
	$<IF:$<CONFIG:Debug>,debug/glad.c,release/glad.c>
	$<IF:$<CONFIG:Debug>,debug/GL/glad.h,release/GL/glad.h>
)
set(source_files "")
foreach(source_file_relative ${source_files_relative})
	list(APPEND source_files ${CMAKE_CURRENT_SOURCE_DIR}/${source_file_relative})
endforeach()

#----------------------------------Compiling-----------------------------------
add_library(glad STATIC ${source_files})

target_include_directories(glad
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/$<IF:$<CONFIG:Debug>,debug,release>>
	PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/$<IF:$<CONFIG:Debug>,debug,release>/GL>)

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

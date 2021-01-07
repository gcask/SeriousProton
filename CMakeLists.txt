cmake_minimum_required(VERSION 3.7)
project(SeriousProton VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MODULE_PATH
	${CMAKE_SOURCE_DIR}/cmake/
	${CMAKE_MODULE_PATH}
)

#--------------------------------Dependencies----------------------------------
find_package(SFML 2.3 REQUIRED COMPONENTS system audio network)
find_package(SDL2 REQUIRED)
add_subdirectory("${PROJECT_SOURCE_DIR}/src/Box2D")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/lua")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/GL")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/json11")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/SFML")


#---------------------------------File lists-----------------------------------
set(SOURCES #All SeriousProton's objects to compile
	src/clipboard.cpp
	src/collisionable.cpp
	src/engine.cpp
	src/event.cpp
	src/fixedSocket.cpp
	src/gameEntity.cpp
	src/httpServer.cpp
	src/input.cpp
	src/logging.cpp
	src/multiplayer.cpp
	src/multiplayer_client.cpp
	src/multiplayer_proxy.cpp
	src/multiplayer_server.cpp
	src/multiplayer_server_scanner.cpp
	src/networkAudioStream.cpp
	src/networkRecorder.cpp
	src/PlayerController.cpp
	src/postProcessManager.cpp
	src/random.cpp
	src/Renderable.cpp
	src/resources.cpp
	src/scriptInterface.cpp
	src/scriptInterfaceMagic.cpp
	src/shaderManager.cpp
	src/soundManager.cpp
	src/stringImproved.cpp
	src/textureManager.cpp
	src/tween.cpp
	src/Updatable.cpp
	src/windowManager.cpp
	src/i18n.cpp
	src/P.cpp
	src/sfml2sdl.cpp
)

add_library(SeriousProton STATIC ${SOURCES})

target_compile_options(${PROJECT_NAME} PUBLIC
	$<$<NOT:$<BOOL:${MSVC}>>:-Wall -Werror=return-type>
	$<$<BOOL:${MSVC}>:/MP>
)

target_include_directories(SeriousProton PUBLIC "${SFML_INCLUDE_DIR}"
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>  
	$<INSTALL_INTERFACE:include/seriousproton>  # <prefix>/include/mylib
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/libopus/include>
	${SDL2_INCLUDE_DIRS}
)
target_compile_definitions(${PROJECT_NAME} PUBLIC
	SFML_NO_DEPRECATED_WARNINGS # SFML 2.5 gives deprication warnings on a few functions we use. But we maintain 2.3 compatibility, so ignore those warnings.
	# Platform-specific
	$<$<BOOL:${WIN32}>:NOMINMAX>
)
target_link_libraries(SeriousProton PUBLIC box2d lua glew $<BUILD_INTERFACE:json11> $<BUILD_INTERFACE:SFMLoverSDL> ${SFML_LIBRARIES} ${SDL2_LIBRARIES})

#--------------------------------Installation----------------------------------
install(
	TARGETS SeriousProton
	EXPORT seriousproton
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
install(
	DIRECTORY "${PROJECT_SOURCE_DIR}/src"
	DESTINATION include/seriousproton
	FILES_MATCHING PATTERN "*.h"
)
install(
	DIRECTORY "${PROJECT_SOURCE_DIR}/src"
	DESTINATION include/seriousproton
	FILES_MATCHING PATTERN "*.hpp"
)
install(
	EXPORT seriousproton
	DESTINATION share/seriousproton
)

cmake_minimum_required(VERSION 3.7)
project(SFMLoverSDL VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
find_package(SFML COMPONENTS system REQUIRED)
find_package(SDL2 REQUIRED)

add_subdirectory(glm)

set(SOURCES
	# Graphics
	Graphics/BlendMode.hpp
	Graphics/CircleShape.hpp
	Graphics/Color.hpp
	Graphics/Drawable.hpp
	Graphics/Font.hpp
	Graphics/Glsl.hpp
	Graphics/Glyph.hpp
	Graphics/Image.hpp
	Graphics/Rect.hpp
	Graphics/RectangleShape.hpp
	Graphics/RenderStates.hpp
	Graphics/RenderTarget.hpp
	Graphics/RenderTexture.hpp
	Graphics/RenderWindow.hpp
	Graphics/Shader.hpp
	Graphics/Shape.hpp
	Graphics/Sprite.hpp
	Graphics/Text.hpp
	Graphics/Texture.hpp
	Graphics/Transform.hpp
	Graphics/Transformable.hpp
	Graphics/Vertex.hpp
	Graphics/View.hpp

	Graphics/Implementation.cpp

	# Window
	Window/ContextSettings.hpp
	Window/VideoMode.hpp
	Window/WindowStyle.hpp

	Window/Implementation.cpp

	stb/stb_rect_pack.h
	stb/stb_image.h
	stb/stb_truetype.h
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_compile_options(${PROJECT_NAME} PUBLIC
	$<$<NOT:$<BOOL:${MSVC}>>:-Wall -Werror=return-type>
	$<$<BOOL:${MSVC}>:/MP>
)

target_include_directories(${PROJECT_NAME}
	PUBLIC ${SFML_INCLUDE_DIR}
	PRIVATE ..)
target_compile_definitions(${PROJECT_NAME} PUBLIC
	SFML_NO_DEPRECATED_WARNINGS # SFML 2.5 gives deprication warnings on a few functions we use. But we maintain 2.3 compatibility, so ignore those warnings.
	# Platform-specific
	$<$<BOOL:${WIN32}>:NOMINMAX>
)
target_link_libraries(${PROJECT_NAME}
	PUBLIC SDL2::SDL2 ${SFML_LIBRARIES} glm::glm
	PRIVATE glad)

add_executable(SFMLTest WIN32 main.cpp EXCLUDE_FROM_ALL)
target_link_libraries(SFMLTest ${PROJECT_NAME} glad)
target_include_directories(SFMLTest PRIVATE ..)
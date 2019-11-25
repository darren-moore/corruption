cmake_minimum_required (VERSION 2.6)
project (BulletGameEngine)
project (EngineCore)

set(SRC "")
set(SRC ${SRC} src/src_Game/main.cpp)

# Core engine
file(GLOB SRC_CORE "src/src_Core/Core/*.cpp" "src/src_Core/Core/*.h")
source_group("Source Files\\Core" FILES ${SRC_CORE})

file(GLOB SRC_CORE_MATH "src/src_Core/Core/math/*.cpp" "src/src_Core/Core/math/*.h")
source_group("Source Files\\Core\\math" FILES ${SRC_CORE_MATH})

file(GLOB SRC_CORE_GRAPHICS "src/src_Core/Core/graphics/*.cpp" "src/src_Core/Core/graphics/*.h")
source_group("Source Files\\Core\\graphics" FILES ${SRC_CORE_GRAPHICS})

file(GLOB SRC_CORE_SUBENGINE "src/src_Core/Core/subengine/*.cpp" "src/src_Core/Core/subengine/*.h")
source_group("Source Files\\Core\\subengine" FILES ${SRC_CORE_SUBENGINE})


# Game systems
file(GLOB SRC_GAME "src/src_Game/Game/*.cpp" "src/src_Game/Game/*.h")
source_group("Source Files\\Game" FILES ${SRC_GAME})

# glad
include_directories(foreign/glad/include)
set(SRC_CORE ${SRC_CORE} foreign/glad/Src/glad.c)

# stb_image
include_directories(foreign/stb)
set(SRC_CORE ${SRC_CORE} foreign/stb/stb/stb_image.h)

add_library(EngineCore ${SRC_CORE} ${SRC_CORE_MATH} ${SRC_CORE_GRAPHICS} ${SRC_CORE_SUBENGINE})
include_directories(src/src_Core)

# irrklang
find_library(IRRKLANG_LIB NAMES "irrKlang" PATHS foreign/irrKlang-1.6.0/lib/Win32-visualStudio)
include_directories(foreign/irrKlang-1.6.0/include)
target_link_libraries(EngineCore ${IRRKLANG_LIB})

# lua
include_directories(foreign/lua-5.3.5/src)
file(GLOB LUA_SRC "foreign/lua-5.3.5/src/*.c")
add_library(lua STATIC ${LUA_SRC})
target_link_libraries(EngineCore lua)


add_executable(BulletGameEngine ${SRC} ${SRC_GAME})
target_link_libraries(BulletGameEngine opengl32.lib)
target_link_libraries(BulletGameEngine EngineCore)

# bulletml
add_subdirectory(foreign/bulletml)
include_directories(foreign/bulletml/src)
target_link_libraries(BulletGameEngine bulletml)

# glfw
add_subdirectory(foreign/glfw-3.3)
include_directories(foreign/glfw-3.3/include)
target_link_libraries(BulletGameEngine glfw ${GLFW_LIBRARIES})

# freetype
add_subdirectory(foreign/freetype-2.10.0)
include_directories(foreign/freetype-2.10.0/include)
target_link_libraries(EngineCore freetype)
cmake_minimum_required (VERSION 2.6)
project (bulletml)

file(GLOB SRC "*.cpp" "src/*.cpp" "src/*.yy")
file(GLOB SRC_TINYXML "src/tinyxml/*.cpp")
file(GLOB SRC_YGG "src/ygg/*.cpp")

# ignoring the tests
list(REMOVE_ITEM SRC_TINYXML ${CMAKE_CURRENT_SOURCE_DIR}/src/tinyxml/tinyxmltest.cpp)
list(REMOVE_ITEM SRC_YGG ${CMAKE_CURRENT_SOURCE_DIR}/src/ygg/ygg_test.cpp)

# for boost
include_directories(bulletml src)

add_compile_definitions(BULLETML_EXPORTS)

add_library(bulletml ${SRC} ${SRC_YGG} ${SRC_TINYXML})
target_link_libraries(bulletml odbc32.lib odbccp32.lib)

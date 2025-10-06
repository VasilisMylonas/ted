cmake_minimum_required(VERSION 3.5)

project(ted CXX)

file(GLOB SOURCES src/*.cpp)

add_executable(ted ${SOURCES})
target_include_directories(ted PRIVATE include ${PROJECT_BINARY_DIR}/include)

find_package(wxWidgets COMPONENTS core base stc REQUIRED)

target_compile_options(ted PRIVATE -Wall -Wextra -Werror)
target_compile_features(ted PRIVATE cxx_std_20)
target_link_libraries(ted PRIVATE wxWidgets::wxWidgets)

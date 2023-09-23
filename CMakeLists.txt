cmake_minimum_required(VERSION 3.2)

project(ted 
    VERSION 0.1 
    DESCRIPTION "A text editor" 
    LANGUAGES CXX
)

add_subdirectory(deps/wxWidgets-3.2.2.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

add_executable(ted)
target_include_directories(ted PRIVATE include ${PROJECT_BINARY_DIR}/include)
target_sources(ted PRIVATE
    src/App.cpp 
    src/MainFrame.cpp
    src/MenuBar.cpp
    src/MainPresenter.cpp
)
target_compile_options(ted PUBLIC -Wall -Wextra -Werror -Wpedantic) # -Wconversion
target_link_libraries(ted wx::core wx::base)

configure_file(config.hpp.in include/config.hpp)


cmake_minimum_required(VERSION 3.10)

project(app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_executable(app 
    src/main.cpp 
    src/character.cpp
)

target_link_libraries(app PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)
add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SFML::System>
    $<TARGET_FILE:SFML::Window>
    $<TARGET_FILE:SFML::Graphics>
    $<TARGET_FILE_DIR:app>
)
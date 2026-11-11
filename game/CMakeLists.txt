cmake_minimum_required(VERSION 3.10)

project(app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

add_executable(app
    main.cpp
    src/player/player.cpp
    src/player/player-update.cpp
    src/player/player-collision.cpp
    src/level/level.cpp
    src/level/level-textures.cpp
    src/camera/camera.cpp
    src/state-manager/state-manager.cpp
    src/menu/main-menu.cpp
    src/hud/hud.cpp
    src/audio/audio.cpp
)

if (MSVC)
    target_compile_options(app PRIVATE /utf-8)
endif()

target_link_libraries(app PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)
add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SFML::System>
    $<TARGET_FILE:SFML::Window>
    $<TARGET_FILE:SFML::Graphics>
    $<TARGET_FILE:SFML::Audio>
    $<TARGET_FILE_DIR:app>
)
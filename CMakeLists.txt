cmake_minimum_required(VERSION 3.0.0)
project(groundbreaker VERSION 0.1.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED true)

include(CTest)
enable_testing()

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_gfx REQUIRED)
find_package(SDL2_sound REQUIRED)
find_package(SDL2_mixer REQUIRED)

find_package(Threads REQUIRED)

add_executable(groundbreaker main.c config.c cache.c utils.c font.c timer.c map.c display.c menu.c loop.c lobby.c game.c moves.c client.c server.c dialog.c player.c sound.c settings.c ia.c)

target_link_libraries(groundbreaker SDL2::SDL2 ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES} SDL2::GFX ${SDL2_SOUND_LIBRARIES} ${SDL2_MIXER_LIBRARIES} Threads::Threads)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

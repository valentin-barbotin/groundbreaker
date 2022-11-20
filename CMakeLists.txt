cmake_minimum_required(VERSION 3.0.0)
project(groundbreaker VERSION 0.1.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED true)

set(DISCORD_LIB "${CMAKE_SOURCE_DIR}/discord/lib/x86_64/")

if (WIN32)
set(CMAKE_TOOLCHAIN_FILE "C:/Users/valen/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(VCPKG_TARGET_TRIPLET "x86-windows" CACHE STRING "")
endif(WIN32)

include(CTest)
enable_testing()

if (UNIX OR APPLE)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(SDL2_gfx REQUIRED)
    find_package(SDL2_sound REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_package(Threads REQUIRED)
endif()

if (WIN32)
    set(SDL2_DIR "C:/Users/valen/vcpkg/installed/x64-windows/share/sdl2")
    set(SDL2-image_DIR "C:/Users/valen/vcpkg/installed/x64-windows/share/sdl2-image")
    set(SDL2-ttf_DIR "C:/Users/valen/vcpkg/installed/x64-windows/share/sdl2-ttf")
    set(SDL2-gfx_DIR "C:/Users/valen/vcpkg/installed/x64-windows/share/sdl2-gfx")
    set(SDL2-mixer_DIR "C:/Users/valen/vcpkg/installed/x64-windows/share/sdl2-mixer")
    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2-image CONFIG REQUIRED)
    find_package(SDL2-ttf REQUIRED)
    find_package(SDL2-gfx REQUIRED)
    # find_package(SDL2_sound REQUIRED)
    find_package(SDL2-mixer REQUIRED)
endif(WIN32)

add_executable(groundbreaker main.c config.c cache.c utils.c font.c timer.c map.c display.c menu.c loop.c lobby.c game.c moves.c client.c server.c dialog.c player.c sound.c inventory.c settings.c items.c ia.c discord.c effects.c sound.c)

add_library(DISCORD SHARED IMPORTED)
if (UNIX)
    set_property(TARGET DISCORD PROPERTY IMPORTED_LOCATION ${DISCORD_LIB}/discord_game_sdk.so)
endif(UNIX)

if (APPLE)
    set_property(TARGET DISCORD PROPERTY IMPORTED_LOCATION ${DISCORD_LIB}/discord_game_sdk.dylib)
endif(APPLE)


if(WIN32)
    set_property(TARGET DISCORD PROPERTY IMPORTED_LOCATION ${DISCORD_LIB}/discord_game_sdk.dll)
endif(WIN32)

if (UNIX OR APPLE)
    target_link_libraries(groundbreaker DISCORD SDL2::SDL2 ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES} SDL2::GFX ${SDL2_SOUND_LIBRARIES} ${SDL2_MIXER_LIBRARIES} Threads::Threads)
endif(UNIX OR APPLE)

if (WIN32)
target_link_libraries(groundbreaker DISCORD SDL2::SDL2 ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${SDL2_GFX_LIBRARIES} ${SDL2_SOUND_LIBRARIES} ${SDL2_MIXER_LIBRARIES})

# target_link_libraries(groundbreaker DISCORD SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_image SDL2::SDL2_ttf SDL2::SDL2_gfx ${SDL2_SOUND_LIBRARIES} SDL2::mixer)
endif(WIN32)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

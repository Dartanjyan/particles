cmake_minimum_required(VERSION 3.16)

project(particles LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB PARTICLE_SOURCES CONFIGURE_DEPENDS "${SRC_DIR}/*.c")

add_executable(particles ${PARTICLE_SOURCES})
set_target_properties(particles PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_include_directories(particles PRIVATE "${SRC_DIR}")
target_compile_options(particles PRIVATE -O2)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(SDL_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/SDL/install")
    set(CHIPMUNK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/chipmunk/install")

    find_library(SDL2_LIBRARY NAMES SDL2
        HINTS "${SDL_ROOT}/lib")
    find_library(SDL2MAIN_LIBRARY NAMES SDL2main
        HINTS "${SDL_ROOT}/lib")
    find_library(CHIPMUNK_LIBRARY NAMES chipmunk
        HINTS "${CHIPMUNK_ROOT}/lib")
    if(NOT SDL2_LIBRARY OR NOT SDL2MAIN_LIBRARY OR NOT CHIPMUNK_LIBRARY)
        message(FATAL_ERROR "Windows SDL2 or Chipmunk2D libraries were not found")
    endif()
    find_file(WINPTHREAD_DLL NAMES libwinpthread-1.dll
        HINTS
            "/usr/x86_64-w64-mingw32/lib"
            "/usr/x86_64-w64-mingw32/bin")

    target_include_directories(particles PRIVATE
        "${SDL_ROOT}/include"
        "${CHIPMUNK_ROOT}/include")
    target_compile_definitions(particles PRIVATE
        main=SDL_main)
    target_link_libraries(particles PRIVATE
        mingw32
        "${SDL2MAIN_LIBRARY}"
        "${SDL2_LIBRARY}"
        "${CHIPMUNK_LIBRARY}"
        "-lwinpthread")
    set_target_properties(particles PROPERTIES
        WIN32_EXECUTABLE ON)

    add_custom_command(TARGET particles POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${SDL_ROOT}/bin/SDL2.dll"
            "$<TARGET_FILE_DIR:particles>")

    if(WINPTHREAD_DLL)
        add_custom_command(TARGET particles POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${WINPTHREAD_DLL}"
                "$<TARGET_FILE_DIR:particles>")
    endif()

    add_custom_target(windows
        DEPENDS particles
        COMMAND "${CMAKE_COMMAND}" -E chdir "$<TARGET_FILE_DIR:particles>"
            zip -j "${CMAKE_BINARY_DIR}/particles.zip"
            "$<TARGET_FILE_NAME:particles>"
            SDL2.dll
            libwinpthread-1.dll
        COMMENT "Creating particles.zip")
    add_custom_target(zip_particles DEPENDS windows)
else()
    find_package(PkgConfig QUIET)

    if(PkgConfig_FOUND)
        pkg_check_modules(SDL2 QUIET sdl2)
        pkg_check_modules(CHIPMUNK QUIET chipmunk)
    endif()

    if(SDL2_FOUND)
        target_include_directories(particles PRIVATE ${SDL2_INCLUDE_DIRS})
        target_link_libraries(particles PRIVATE ${SDL2_LIBRARIES})
    else()
        find_path(SDL2_INCLUDE_DIR SDL2/SDL.h
            HINTS "/usr/include/SDL2")
        find_library(SDL2_LIBRARY NAMES SDL2)
        if(NOT SDL2_INCLUDE_DIR OR NOT SDL2_LIBRARY)
            message(FATAL_ERROR "SDL2 development files were not found")
        endif()
        target_include_directories(particles PRIVATE "${SDL2_INCLUDE_DIR}")
        target_link_libraries(particles PRIVATE "${SDL2_LIBRARY}")
    endif()

    if(CHIPMUNK_FOUND)
        target_include_directories(particles PRIVATE ${CHIPMUNK_INCLUDE_DIRS})
        target_link_libraries(particles PRIVATE ${CHIPMUNK_LIBRARIES})
    else()
        find_path(CHIPMUNK_INCLUDE_DIR chipmunk/chipmunk.h
            HINTS "/usr/local/include")
        find_library(CHIPMUNK_LIBRARY NAMES chipmunk
            HINTS "/usr/local/lib")
        if(NOT CHIPMUNK_INCLUDE_DIR OR NOT CHIPMUNK_LIBRARY)
            message(FATAL_ERROR "Chipmunk2D development files were not found")
        endif()
        target_include_directories(particles PRIVATE "${CHIPMUNK_INCLUDE_DIR}")
        target_link_libraries(particles PRIVATE "${CHIPMUNK_LIBRARY}")
    endif()

    target_compile_definitions(particles PRIVATE _REENTRANT)
    target_link_libraries(particles PRIVATE m)

    add_custom_target(run
        COMMAND "$<TARGET_FILE:particles>"
        DEPENDS particles
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Running particles")
    add_custom_target(linux DEPENDS particles)
endif()

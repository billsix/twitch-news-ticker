cmake_minimum_required(VERSION 3.0)
project(twitch-news-ticker)

include(CheckIncludeFiles)

#find dependencies

if(APPLE OR WIN32)
else()
    set(OpenGL_GL_PREFERENCE "GLVND")
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLFW REQUIRED glfw3)
    find_package(OpenGL REQUIRED)
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/contrib/freetype2)
include_directories(${CMAKE_SOURCE_DIR}/contrib/freetype2/include)



if(WIN32)
    add_subdirectory(${CMAKE_SOURCE_DIR}/contrib/glfw)
    include_directories(${CMAKE_SOURCE_DIR}/contrib/glfw/include)
endif(WIN32)

add_definitions(-DPACKAGE_NAME="twitch-news-ticker")
add_definitions(-DPACKAGE_TARNAME="twitch-news-ticker")
add_definitions(-DPACKAGE_VERSION="0.0.1")
add_definitions(-DPACKAGE_STRING="twitch-news-ticker 0.0.1")
add_definitions(-DPACKAGE_BUGREPORT="William Emerison Six <billsix@gmail.com>")
add_definitions(-DPACKAGE_URL="")


#not quite sure why ${CMAKE_INSTALL_PREFIX} resolves correctly, but ${DESTINATION} doesn't
add_definitions(-DTEXTURE_DIR="${CMAKE_INSTALL_PREFIX}/share/twitch-news-ticker/textures/")
add_definitions(-DFONT_DIR="${CMAKE_INSTALL_PREFIX}/share/twitch-news-ticker/font/")
add_definitions(-DSHADER_DIR="${CMAKE_INSTALL_PREFIX}/share/twitch-news-ticker/shaders/")

add_definitions(-DCMAKE_CONFIG)

#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory(src)

install(DIRECTORY font/ DESTINATION share/twitch-news-ticker/font)
install(DIRECTORY shaders/ DESTINATION share/twitch-news-ticker/shaders)
install(DIRECTORY textures/ DESTINATION share/twitch-news-ticker/textures)

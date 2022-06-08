project(twitch-news-ticker)

set(twitch-news-ticker_headers
    ./common.h
    ./main.h
    ./shader.h
    ./news_window.h
)

set(twitch-news-ticker_source
    ./main.c
    ./shader.c
    ./news_window.c
)

# TODO - make the fetching and building of these static libraries
# possible on linux through an option
if(APPLE OR WIN32)
     set(BUILD_SHARED_LIBS OFF)
     if(WIN32)
         # no console on start
         #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
     endif(WIN32)
endif()




add_executable(twitch-news-ticker ${twitch-news-ticker_headers} ${twitch-news-ticker_source})


if(WIN32)
    target_link_libraries(twitch-news-ticker glfw opengl32 freetype)
else()
    target_link_libraries ( twitch-news-ticker
        PRIVATE
        "-lm"
        "-ldl"
        ${OPENGL_gl_LIBRARY}
        ${GLFW_LIBRARIES}
        freetype
)
endif(WIN32)

target_include_directories ( twitch-news-ticker
    PRIVATE
    ${CMAKE_SOURCE_DIR}/contrib/gl3w-Single-File
    ${CMAKE_SOURCE_DIR}/contrib/stb
    ${CMAKE_SOURCE_DIR}/contrib/nuklear
    ${CMAKE_SOURCE_DIR}/contrib/gl-matrix-stack/
)



install(TARGETS twitch-news-ticker DESTINATION bin)

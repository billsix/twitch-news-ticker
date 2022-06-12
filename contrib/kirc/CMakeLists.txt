cmake_minimum_required(VERSION 3.0)

project(craft LANGUAGES C)

set(CMAKE_C_STANDARD 11)


# set SOURCE_FILES to all of the c files
set(SOURCE_FILES
  src/kirc.c
)


# definition of the craft, and its dependent sources
add_executable(
    kirc
    ${SOURCE_FILES}
)

target_link_libraries(kirc m)

#add_library(world SHARED deps/noise/noise.c src/world.c)

# Install
install(TARGETS kirc DESTINATION bin)

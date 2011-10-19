cmake_minimum_required(VERSION 2.8)

project(eaa)

find_package(PkgConfig)

pkg_check_modules(GLIB REQUIRED glib-2.0)

include_directories(${GLIB_INCLUDE_DIRS})

add_definitions(-D_GNU_SOURCE)

add_executable(eaa main.c cdrom.c)
target_link_libraries(eaa ${GLIB_LIBRARIES})
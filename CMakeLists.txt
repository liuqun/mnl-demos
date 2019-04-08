cmake_minimum_required(VERSION 3.1)
project(demo LANGUAGES C)

file(WRITE "${CMAKE_BINARY_DIR}/include/platform.h.in" "/*(Auto generated from CMakeList.txt)*/
")

include(CheckIncludeFiles)
configure_file(${CMAKE_BINARY_DIR}/include/platform.h.in ${CMAKE_BINARY_DIR}/include/platform.h)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${PROJECT_BINARY_DIR}/include)
aux_source_directory(./src demo_SOURCES)
add_executable(demo ${demo_SOURCES})
target_compile_definitions(demo PRIVATE PLATFORM_CHECK_OK)

find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBMNL REQUIRED libmnl)
find_library(LIBMNL_LIBRARIES
  NAMES mnl
  HINTS ${LIBMNL_LIBDIR} ${LIBMNL_LIBRARY_DIRS}
)

target_include_directories(demo PRIVATE ${LIBMNL_INCLUSE_DIRS})
target_link_libraries(demo ${LIBMNL_LIBRARIES})

cmake_minimum_required(VERSION 3.1)
project(demo LANGUAGES C)
set(demo_SOURCES "check_genl_family_id.c")
add_executable(check_genl_family_id ${demo_SOURCES})

find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBMNL REQUIRED libmnl)
find_library(LIBMNL_LIBRARIES
  NAMES mnl
  HINTS ${LIBMNL_LIBDIR} ${LIBMNL_LIBRARY_DIRS}
)
target_include_directories(check_genl_family_id PRIVATE ${LIBMNL_INCLUSE_DIRS})
target_link_libraries(check_genl_family_id ${LIBMNL_LIBRARIES})

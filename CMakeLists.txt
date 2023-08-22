cmake_minimum_required(VERSION 3.27.0)
project(system_amd VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

file(GLOB SOURCES "src/*.c")
add_executable(system_amd ${SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

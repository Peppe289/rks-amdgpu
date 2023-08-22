cmake_minimum_required(VERSION 3.27.0)
project(rks_amdgpu VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

file(GLOB SOURCES "src/*.c")
add_executable(rks_amdgpu ${SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

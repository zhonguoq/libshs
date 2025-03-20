project(shs)
cmake_minimum_required(VERSION 3.13)

option(SHS_INSTALL_SRC "install src codes" OFF)

# set build type to DEBUG
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_subdirectory(src)

# Assuming the external libraries are built using CMake If not, provide the
# necessary commands to import the external libraries

if(SHS_INSTALL_SRC)
  install(DIRECTORY include/ DESTINATION include)
endif()

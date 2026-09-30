cmake_minimum_required(VERSION 3.23)

# SET GLOBAL VARIABLES

set(PROJECT_NAME kilbyEngine)
set(PROJECT_VERSION 0.1)

  # executables and runtime libraries are in the same folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR/$<CONFIGURATION>")

set(EXTERN_DIR    "${PROJECT_SOURCE_DIR}/extern")
set(INCLUDE_DIRS  "${PROJECT_SOURCE_DIR}/include/
                   ${VULKAN_SDK}/include/")


# PROJECT_NAME
project(
  ${PROJECT_NAME}
  ${PROJECT_VERSION}
  DESCRIPTION "Graphics engine"
  LANGUAGES CXX
)

# CPP STANDARD
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# Setting the R-PATH (runtime Path)
# to be the same for executables and runtime libraries
if(APPLE)
  set_target_prperties(${PROJECT_NAME} PROPERTIES
                       INSTALL_RPATH "@executable_path;@loader_path"
                       BUILD_WITH_INSTALL_RPATH TRUE
                       MACOSX_RPATH TRUE)
endif()

if (UNIX AND NOT APPLE)
  set_target_prperties(${PROJECT_NAME} PROPERTIES
                       INSTALL_RPATH "$ORIGIN"
                       BUILD_WITH_INSTALL_RPATH TRUE)
endif()
#


add_executable(${PROJECT_NAME}
  src/main.cpp
)


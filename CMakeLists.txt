cmake_minimum_required(VERSION 3.23)

# SET GLOBAL VARIABLES

set(PROJECT_NAME kilbyEngine)
set(PROJECT_VERSION 0.1)

  # executables and runtime libraries are in the same folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(SLANGC_EXECUTABLE        "${VULKAN_SDK}/bin/slangc")
set(SHADERS_SOURCE_DIR       "${CMAKE_SOURCE_DIR}/src/shaders")
set(SHADERS_BIN_DIR          "${CMAKE_BINARY_DIR}/shaders")
set(EXTERN_DIR               "${CMAKE_SOURCE_DIR}/extern")


file(GLOB_RECURSE INCLUDE_HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/*.hpp" "${CMAKE_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE INCLUDE_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/*.cpp" "${CMAKE_SOURCE_DIR}/include/*.c")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # generates the compile_commands.json for clangd's lsp


# SET ENVIROMENT CONSTANTS
if (DEBUG)

  ADD_DEFINITIONS(-D__DEBUG__)
  ADD_DEFINITIONS(-DROOT_DIR="${CMAKE_SOURCE_DIR}")
  ADD_DEFINITIONS(-DSHADERS_SOURCE_DIR="${SHADERS_SOURCE_DIR}")
  ADD_DEFINITIONS(-DSHADERS_BIN_DIR="${SHADERS_BIN_DIR}")
 

  set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
endif()


# PROJECT_NAME
project(
  ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  DESCRIPTION "Graphics engine"
  LANGUAGES CXX
)

# CPP STANDARD
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# CREATE EXECUTABLE
add_executable(${PROJECT_NAME}
  src/main.cpp
  ${INCLUDE_HEADERS}
  ${INCLUDE_SOURCES}
)


# Setting the R-PATH (runtime Path)
# to be the same for executables and runtime libraries
if(APPLE)
  set_target_properties(${PROJECT_NAME} PROPERTIES
                       INSTALL_RPATH "@executable_path;@loader_path"
                       BUILD_WITH_INSTALL_RPATH TRUE
                       MACOSX_RPATH TRUE)
endif()

if (UNIX AND NOT APPLE)
  set_target_properties(${PROJECT_NAME} PROPERTIES
                       INSTALL_RPATH "$ORIGIN"
                       BUILD_WITH_INSTALL_RPATH TRUE
                     )
endif()
#


# LINK LIBRARIES

# project's include directory

target_include_directories(${PROJECT_NAME} PRIVATE
  "${CMAKE_SOURCE_DIR}/include"
)

# vulkan and glfw
find_package(Vulkan REQUIRED)
add_subdirectory(${EXTERN_DIR}/glfw)

target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} glfw)


# COMPILE SHADERS TODO

function (add_slang_shader_target TARGET)
  cmake_parse_arguments ("SHADER" "" "" "SOURCES" ${ARGN})

  set (ENTRY_POINTS -entry vertMain -entry fragMain)
  add_custom_command (
          OUTPUT ${SHADERS_BIN_DIR}
          COMMAND ${CMAKE_COMMAND} -E make_directory(${SHADERS_BIN_DIR})
  )
  add_custom_command (
    OUTPUT  ${SHADERS_BIN_DIR}/slang.spv
          COMMAND ${SLANGC_EXECUTABLE} ${SHADER_SOURCES} -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name ${ENTRY_POINTS} -o slang.spv
          WORKING_DIRECTORY ${SHADERS_BIN_DIR}
          DEPENDS ${SHADERS_BIN_DIR} ${SHADER_SOURCES}
          COMMENT "Compiling Slang Shaders"
          VERBATIM
  )
  add_custom_target (${TARGET} DEPENDS ${SHADERS_BIN_DIR}/slang.spv)
endfunction()

add_slang_shader_target( shaders SOURCES ${SHADERS_SOURCE_DIR})
add_dependencies(${PROJECT_NAME} shaders)

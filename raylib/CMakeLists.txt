<<<<<<< HEAD
# Setup the project and settings
project(raylib C)
set(PROJECT_VERSION 5.5.0)
set(API_VERSION 550)
=======
cmake_minimum_required(VERSION 3.25)
#this change avoid the warning that appear when we include raylib using Cmake fatch content 
project(raylib)
>>>>>>> 3ba186f2c1d6f307740d313653772f0a312f5ec3

include(GNUInstallDirs)
include(JoinPaths)

# Sets build type if not set by now
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    if(RAYLIB_IS_MAIN)
        set(default_build_type Debug)
    else()
        message(WARNING "Default build type is not set (CMAKE_BUILD_TYPE)")
    endif()

    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")

<<<<<<< HEAD
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Used as public API to be included into other projects
set(raylib_public_headers
    raylib.h
    rlgl.h
    raymath.h
    )

# Sources to be compiled
set(raylib_sources
    raudio.c
    rcore.c
    rmodels.c
    rshapes.c
    rtext.c
    rtextures.c
    utils.c
    )

# <root>/cmake/GlfwImport.cmake handles the details around the inclusion of glfw
if (NOT ${PLATFORM} MATCHES "Web")
    include(GlfwImport)
endif ()

# Sets additional platform options and link libraries for each platform
# also selects the proper graphics API and version for that platform
# Produces a variable LIBS_PRIVATE that will be used later
include(LibraryConfigurations)

if (SUPPORT_MODULE_RAUDIO)
    MESSAGE(STATUS "Audio Backend: miniaudio")
else ()
    MESSAGE(STATUS "Audio Backend: None (-DCUSTOMIZE_BUILD=ON -DSUPPORT_MODULE_RAUDIO=OFF)")
endif ()

add_library(raylib ${raylib_sources} ${raylib_public_headers})

if (NOT BUILD_SHARED_LIBS)
    MESSAGE(STATUS "Building raylib static library")
    add_library(raylib_static ALIAS raylib)
else()
    MESSAGE(STATUS "Building raylib shared library")
    target_compile_definitions(raylib
                               PRIVATE $<BUILD_INTERFACE:BUILD_LIBTYPE_SHARED>
                               INTERFACE $<INSTALL_INTERFACE:USE_LIBTYPE_SHARED>
                               )
endif()

if (${PLATFORM} MATCHES "Web")
    target_link_options(raylib PUBLIC "-sUSE_GLFW=3")
    if(${GRAPHICS} MATCHES "GRAPHICS_API_OPENGL_ES3")
        target_link_options(raylib PUBLIC "-sFULL_ES3")
    endif()
=======
# Sets compiler flags and language standard
include(CompilerFlags)

# Registers build options that are exposed to cmake
include(CMakeOptions.txt)

if (UNIX AND NOT APPLE AND NOT "${PLATFORM}" MATCHES "DRM" AND NOT "${PLATFORM}" MATCHES "Web")
  if (NOT GLFW_BUILD_WAYLAND AND NOT GLFW_BUILD_X11)
    message(FATAL_ERROR "Cannot disable both Wayland and X11")
  endif()
>>>>>>> 3ba186f2c1d6f307740d313653772f0a312f5ec3
endif()

set_target_properties(raylib PROPERTIES
                      PUBLIC_HEADER "${raylib_public_headers}"
                      VERSION ${PROJECT_VERSION}
                      SOVERSION ${API_VERSION}
                      )

<<<<<<< HEAD
if (WITH_PIC OR BUILD_SHARED_LIBS)
    set_property(TARGET raylib PROPERTY POSITION_INDEPENDENT_CODE ON)
endif ()
=======
# Uninstall target, only create when building raylib by itself
# Avoid conflicting target names when using raylib with other libraries
if(NOT TARGET uninstall AND PROJECT_IS_TOP_LEVEL)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Uninstall.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
>>>>>>> 3ba186f2c1d6f307740d313653772f0a312f5ec3

if (BUILD_SHARED_LIBS)
    # Hide raylib's symbols by default so RLAPI can expose them
    set_property(TARGET raylib PROPERTY C_VISIBILITY_PRESET hidden)
endif ()

<<<<<<< HEAD
target_link_libraries(raylib "${LIBS_PRIVATE}")

# Sets some compile time definitions for the pre-processor
# If CUSTOMIZE_BUILD option is on you will not use config.h by default
# and you will be able to select more build options
include(CompileDefinitions)

# Registering include directories
target_include_directories(raylib
                           PUBLIC
                           $<INSTALL_INTERFACE:include>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                           PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ${OPENGL_INCLUDE_DIR}
                           ${OPENAL_INCLUDE_DIR}
                           )

# Copy the header files to the build directory for convenience
file(COPY ${raylib_public_headers} DESTINATION "include")

# Includes information on how the library will be installed on the system
# when cmake --install is run
include(InstallConfigurations)

# Print the flags for the user
if (DEFINED CMAKE_BUILD_TYPE)
    message(STATUS "Generated build type: ${CMAKE_BUILD_TYPE}")
else ()
    message(STATUS "Generated config types: ${CMAKE_CONFIGURATION_TYPES}")
endif ()

message(STATUS "Compiling with the flags:")
message(STATUS "  PLATFORM=" ${PLATFORM_CPP})
message(STATUS "  GRAPHICS=" ${GRAPHICS})

# Options if you want to create an installer using CPack
include(PackConfigurations)
=======
if (${BUILD_EXAMPLES})
  message(STATUS "Building examples is enabled")
  add_subdirectory(examples)
endif()
>>>>>>> 3ba186f2c1d6f307740d313653772f0a312f5ec3

enable_testing()

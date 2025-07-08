cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

option(VERBOSE "Show explanatory messages" ON)

function(print_message msg)
    if (VERBOSE)
        message("${msg}")
    endif()
endfunction()

print_message("${CMAKE_GENERATOR} generator has been selected")
# determine whether we are generating a Visual Studio project
if (${CMAKE_GENERATOR} MATCHES "Visual Studio")
    print_message("Visual Studio generator detected")
    set (VISUAL_STUDIO TRUE)
    set (NOT_VISUAL_STUDIO FALSE)
else()
    set (VISUAL_STUDIO FALSE)
    set (NOT_VISUAL_STUDIO TRUE)
endif()

print_message("-------------------- COMPUTER SCIENCE --------------------")

# specifies the project name (in MS Visual Studio defines the solution name)
project(computer-science
        VERSION 1.0
        DESCRIPTION "Computer Science (c) 2023-2025 Dmitry Savchenkov"
        HOMEPAGE_URL "https://github.com/dvsav/computer-science"
        LANGUAGES CXX)

# # Sets the required C++ standard (C++17 in our case) across all of the targets in this CMakeLists file
# Sets the desired C++ standard (e.g., 17 for C++17)
# (https://cmake.org/cmake/help/latest/variable/CMAKE_CXX_STANDARD.html#cmake-cxx-standard).
set(CMAKE_CXX_STANDARD 17)
 # Ensures the compiler must support the specified standard, not an older one
# (https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD_REQUIRED.html).
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Optional: disables compiler-specific extensions like GNU++17
# (https://cmake.org/cmake/help/latest/variable/CMAKE_CXX_EXTENSIONS.html#cmake-cxx-extensions)
set(CMAKE_CXX_EXTENSIONS OFF)

# Alternatively we could have set the required C++ standard for a particular build target
# target_compile_features(${PROJECT_NAME} PUBLIC c_std_17)

# Store the paths to some interesting directories in variables
# (the repository's root is two folders higher than this CMakeLists file)
set(ROOT_DIR "${PROJECT_SOURCE_DIR}/../..")
set(SRC_DIR "${ROOT_DIR}/src")
set(TEST_DIR "${ROOT_DIR}/test")

# Collect the source and header files.
# Without CONFIGURE_DEPENDS CMake doesn't rerun file globbing procedure during build time (cmake --build),
# So without CONFIGURE_DEPENDS you need to manually regenerate (cmake -B) whenever you add any new source files
# (https://cmake.org/cmake/help/latest/command/file.html).
# Note that we are collecting both source and header files to be able to see them, for example, in Visual Studio project explorer.
file(GLOB SOURCE_FILES CONFIGURE_DEPENDS
     "${SRC_DIR}/*.cpp"
     "${SRC_DIR}/*.h")

foreach (file ${SOURCE_FILES})
    print_message("source file = ${file}")
endforeach()

# Defines a build target - the executable file and its dependencies (the source files collected above)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Include CMake's FetchContent module
include(FetchContent)

# Add a dependency: unit testing framework Catch2
# (https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md)

# FetchContent_Declare declares where and how to fetch the content (e.g., Git URL, tarball, local path), but does not yet download or unpack it.
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1)
# FetchContent_MakeAvailable does the following:
# 1. Download or copy the external content to a local directory (<binary_dir>/_deps/ by default).
# 2. Extract/unpack it if it's an archive.
# 3. Prepare it for inclusion in the build (e.g., by configuring it).
# This step is what CMake refers to as "populating the content" — turning the declaration into actual usable files.
FetchContent_MakeAvailable(Catch2)
# Link to the Catch2 static library
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain)

if (VISUAL_STUDIO)
    # set our project as the startup project in the Visual Studio solution
    set_property(
        DIRECTORY "${PROJECT_SOURCE_DIR}"
        PROPERTY VS_STARTUP_PROJECT "${PROJECT_NAME}")

    # set Visual Studio Debug working directory to the repository's root directory
    set_target_properties(
        ${PROJECT_NAME} PROPERTIES 
        VS_DEBUGGER_WORKING_DIRECTORY "${ROOT_DIR}")
else() # CYGWIN MSYS2 *NIX environment is assumed below
    # Linking to libstdc++fs is required for use of the Filesystem library extensions in <experimental/filesystem>
    target_link_libraries(${PROJECT_NAME} PRIVATE "stdc++fs")

    # extract standard libraries' search paths from the compiler
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -print-search-dirs
        COMMAND grep libraries
        COMMAND cut -d= -f2
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE CXX_STD_LIB_PATHS)

    # replace colons ':' with semicolons ';'
    string(REPLACE ":" ";" CXX_STD_LIB_PATHS "${CXX_STD_LIB_PATHS}")
    print_message("CXX_STD_LIB_PATHS = ${CXX_STD_LIB_PATHS}")

    # embed library search directories into the executable
    get_target_property(RPATH ${PROJECT_NAME} BUILD_RPATH)
    list(APPEND RPATH ${CXX_STD_LIB_PATHS})

    set_target_properties(
        ${PROJECT_NAME} PROPERTIES 
        BUILD_RPATH "${RPATH}")

    set_target_properties(
        ${PROJECT_NAME} PROPERTIES 
        INSTALL_RPATH "${RPATH}")
endif()

print_message("-------------------- CTEST --------------------")

# enable testing functionality
enable_testing()

# define tests
add_test(
    NAME ${PROJECT_NAME}
    COMMAND $<TARGET_FILE:${PROJECT_NAME}>
	WORKING_DIRECTORY ${TEST_DIR})

print_message("-------------------- INSTALLATION --------------------")

# install the executable file to ${CMAKE_INSTALL_PREFIX}
install(TARGETS ${PROJECT_NAME}
        CONFIGURATIONS Release
        PERMISSIONS OWNER_READ
                    GROUP_READ
                    WORLD_READ
                    OWNER_WRITE
                    GROUP_WRITE
                    WORLD_WRITE)

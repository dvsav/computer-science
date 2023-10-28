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
        DESCRIPTION "Computer Science (c) 2023 Dmitry Savchenkov"
        LANGUAGES CXX)

# the repository's root is two folders higher than this CMakeLists file
set(ROOT_DIR "${PROJECT_SOURCE_DIR}/../..")
set(SRC_DIR "${ROOT_DIR}/src")

# collect the source and header files
file(GLOB SOURCE_FILES CONFIGURE_DEPENDS
     "${SRC_DIR}/*.cpp"
     "${SRC_DIR}/*.h")

foreach (file ${SOURCE_FILES})
    print_message("source file = ${file}")
endforeach()

# defines the executable file name (target) and its dependencies
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

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
    COMMAND $<TARGET_FILE:${PROJECT_NAME}>)

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

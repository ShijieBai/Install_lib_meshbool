cmake_minimum_required(VERSION 3.2)

# https://cmake.org/cmake/help/git-stage/module/FetchContent.html
include(FetchContent)

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
# BASIC HEADER ONLY LIBRARY :::::::::::::::::::::::::::::::::::::::::::::
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#interface-libraries
add_library(cinolib INTERFACE)
target_include_directories(cinolib INTERFACE $<BUILD_INTERFACE:${cinolib_DIR}/include>)
target_include_directories(cinolib INTERFACE $<BUILD_INTERFACE:${cinolib_DIR}/external/eigen>)

# https://cliutils.gitlab.io/modern-cmake/chapters/features/cpp11.html
target_compile_features(cinolib INTERFACE cxx_std_11)

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
# OPTIONAL MODULES ::::::::::::::::::::::::::::::::::::::::::::::::::::::
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

# https://cmake.org/cmake/help/latest/policy/CMP0077.html
cmake_policy(SET CMP0077 NEW)

option(CINOLIB_USES_OPENGL_GLFW_IMGUI   "Use OpenGL, GLFW and ImGui" OFF)
option(CINOLIB_USES_TETGEN              "Use Tetgen"                 OFF)
option(CINOLIB_USES_TRIANGLE            "Use Triangle"               OFF)
option(CINOLIB_USES_SHEWCHUK_PREDICATES "Use Shewchuk Predicates"    OFF)
option(CINOLIB_USES_INDIRECT_PREDICATES "Use Indirect Predicates"    OFF)
option(CINOLIB_USES_GRAPH_CUT           "Use Graph Cut"              OFF)
option(CINOLIB_USES_BOOST               "Use Boost"                  OFF)
option(CINOLIB_USES_VTK                 "Use VTK"                    OFF)

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

if(CINOLIB_USES_TETGEN)
    message("CINOLIB OPTIONAL MODULE: Tetgen")
    FetchContent_Declare(tetgen GIT_REPOSITORY "https://github.com/cinolib-dev-team/tetgen.git")
    FetchContent_MakeAvailable(tetgen)
    target_compile_definitions(cinolib INTERFACE CINOLIB_USES_TETGEN)
    target_compile_definitions(cinolib INTERFACE TETLIBRARY)
    target_compile_definitions(cinolib INTERFACE _CRT_SECURE_NO_WARNINGS) # disable MSVC unsafe warnings
    target_include_directories(cinolib INTERFACE ${tetgen_SOURCE_DIR})
    target_link_libraries(cinolib INTERFACE tetgen)
endif()

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

if(CINOLIB_USES_TRIANGLE)
    message("CINOLIB OPTIONAL MODULE: Triangle")
    FetchContent_Declare(triangle GIT_REPOSITORY "https://github.com/cinolib-dev-team/triangle.git")
    FetchContent_MakeAvailable(triangle)
    target_compile_definitions(cinolib INTERFACE CINOLIB_USES_TRIANGLE)
    target_include_directories(cinolib INTERFACE ${triangle_SOURCE_DIR})
    target_link_libraries(cinolib INTERFACE triangle)
endif()

##::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

if(CINOLIB_USES_SHEWCHUK_PREDICATES)
    message("CINOLIB OPTIONAL MODULE: Shewchuk Predicates")
    add_subdirectory(${cinolib_DIR}/external/shewchuk_predicates shewchuk_predicates)
    target_link_libraries(cinolib INTERFACE shewchuk_predicates)
    target_compile_definitions(cinolib INTERFACE CINOLIB_USES_SHEWCHUK_PREDICATES)
endif()

##::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

if(CINOLIB_USES_INDIRECT_PREDICATES)
    message("CINOLIB OPTIONAL MODULE: Indirect Predicates")
    FetchContent_Declare(indirect_predicates GIT_REPOSITORY "https://github.com/cinolib-dev-team/Indirect_Predicates.git")
    FetchContent_MakeAvailable(indirect_predicates)
    target_link_libraries(cinolib INTERFACE indirectPredicates)
    target_compile_definitions(cinolib INTERFACE CINOLIB_USES_INDIRECT_PREDICATES)
endif()

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

if(CINOLIB_USES_GRAPH_CUT)
    message("CINOLIB OPTIONAL MODULE: Graph Cuts")
    FetchContent_Declare(graph_cuts GIT_REPOSITORY "https://github.com/cinolib-dev-team/GraphCuts.git")
    FetchContent_MakeAvailable(graph_cuts)
    target_compile_definitions(cinolib INTERFACE CINOLIB_USES_GRAPH_CUT)
    target_link_libraries(cinolib INTERFACE graph_cuts)
    target_include_directories(cinolib INTERFACE ${graph_cuts_SOURCE_DIR})
endif()

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

if(CINOLIB_USES_BOOST)
    message("CINOLIB OPTIONAL MODULE: Boost")
    find_package(Boost)
    if(Boost_FOUND)        
        target_compile_features(cinolib INTERFACE cxx_std_14) # Boost requires C++14
        target_include_directories(cinolib INTERFACE ${Boost_INCLUDE_DIRS})
        target_compile_definitions(cinolib INTERFACE CINOLIB_USES_BOOST)
    else()
        message("Could not find Boost!")
	set(CINOLIB_USES_BOOST OFF)
    endif()
endif()


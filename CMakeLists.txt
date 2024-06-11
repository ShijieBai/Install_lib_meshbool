cmake_minimum_required(VERSION 3.21)

# Set the target architecture.
# All modern x86/x64 processors support AVX2.
# Older x86/x64 processors may support SSE2 but not AVX2.
# Very old x86/x64 processors, or non x86/x64
# processors, do not support any of the two.
set(ENABLE_SSE2 True)
set(ENABLE_AVX2 True)


# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set the project name
project(mesh_booleans)
# project(mesh_booleans LANGUAGES C CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")


set(TBB_TEST OFF CACHE BOOL " " FORCE)
set(TBB_EXAMPLES OFF CACHE BOOL " " FORCE)
set(tbb_DIR D:/soft/tbb-prefix)
set(eigen_root_dir D:/Eigen/eigen-3.4.0)

#USE EIGEN
set(EIGEN3_DIR ${eigen_root_dir}/install-me/share/eigen3/cmake)
set(Eigen3_DIR ${eigen_root_dir}/install-me/share/eigen3/cmake)
if (NOT IS_DIRECTORY ${eigen_root_dir}/install-me)
    message("The Eigen3 is found, but not INSTALLED. So Now I install it.")
    execute_process(COMMAND cmake -S ${eigen_root_dir} -B ${eigen_root_dir}/build -D CMAKE_INSTALL_PREFIX=${eigen_root_dir}/install-me)
    execute_process(COMMAND cmake --build ${eigen_root_dir}/build)
    execute_process(COMMAND cmake --install ${eigen_root_dir}/build)
else ()
    message("Good. Eigen3 is found and installed.")
endif ()
# 返回一个interface target: Eigen3::Eigen
message(STATUS "EIGEN3_DIR = ${EIGEN3_DIR}")
find_package(Eigen3 REQUIRED NO_MODULE)

#USE TBB
add_library(target_interface_tbb INTERFACE)
#find_package(tbb CONFIG REQUIRED)
target_include_directories(target_interface_tbb INTERFACE ${tbb_DIR}/include)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(lib_debug ${tbb_DIR}/lib-debug/*.lib)
        set(lib_release ${tbb_DIR}/lib-release/*.lib)
        target_link_libraries(target_interface_tbb INTERFACE 
        "$<IF:$<CONFIG:Debug>,${lib_debug},${lib_release}>")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_link_libraries(target_interface_tbb INTERFACE ${tbb_DIR}/lib/*.so)
endif()
#if(NOT tbb_INCLUDE_DIRS)
#        message(FATAL_ERROR "cannot find tbb include files!")
#else()
#        message(STATUS "Found tbb include files: ${tbb_INCLUDE_DIRS}")
#        message(STATUS "Found tbb lib files: ${tbb_LIBRARY_DIRS}")
#        message(STATUS "Found tbb  CMAKE_CURRENT_LIST_FILE: ${CMAKE_CURRENT_LIST_FILE}")
#endif()

# add_subdirectory(arrangements/external/oneTBB)


#set(cinolib_DIR ${PROJECT_SOURCE_DIR}/arrangements/external/Cinolib)

# USE CINOLIB
set(CINOLIB_USES_OPENGL_GLFW_IMGUI OFF)
set(CINOLIB_USES_SHEWCHUK_PREDICATES ON)
add_subdirectory(arrangements/external/Cinolib)
#find_package(cinolib REQUIRED)
#set_target_properties(shewchuk_predicates PROPERTIES LINKER_LANGUAGE CXX)

##########################################################
set(bin_dest "bin")  

set(include_dest "include")  

set(lib_dest "lib")

##########################################################

file(GLOB external_files ${PROJECT_SOURCE_DIR}/arrangements/external/Indirect_Predicates/include/*.h 
                ${PROJECT_SOURCE_DIR}/arrangements/external/Indirect_Predicates/include/*.hpp
                ${PROJECT_SOURCE_DIR}/arrangements/external/Indirect_Predicates/include/*.cpp
                ${PROJECT_SOURCE_DIR}/arrangements/external/parallel-hashmap/parallel_hashmap/*.h)


set(arrange_include_dir ${PROJECT_SOURCE_DIR}/arrangements/external/Indirect_Predicates/include
                ${PROJECT_SOURCE_DIR}/arrangements/external/parallel-hashmap/parallel_hashmap)


file(GLOB arrange_include_files  ${PROJECT_SOURCE_DIR}/arrangements/external/Indirect_Predicates/include/*.h
        ${PROJECT_SOURCE_DIR}/arrangements/external/parallel-hashmap/parallel_hashmap/*.h
        ${PROJECT_SOURCE_DIR}/arrangements/external/Indirect_Predicates/include/*.hpp)


#add_library(target_lib_Indirect_Predicates ${Indirect_Predicates_files} )
#target_include_directories(target_lib_Indirect_Predicates PUBLIC ${include_dir})
#set_target_properties(target_lib_Indirect_Predicates PROPERTIES LINKER_LANGUAGE CXX)
#target_link_libraries(target_lib_Indirect_Predicates PUBLIC tbb cinolib)

add_subdirectory(arrangements/code)
target_include_directories(target_lib_arrangement PUBLIC  ${arrange_include_dir})
target_include_directories(target_lib_arrangement PUBLIC ${PROJECT_SOURCE_DIR}/arrangements/external/abseil-cpp/)
#target_include_directories(target_lib_arrangement PUBLIC  ${tbb_INCLUDE_DIRS})
#set_target_properties(target_lib_arrangement PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(target_lib_arrangement PUBLIC target_interface_tbb target_lib_cinolib)
target_compile_definitions(target_lib_arrangement PUBLIC TBB_PARALLEL=1)

install(TARGETS target_lib_arrangement DESTINATION ${lib_dest})

install(FILES ${arrange_include_files} DESTINATION ${include_dest})

install(DIRECTORY ${PROJECT_SOURCE_DIR}/arrangements/external/abseil-cpp/absl DESTINATION ${include_dest})


add_subdirectory(code)
target_compile_definitions(target_lib_mesh_bool PUBLIC TBB_PARALLEL=1)
#set_target_properties(target_lib_mesh_bool PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(target_lib_mesh_bool PUBLIC target_lib_arrangement)



# add the executable
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE target_lib_mesh_bool)
target_compile_definitions(${PROJECT_NAME} PUBLIC TBB_PARALLEL=1)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # grant IEEE 754 compliance
        target_compile_options(target_lib_mesh_bool PUBLIC "/fp:strict")
        target_compile_options(target_lib_arrangement PUBLIC "/fp:strict")
        # fix the min max collision and byte problem
        target_compile_definitions(target_lib_cinolib INTERFACE NOMINMAX)
        target_compile_definitions(target_lib_cinolib INTERFACE _HAS_STD_BYTE=0)
        # use intrinsic functions
        target_compile_options(target_lib_mesh_bool PUBLIC "/Oi")
        target_compile_options(target_lib_arrangement PUBLIC "/Oi")
        # set target architecture
        if(ENABLE_AVX2)
                target_compile_options(target_lib_mesh_bool PUBLIC "/arch:AVX2")
                target_compile_options(target_lib_arrangement PUBLIC "/arch:AVX2")
        elseif(ENABLE_SSE2)
                target_compile_options(target_lib_mesh_bool PUBLIC "/arch:SSE2")
                target_compile_options(target_lib_arrangement PUBLIC "/arch:SSE2")
        endif()
        # reserve enough stack size
        target_link_options(target_lib_mesh_bool PUBLIC "/STACK:8421376")
        target_link_options(target_lib_arrangement PUBLIC "/STACK:8421376")
        # turn off annoying warnings
        target_compile_options(target_lib_mesh_bool PUBLIC "/D _CRT_SECURE_NO_WARNINGS")
        target_compile_options(target_lib_arrangement PUBLIC "/D _CRT_SECURE_NO_WARNINGS")
else()
        # set standard optimization level
        target_compile_options(target_lib_mesh_bool PUBLIC -O2)
        # reserve enough stack size
        target_compile_options(target_lib_mesh_bool PUBLIC -Wl,-z,stacksize=8421376)
        # grant IEEE 754 compliance
        target_compile_options(target_lib_mesh_bool PUBLIC -frounding-math)
        # set target architecture
        if(ENABLE_AVX2)
                target_compile_options(target_lib_mesh_bool PUBLIC "-mavx2")
        elseif(ENABLE_SSE2)
                target_compile_options(target_lib_mesh_bool PUBLIC "-msse2")
        endif()
endif()

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # grant IEEE 754 compliance
        target_compile_options(${PROJECT_NAME} PUBLIC "/fp:strict")
        # use intrinsic functions
        target_compile_options(${PROJECT_NAME} PUBLIC "/Oi")
        # set target architecture
        if(ENABLE_AVX2)
                target_compile_options(${PROJECT_NAME} PUBLIC "/arch:AVX2")
        elseif(ENABLE_SSE2)
                target_compile_options(${PROJECT_NAME} PUBLIC "/arch:SSE2")
        endif()
        # reserve enough stack size
        target_link_options(${PROJECT_NAME} PUBLIC "/STACK:8421376")
        # turn off annoying warnings
        target_compile_options(${PROJECT_NAME} PUBLIC "/D _CRT_SECURE_NO_WARNINGS")
else()
        # set standard optimization level
        target_compile_options(${PROJECT_NAME} PUBLIC -O2)
        # reserve enough stack size
        target_compile_options(${PROJECT_NAME} PUBLIC -Wl,-z,stacksize=8421376)
        # grant IEEE 754 compliance
        target_compile_options(${PROJECT_NAME} PUBLIC -frounding-math)
        # set target architecture
        if(ENABLE_AVX2)
                target_compile_options(${PROJECT_NAME} PUBLIC "-mavx2")
        elseif(ENABLE_SSE2)
                target_compile_options(${PROJECT_NAME} PUBLIC "-msse2")
        endif()
endif()

file(GLOB src_files *.h *.cpp)

set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(target_lib_mesh_bool ${src_files})

target_include_directories(target_lib_mesh_bool PUBLIC ${include_dir})

install(TARGETS target_lib_mesh_bool DESTINATION ${lib_dest})

file(GLOB include_files ${include_dir}/*.h)

install(FILES ${include_files} DESTINATION ${include_dest})
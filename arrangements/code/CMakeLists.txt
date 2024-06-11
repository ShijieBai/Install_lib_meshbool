file(GLOB src_files *.h *.cpp)

if(NOT external_files)
    message("external_files is empty")
else()
    list(APPEND src_files ${external_files})
endif()

set(include_dir "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(target_lib_arrangement ${src_files})

target_include_directories(target_lib_arrangement PUBLIC ${include_dir})

#install(TARGETS target_lib_arrangement DESTINATION ${lib_dest})
file(GLOB arrange_include_files ${include_dir}/*.h)

if(NOT arrange_include_files)
    message("arrange_include_files is empty")
endif()

install(FILES ${arrange_include_files} DESTINATION ${include_dest})
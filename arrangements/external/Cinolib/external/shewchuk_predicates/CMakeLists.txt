cmake_minimum_required(VERSION 3.14)

add_library(shewchuk_predicates shewchuk.c)

install(TARGETS shewchuk_predicates  DESTINATION ${lib_dest})

#install(FILES ${include_dir}/*.h DESTINATION ${include_dest})

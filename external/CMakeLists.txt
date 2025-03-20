
aux_source_directory(. DIR_SRCS)

add_library(library_name ${DIR_SRCS})
add_subdirectory(flatbuffer)

target_include_directories(library_name PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


# target_link_libraries(shs PUBLIC library_name)

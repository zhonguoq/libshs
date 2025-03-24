# aux_source_directory(${CMAKE_SOURCE_DIR}/src SRC_LIST)

set(SRC_LIST file_table.c q_string.c shs_accessor.c shs_binary.c shs_file.c
             shs_inner_file_table.c shs_pack.c)

# Set the path to the external libraries include directory
set(HASH_DIR ${CMAKE_SOURCE_DIR}/external/hash)
set(FB_DIR ${CMAKE_SOURCE_DIR}/external/flatbuffer)

# add cmetrohash64.c to the source list
set(ALL_SRC ${SRC_LIST} ${HASH_DIR}/cmetrohash64.c)

# add flatccrt to the source list
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/flatccrt)

message(STATUS "CMAKE_SOURCE_DIR: ${ALL_SRC}")

# Create a static library
add_library(shs SHARED ${ALL_SRC})

target_include_directories(shs PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(shs PUBLIC ${HASH_DIR})
target_include_directories(shs PUBLIC ${FB_DIR})
target_include_directories(shs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB HEADERS ${CMAKE_SOURCE_DIR}/include/*.h ${HASH_DIR}/*.h ${FB_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
message(STATUS "HEADERS: ${HEADERS}")
install(FILES ${HEADERS} DESTINATION include/shs)
install(TARGETS shs
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)

# Link the flatccrt library
target_link_libraries(shs PUBLIC lflatccrt)

# Add an executable to generate shs_read.
add_executable(shs_read shs_read_main.c)
target_link_libraries(shs_read shs)

#Add an executable to generate shs_pack.
add_executable(shs_pack shs_pack_main.c)
target_link_libraries(shs_pack shs)

if(SHS_INSTALL_SRC)
  # install all files ends with .c in the current source directory to destinaton
  # .
  file(GLOB C_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
  list(FILTER C_FILES EXCLUDE REGEX "shs_pack.c")
  install(FILES ${C_FILES} DESTINATION .)

  # install flatbuffer generated headers
  file(GLOB FB_HEADERS ${FB_DIR}/*.h)
  install(FILES ${FB_HEADERS} DESTINATION include)

  set(HASH_HEADERS
      ${HASH_DIR}/hash.h
      ${HASH_DIR}/cmetrohash.h
      ${HASH_DIR}/hash_table.h
      ${HASH_DIR}/hash_table_def.h
      ${HASH_DIR}/hash_table_impl.h
      ${HASH_DIR}/hash_table_impl_rh.h
      ${HASH_DIR}/ht_hash_function.h
      ${HASH_DIR}/ht_portable.h
      ${HASH_DIR}/ht_trace.h
      ${HASH_DIR}/unaligned.h
      ${HASH_DIR}/pstdint.h)
  install(FILES ${HASH_HEADERS} DESTINATION include)

  set(CMETOHASH_C ${HASH_DIR}/cmetrohash64.c)
  install(FILES ${CMETOHASH_C} DESTINATION .)

  file(GLOB TARGET_FILES ${CMAKE_SOURCE_DIR}/target/WATCH_RTT_SCONS/*)
  install(FILES ${TARGET_FILES} DESTINATION .)
endif()

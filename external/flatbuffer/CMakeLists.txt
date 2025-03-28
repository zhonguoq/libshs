# add_subdirectory(runtime)

file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
install(FILES ${HEADERS} DESTINATION include)

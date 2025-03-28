include_directories (
    "${CMAKE_SOURCE_DIR}/include"
)

add_library(lflatccrt
    builder.c
    emitter.c
    refmap.c
    verifier.c
    json_parser.c
    json_printer.c
)

target_compile_options(lflatccrt PRIVATE -fPIC)

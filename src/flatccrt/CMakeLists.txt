include_directories (
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_library(lflatccrt
    builder.c
    emitter.c
    refmap.c
    verifier.c
    json_parser.c
    json_printer.c
)
cmake_minimum_required(VERSION 3.28.0)
project(GameBoy VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

set(SOURCES
    lib/cpu.c
    lib/memory.c
    lib/instructions.c
    lib/bus.c
    lib/ram.c
    lib/io.c
    lib/opcode.c
    lib/dbg.c
    main.c
)

add_executable(GameBoy ${SOURCES})

target_include_directories(GameBoy
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

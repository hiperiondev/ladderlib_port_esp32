file(
    GLOB_RECURSE
        SOURCES
            ladderlib/source/*.c
            ladderlib/port/esp32/*.c
)

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS
        ladderlib/source/include/
        ladderlib/port/esp32/
        ladderlib/
)
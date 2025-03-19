file(
    GLOB_RECURSE
        SOURCES
            ./*.c
)

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS
        ./
    REQUIRES
        vfs
        ladderlib
        console
        driver
        ftpserver
        nvs_flash
        hal_esp32
        cmd_system
        cmd_ladderlib
        esp32-wifi-provision-care
)
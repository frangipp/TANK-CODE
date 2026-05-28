add_library(stusb4531 STATIC
    Src/platform.c
    Src/stusb4531.c
    Src/example_usage.c
)

target_include_directories(stusb4531 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

target_link_libraries(stusb4531 PUBLIC stm32cubemx)
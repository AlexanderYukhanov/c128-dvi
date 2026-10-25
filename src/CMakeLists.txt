
add_executable(c128_dvi
    main.c
    capture/capture.c
    stream/stream.c
    stream/xga_font_data.c
)

target_include_directories(c128_dvi PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/capture
    ${CMAKE_CURRENT_LIST_DIR}/pio
    ${CMAKE_CURRENT_LIST_DIR}/stream
    ${CMAKE_CURRENT_LIST_DIR}/palette
)

target_link_libraries(c128_dvi
    pico_stdlib
    hardware_pio
    pico_multicore
    hardware_dma
    hardware_clocks
    pico_sync
    hardware_structs
)

pico_generate_pio_header(c128_dvi ${CMAKE_CURRENT_LIST_DIR}/pio/capture.pio)

pico_enable_stdio_usb(c128_dvi 1)
pico_enable_stdio_uart(c128_dvi 0)
pico_add_extra_outputs(c128_dvi)

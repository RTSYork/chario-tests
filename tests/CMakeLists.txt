add_executable(libchario-test libchario-test.c)
target_link_libraries(libchario-test chario)

add_executable(bs-timing bs-timing.c)
target_link_libraries(bs-timing proftimer output)

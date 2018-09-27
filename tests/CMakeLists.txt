add_executable(libchario-test libchario-test.c)
target_link_libraries(libchario-test chario)

#add_executable(libchario-test-2 libchario-test-2.c)
#target_link_libraries(libchario-test-2 chario)

add_executable(bs-timing bs-timing.c)
target_link_libraries(bs-timing proftimer output)

add_executable(bs-timing-phys bs-timing-phys.c)
target_link_libraries(bs-timing-phys proftimer output)

add_executable(simplefs-test-1 simplefs-test-1.c)
target_link_libraries(simplefs-test-1 simplefs)

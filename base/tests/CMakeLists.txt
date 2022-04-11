add_executable(asynclogging_test AsyncLogging_test.cc)
target_link_libraries(asynclogging_test muduo_base)

add_executable(Atomic_test Atomic_test.cc)
add_test(NAME Atomic_test COMMAND Atomic_test)

add_executable(fileutil_test FileUtil_test.cc)
target_link_libraries(fileutil_test muduo_base)
add_test(NAME fileutil_test COMMAND fileutil_test)
find_package(Threads REQUIRED)

# Ubuntu repos only include gtest source, not binaries.
# TODO: Make this also work for distros that include binaries.
set(GTEST_SRC "/usr/src/gtest")
include_directories(${GTEST_SRC})
add_library(gtest
    ${GTEST_SRC}/src/gtest-all.cc
)
target_link_libraries(gtest
    Threads::Threads
)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(test_zlgb
    CpuTest.cpp
    DisplayTest.cpp
    InputTest.cpp
    main.cpp
    MbcTest.cpp
    MemoryTest.cpp
)

target_link_libraries(test_zlgb
    ${SDL2_LIBRARIES}
    gtest
    zlgb_core
)

add_test(test_zlgb test_zlgb)
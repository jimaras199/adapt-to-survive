cmake_minimum_required(VERSION 3.27)

project(AdaptToSurvive
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(EnTT CONFIG REQUIRED)

add_executable(ats_game
    src/main.cpp
)

target_include_directories(ats_game PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(ats_game PRIVATE
    EnTT::EnTT
)

include(CTest)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(ats_tests
    tests/Vector2Tests.cpp
    tests/Transform2DTests.cpp
    tests/MovementSystemTests.cpp
    tests/FixedStepRunnerTests.cpp
    tests/WorldTests.cpp
    tests/SimulationTests.cpp
    tests/SystemSchedulerTests.cpp
    tests/EcsMovementSystemTests.cpp
    tests/HeadlessRunnerTests.cpp
    tests/SurvivalStateTests.cpp
    tests/SurvivalDepletionSystemTests.cpp)

    target_include_directories(ats_tests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

    target_link_libraries(ats_tests PRIVATE
        GTest::gtest_main
        EnTT::EnTT
    )

    include(GoogleTest)
    gtest_discover_tests(ats_tests)
endif()
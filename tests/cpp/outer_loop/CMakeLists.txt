add_executable(outer_loop_test 
        outer_loop_test.cpp)

target_link_libraries(outer_loop_test
        PRIVATE
        solvers
        outer_loop_lib
        benders_by_batch_core
        benders_core
        output_core
        logger_lib
        GTest::Main
        tests_utils)

add_test(NAME unit_outer_loop COMMAND outer_loop_test WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
set_property(TEST unit_outer_loop PROPERTY LABELS unit)
set(TASKMANAGER_TEST_FIXTURES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/proc")

function(add_datasource_test name)
    qt_add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE taskmanager_datasource Qt6::Test)
    target_compile_definitions(${name} PRIVATE
        TASKMANAGER_TEST_FIXTURES_DIR="${TASKMANAGER_TEST_FIXTURES_DIR}"
    )
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_datasource_test(test_Formatting)
add_datasource_test(test_CpuStats)
add_datasource_test(test_MemoryStats)
add_datasource_test(test_NetworkStats)
add_datasource_test(test_ProcessReader)

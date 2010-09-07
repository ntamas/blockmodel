set(TEST_CASES blockmodel
               greedy_strategy
               moving_average
)

foreach(test ${TEST_CASES})
    add_executable(${test}_test ${test}_test)
    target_link_libraries(${test}_test igraph igraphpp block mtwister)
    add_test(${test}_test ${test}_test)
endforeach(test)

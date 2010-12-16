set(TEST_CASES undir_blockmodel
               dc_undir_blockmodel
               greedy_strategy
               moving_average
               statistics
               vector_matrix
               util
)

foreach(test ${TEST_CASES})
    add_executable(${test}_test ${test}_test)
    target_link_libraries(${test}_test block igraphpp mtwister ${igraph_LIBRARIES})
    add_test(${test}_test ${test}_test)
endforeach(test)

add_executable(block-fit main
                         ../common/cmd_arguments_base
                         ../common/graph_util
                         ../common/string_util
                         cmd_arguments)
target_link_libraries(block-fit block igraphpp mtwister ${igraph_LIBRARIES})

install(TARGETS block-fit
	    RUNTIME DESTINATION bin)

add_executable(blockmodel main cmd_arguments)
target_link_libraries(blockmodel igraph igraphpp block)

install(TARGETS blockmodel
	    RUNTIME DESTINATION bin)

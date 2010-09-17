add_executable(block-fit main cmd_arguments)
target_link_libraries(block-fit igraph igraphpp block mtwister)

install(TARGETS block-fit
	    RUNTIME DESTINATION bin)

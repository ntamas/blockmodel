add_executable(block-gen main
                         ../common/cmd_arguments_base
                         cmd_arguments)
target_link_libraries(block-gen igraph igraphpp block mtwister)

install(TARGETS block-gen
	    RUNTIME DESTINATION bin)

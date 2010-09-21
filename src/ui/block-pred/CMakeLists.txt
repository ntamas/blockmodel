add_executable(block-pred main
                          ../common/cmd_arguments_base
                          ../common/string_util
                          cmd_arguments)
target_link_libraries(block-pred block igraphpp igraph mtwister)

install(TARGETS block-pred
	    RUNTIME DESTINATION bin)

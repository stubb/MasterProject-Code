Mac build notes:
Change lib path after compiling with (done for bundle within Makefile):
install_name_tool -change @rpath/SDL2.framework/Versions/A/SDL2 @executable_path/bin/mac64/libs/SDL2.framework/Versions/A/SDL2 RenderingClient.out
install_name_tool -change @rpath/SDL2_net.framework/Versions/A/SDL2_net @executable_path/bin/mac64/libs/SDL2_net.framework/Versions/A/SDL2_net RenderingClient.out


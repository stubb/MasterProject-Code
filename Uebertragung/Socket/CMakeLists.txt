cmake_minimum_required(VERSION 2.8)
project( socket_server )
find_package( websocketpp REQUIRED )
find_package(Boost COMPONENTS system REQUIRED)
add_executable( socket_server.app socket.cpp)
target_link_libraries( socket_server.app
	${websocketpp_LIBS}
	${Boost_SYSTEM_LIBRARY}
)
if (Linux)
	target_link_libraries( socket_server.app )
endif()

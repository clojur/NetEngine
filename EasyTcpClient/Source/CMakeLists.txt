
MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

if(WIN32)
	include_directories("C:\\Program Files (x86)\\Windows Kits\\8.1\\Include\\um")

#ELSEIF (UNIX)

#ELSE
endif()

#add_library(easyTcpClient easyTcpClient.cpp)

add_executable(client client.cpp easyTcpClient.cpp)

if(WIN32)
	target_link_libraries(client "C:\\Program Files (x86)\\Windows Kits\\8.1\\Lib\\winv6.3\\um\\x86\\ws2_32.lib")
endif()
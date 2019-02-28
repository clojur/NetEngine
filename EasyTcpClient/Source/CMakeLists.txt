add_library(easyTcpClient 
    easyTcpClient.cpp
)

add_executable(client client.cpp)

target_link_libraries(client easyTcpClient)
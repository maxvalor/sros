cmake_minimum_required(VERSION 3.0)
project(light)

add_definitions(-std=c++17)

include_directories(
  include
  include/light
)

add_library(pssc SHARED
  src/pssc/Node.cpp
  src/pssc/Core.cpp
  src/tcp/TCPConnection.cpp
  src/tcp/TCPServer.cpp
  src/tcp/TCPClient.cpp
)

target_link_libraries(pssc
  -lpthread
  glog
  -lboost_system
)

add_executable(pssc_core
  src/pssc/test_core.cpp
  src/pssc/Core.cpp
  src/tcp/TCPConnection.cpp
  src/tcp/TCPServer.cpp
)
target_link_libraries(pssc_core
  -lpthread
  glog
  -lboost_system
)

add_executable(test_client
  src/pssc/test_client.cpp
  src/pssc/Node.cpp
  src/tcp/TCPConnection.cpp
  src/tcp/TCPClient.cpp
)
target_link_libraries(test_client
  -lpthread
  glog
  -lboost_system
)


add_executable(test_subscriber
  src/test_subscribe.cpp
  src/pssc/Node.cpp
  src/tcp/TCPConnection.cpp
  src/tcp/TCPClient.cpp
)
target_link_libraries(test_subscriber
  -lpthread
  glog
  -lboost_system
)

add_executable(test_publish
  src/test_publish.cpp
  src/pssc/Node.cpp
  src/tcp/TCPConnection.cpp
  src/tcp/TCPClient.cpp
)
target_link_libraries(test_publish
  -lpthread
  glog
  -lboost_system
)

add_executable(test_service
  src/test_service.cpp
  src/pssc/Node.cpp
  src/tcp/TCPConnection.cpp
  src/tcp/TCPClient.cpp
)
target_link_libraries(test_service
  -lpthread
  glog
  -lboost_system
)

add_executable(test_call
  src/test_call.cpp
  src/pssc/Node.cpp
  src/tcp/TCPConnection.cpp
  src/tcp/TCPClient.cpp
)
target_link_libraries(test_call
  -lpthread
  glog
  -lboost_system
)

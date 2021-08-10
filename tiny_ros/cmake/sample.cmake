# sample.cmake

include_directories(
  include
  sample/pub_sub
)

add_executable(sample_ps sample/pub_sub/main.cpp)
add_executable(sample_ps2 sample/pub_sub/main2.cpp)
add_executable(sample_sc sample/srv_call/main.cpp)
add_executable(sample_sc2 sample/srv_call/main2.cpp)

target_link_libraries(sample_ps
  pthread
  boost_system
  glog
  /home/ubuntu/maxvalor/doggy/pssc/build/libpssc.so
)

target_link_libraries(sample_ps2
  pthread
  boost_system
  glog
  /home/ubuntu/maxvalor/doggy/pssc/build/libpssc.so
)

target_link_libraries(sample_sc
  pthread
  boost_system
  glog
  /home/ubuntu/maxvalor/doggy/pssc/build/libpssc.so
)

target_link_libraries(sample_sc2
  pthread
  boost_system
  glog
  /home/ubuntu/maxvalor/doggy/pssc/build/libpssc.so
)


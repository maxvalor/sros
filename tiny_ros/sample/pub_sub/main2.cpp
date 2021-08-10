#include "tiny_ros/tiny_ros.h"
#include "sub_module.h"
#include <cstdio>

int main()
{
  tiny_ros::init();
  SubModule sm;
  sm.join();
  getchar();
  return 0;
}

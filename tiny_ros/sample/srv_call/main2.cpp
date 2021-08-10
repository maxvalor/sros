#include "client_module.h"
#include "server_module.h"
#include <cstdio>

int main()
{
  tiny_ros::init();
  ClientModule cm;
  cm.join();
  getchar();
  return 0;
}

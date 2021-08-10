#include "client_module.h"
#include "server_module.h"
#include <cstdio>


int main()
{
  tiny_ros::init();
  ServerModule sm;
  ClientModule cm;
  sm.join();
  cm.join();
  getchar();
  return 0;
}

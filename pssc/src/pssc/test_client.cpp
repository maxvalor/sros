/*
 * test_core.cpp
 *
 *  Created on: Apr 23, 2021
 *      Author: ubuntu
 */


#include "pssc/protocol/Node.h"

#include <glog/logging.h>

int main()
{
    pssc::Node n;

    n.Initialize(20001);

    getchar();
    return 0;


}


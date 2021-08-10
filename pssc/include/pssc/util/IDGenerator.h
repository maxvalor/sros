/*
 * id_generator.h
 *
 *  Created on: Mar 9, 2021
 *      Author: ubuntu
 */

#ifndef PSSC_ID_GENERATOR_H_
#define PSSC_ID_GENERATOR_H_

#include <mutex>

template <typename T>
class IDGenerator
{
  std::mutex mtx;
  T id;
public:
  T Next()
  {
    std::lock_guard<std::mutex> l(mtx);
    return id++;
  }
};


#endif /* PSSC_ID_GENERATOR_H_ */

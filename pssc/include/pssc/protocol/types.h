/*
 * types.h
 *
 *  Created on: Apr 25, 2021
 *      Author: ubuntu
 */

#ifndef INCLUDE_PSSC_PROTOCOL_TYPES_H_
#define INCLUDE_PSSC_PROTOCOL_TYPES_H_

#include <cstddef>
#include <cstdint>
#include <shared_mutex>

#define BUILD_DEPENDS_ON_PLATFORM

#ifdef BUILD_DEPENDS_ON_PLATFORM

#define SIZE_OF_PSSC_INS sizeof(std::uint8_t)
#define SIZE_OF_PSSC_ID sizeof(std::uint64_t)
#define SIZE_OF_SIZE sizeof(size_t)
#define SIZE_OF_BOOL sizeof(bool)

#else /* not BUILD_DEPENDS_ON_PLATFORM */

#define SIZE_OF_PSSC_INS 1u
#define SIZE_OF_PSSC_ID 8u
#define SIZE_OF_SIZE sizeof(size_t)
#define SIZE_OF_BOOL 1u

#endif /* BUILD_DEPENDS_ON_PLATFORM */

#define PSSC_SIZE_OF_HUGE_MSG 10000u

namespace pssc {

using pssc_size = size_t;
using pssc_ins = std::uint8_t;
using pssc_id = std::uint64_t;
using pssc_byte = std::uint8_t;
using pssc_bytes = std::uint8_t*;

using pssc_lock_guard = std::lock_guard<std::mutex>;
using pssc_rw_mutex = std::shared_timed_mutex;
using pssc_read_guard = std::shared_lock<std::shared_timed_mutex>;
using pssc_write_guard = std::unique_lock<std::shared_timed_mutex>;

}

#endif /* INCLUDE_PSSC_PROTOCOL_TYPES_H_ */

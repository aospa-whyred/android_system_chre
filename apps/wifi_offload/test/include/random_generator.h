/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CHRE_WIFI_OFFLOAD_TEST_RANDOM_GENERATOR_H_
#define CHRE_WIFI_OFFLOAD_TEST_RANDOM_GENERATOR_H_

#include <cinttypes>
#include <random>

namespace wifi_offload_test {

/**
 * @class RandomGenerator A class to generate random values for any uint type:
 *        (uint8_t, ..., uint64_t). Also supports resetting to its initial
 *        state to be able to reproduce the same random sequence.
 */
class RandomGenerator {
 public:
  /**
   * Default constructs a RandomGenerator object
   */
  RandomGenerator();

  /**
   * Resets the object to its initial state. Useful if we want to reproduce the
   * exact same sequenct again.
   */
  void Reset();

  /**
   * Generates a random number of type IntType
   *
   * @return A random number of type IntType
   */
  template <typename IntType>
  IntType get() {
    return static_cast<IntType>(uniform_distribution_(random_engine_));
  }

 private:
  /* The initial seed kept to use again when need to reset  */
  std::random_device::result_type initial_seed_;

  /* Standard mersenne_twister_engine */
  std::mt19937 random_engine_;

  /**
   * Use uniform_distribution_ to transform the random unsigned int generated by
   * random_engine_ into an uint64_t
   */
  std::uniform_int_distribution<uint64_t> uniform_distribution_;
};

}  // namespace wifi_offload_test

#endif  // CHRE_WIFI_OFFLOAD_TEST_RANDOM_GENERATOR_H_

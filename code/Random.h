// Random.h

#ifndef RANDOM_H_
#define RANDOM_H_

#include <cstdint>



// Collection of functions for generating random numbers with a variety of
// different distributions. (This is done partly because on some systems the
// random number generation is not thread-safe.)
class Random {
public:
  // Seed the generator (e.g. to make it produce exactly the same random
  // numbers it produced previously).
  static void Seed(uint64_t seed);

  static uint32_t Int();
  static uint32_t Int(uint32_t modulus);

  static double Real();

  // Slower functions for getting random numbers from a given distribution.
  // Do not use these functions in time-critical code.

  // Return the expected number of failures before k successes, when the 
  // probability of success is p. The mean value will be k / (1 - p).
  static uint32_t Polya(uint32_t k, double p = .5);
  // Get a number from a binomial distribution (i.e. integer bell curve).
  static uint32_t Binomial(uint32_t t, double p = .5);
  // Get a normally distributed number (mean = 0, sigma= 1).
  static double Normal();
};



#endif

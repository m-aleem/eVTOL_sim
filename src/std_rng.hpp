#ifndef STD_RNG_HPP
#define STD_RNG_HPP

#include "interface_rng.hpp"
#include <random>

class StdRandomGenerator : public RandomGenerator {
public:
    bool bernoulli(double p) override;
};

#endif

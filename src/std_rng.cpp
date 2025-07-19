#include "std_rng.hpp"
#include <random>

bool StdRandomGenerator::bernoulli(double p) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::bernoulli_distribution d(p);
    return d(gen);
}

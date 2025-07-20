#include "std_rng.hpp"
#include <random>

std::mt19937& StdRandomGenerator::getEngine() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

bool StdRandomGenerator::bernoulli(double p) {
    std::bernoulli_distribution d(p);
    return d(getEngine());
}

int StdRandomGenerator::uniformInt(int min, int max) {
    std::uniform_int_distribution<int> d(min, max);
    return d(getEngine());
}

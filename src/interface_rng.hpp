#ifndef INTERFACE_RNG_HPP
#define INTERFACE_RNG_HPP

class RandomGenerator {
public:
    virtual ~RandomGenerator() = default;
    virtual bool bernoulli(double p) = 0;
    virtual int uniformInt(int min, int max) = 0;
};

#endif
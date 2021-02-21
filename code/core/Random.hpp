#pragma once

#include <algorithm>
#include <cmath>
#include <initializer_list>
#include <iterator>
#include <random>
#include "Assertions.hpp"


template <typename RandomTraits>
class RandomBase
{
public:
    using Generator = typename RandomTraits::GeneratorType;

    template <typename T>
    static T uniform(Generator& generator = RandomTraits::generator());
    template <typename T>
    static T uniform(T to, Generator& generator = RandomTraits::generator());
    template <typename T>
    static T uniform(T from, T to, Generator& generator = RandomTraits::generator());
    template <typename T>
    static T probability(Generator& generator = RandomTraits::generator());

    template <typename T>
    static T uniformf(Generator& generator = RandomTraits::generator());
    template <typename T>
    static T uniformf(T to, Generator& generator = RandomTraits::generator());
    template <typename T>
    static T uniformf(T from, T to, Generator& generator = RandomTraits::generator());
    template <typename T>
    static T probabilityf(Generator& generator = RandomTraits::generator());

    static bool yesNo(Generator& generator = RandomTraits::generator());

    template <typename T>
    static T normalf(T mean, T stddev, Generator& generator = RandomTraits::generator());

    template <typename C>
    static typename C::value_type uniformFrom(const C& collection, Generator& generator = RandomTraits::generator());

    template <typename C>
    static typename C::value_type weightedFrom(const std::vector<float>& weights,
        const C& collection,
        Generator& generator = RandomTraits::generator());

    template <class RandomAccessIterator>
    static void shuffle(RandomAccessIterator first,
        RandomAccessIterator last,
        Generator& generator = RandomTraits::generator());

    template <typename T>
    static T triangularf(T a, T b, T c, Generator& generator = RandomTraits::generator());
};

// implementation

template <typename RandomTraits>
template <typename T>
T RandomBase<RandomTraits>::triangularf(T a, T b, T c, Generator& generator)
{
    static_assert(std::is_floating_point<T>::value, "Floating point required.");

    //
    // https://en.wikipedia.org/wiki/Triangular_distribution#Generating_triangular-distributed_random_variates
    //
    const auto u = RandomBase::uniformf<T>(generator);
    const auto f = (c - a) / (b - a);

    if (u < f) {
        return a + std::sqrt(u * (b - a) * (c - a));
    } else {
        return b - std::sqrt((1 - u) * (b - a) * (b - c));
    }
}

template <typename RandomTraits>
template <typename T>
T RandomBase<RandomTraits>::normalf(T mean, T stddev, Generator& generator)
{
    static_assert(std::is_floating_point<T>::value, "Floating point required.");
    std::normal_distribution<T> dis{mean, stddev};
    return dis(generator);
}

template <typename RandomTraits>
inline bool RandomBase<RandomTraits>::yesNo(Generator& generator)
{
    return static_cast<bool>(uniform<int>(0, 1, generator));
}

template <typename RandomTraits>
template <typename T>
T RandomBase<RandomTraits>::probability(Generator& generator)
{
    static_assert(std::is_integral<T>::value, "Integral required.");
    std::uniform_int_distribution<T> dis(static_cast<T>(0), static_cast<T>(100));
    return dis(generator);
}

template <typename RandomTraits>
template <typename T>
T RandomBase<RandomTraits>::probabilityf(Generator& generator)
{
    static_assert(std::is_floating_point<T>::value, "Floating point required.");
    std::uniform_real_distribution<T> dis(
        static_cast<T>(0.f), static_cast<T>(std::nextafter(1.f, std::numeric_limits<T>::max())));
    // nextafter used to simulate closed interval
    return dis(generator);
}

template <typename RandomTraits>
template <typename T>
T RandomBase<RandomTraits>::uniform(Generator& generator)
{
    static_assert(std::is_integral<T>::value, "Integral required.");

    std::uniform_int_distribution<T> dis;
    return dis(generator);
}

template <typename RandomTraits>
template <typename T>
T RandomBase<RandomTraits>::uniform(T to, Generator& generator)
{
    static_assert(std::is_integral<T>::value, "Integral required.");

    std::uniform_int_distribution<T> dis(static_cast<T>(0), to);
    return dis(generator);
}

template <typename RandomTraits>
template <typename T>
T RandomBase<RandomTraits>::uniform(T from, T to, Generator& generator)
{
    static_assert(std::is_integral<T>::value, "Integral required.");

    std::uniform_int_distribution<T> dis(from, to);
    return dis(generator);
}

template <typename RandomTraits>
template <typename T>
T RandomBase<RandomTraits>::uniformf(Generator& generator)
{
    static_assert(std::is_floating_point<T>::value, "Floating point required.");

    //
    // INFO: Don't add nextafter, correct range is (0, 1]
    //
    std::uniform_real_distribution<T> dis(static_cast<T>(0), static_cast<T>(1));
    return dis(generator);
}

template <typename RandomTraits>
template <typename T>
T RandomBase<RandomTraits>::uniformf(T to, Generator& generator)
{
    static_assert(std::is_floating_point<T>::value, "Floating point required.");

    std::uniform_real_distribution<T> dis(
        static_cast<T>(0), static_cast<T>(std::nextafter(to, std::numeric_limits<T>::max())));
    return dis(generator);
}

template <typename RandomTraits>
template <typename T>
T RandomBase<RandomTraits>::uniformf(T from, T to, Generator& generator)
{
    static_assert(std::is_floating_point<T>::value, "Floating point required.");

    std::uniform_real_distribution<T> dis(from, to);
    return dis(generator);
}

template <typename RandomTraits>
template <class RandomAccessIterator>
void RandomBase<RandomTraits>::shuffle(RandomAccessIterator first, RandomAccessIterator last, Generator& generator)
{
    //
    // std::random_shuffle is depricated and removed in C++17, don't use it
    //
    // https://meetingcpp.com/blog/items/stdrandom_shuffle-is-deprecated.html
    //
    std::shuffle(first, last, generator);
}

template <typename RandomTraits>
template <typename C>
typename C::value_type RandomBase<RandomTraits>::weightedFrom(const std::vector<float>& weights,
    const C& collection,
    Generator& generator)
{
    std::discrete_distribution<size_t> dis(weights.begin(), weights.end());

    auto offset = dis(generator);

    auto it = collection.begin();
    using OffsetType = typename std::iterator_traits<decltype(it)>::difference_type;
    std::advance(it, static_cast<OffsetType>(offset));

    ally_assert(it != collection.end());

    return *it;
}

//
// Don't change type 'C::value_type' to reference type, because we can
// pick item from collection on rvalue object e.g.'uniformFrom(getMyFancyVector())'
//
template <typename RandomTraits>
template <typename C>
typename C::value_type RandomBase<RandomTraits>::uniformFrom(const C& collection, Generator& generator)
{
    //
    // TODO: uncomment this line when C++17 would be available
    //
    // auto offset = uniform(std::size(collection) - 1);
    //

    ally_assert(!collection.empty());

    auto offset = uniform(collection.size() - 1, generator);

    auto it = collection.begin();
    using OffsetType = typename std::iterator_traits<decltype(it)>::difference_type;
    std::advance(it, static_cast<OffsetType>(offset));

    ally_assert(it != collection.end());

    return *it;
}

//
// use types below
//

struct FastRandomTraits
{
    using GeneratorType = std::mt19937;
    static GeneratorType& generator();
};

struct ServerRandomTraits
{
    using GeneratorType = std::mt19937_64;
    static GeneratorType& generator();
};

using Random = RandomBase<FastRandomTraits>;
using ServerRandom = RandomBase<ServerRandomTraits>;

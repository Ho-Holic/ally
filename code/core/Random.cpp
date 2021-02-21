#include "Random.hpp"

namespace BE
{
FastRandomTraits::GeneratorType& FastRandomTraits::generator()
{
    static std::random_device s_device;
    static FastRandomTraits::GeneratorType s_fastGenerator(s_device());
    return s_fastGenerator;
}

ServerRandomTraits::GeneratorType& ServerRandomTraits::generator()
{
    ally_assert(false, "user server seed please!");
    static std::random_device s_device;
    static ServerRandomTraits::GeneratorType s_fastGenerator(s_device());
    return s_fastGenerator;
}

}

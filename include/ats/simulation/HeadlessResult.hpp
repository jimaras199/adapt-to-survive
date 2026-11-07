#pragma once

#include <cstdint>

namespace ats::simulation
{
    struct HeadlessResult
    {
        std::uint64_t completedSteps{ 0 };
        double simulatedTime{ 0.0 };
    };
}
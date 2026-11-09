#pragma once

namespace ats::world
{
    struct SurvivalDepletionConfig
    {
        double foodRate{ 100.0 / 60.0 };
        double waterRate{ (100.0 / 60.0) * 1.15 };
    };
}
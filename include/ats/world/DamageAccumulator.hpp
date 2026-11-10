#pragma once

namespace ats::world
{
    struct DamageAccumulator
    {
        double internalDamage{ 0.0 };
        double externalDamage{ 0.0 };
    };
}
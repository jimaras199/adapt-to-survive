
#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <limits>

namespace ats::simulation
{
    class FixedStepRunner
    {
    public:
        explicit FixedStepRunner(double fixedDeltaTime)
            : fixedDeltaTime_(fixedDeltaTime)
        {
            if (!std::isfinite(fixedDeltaTime_) ||
                fixedDeltaTime_ <= 0.0)
            {
                throw std::invalid_argument(
                    "Fixed delta time must be positive and finite"
                );
            }
        }

        template <typename StepCallback>
        void RunSteps(std::uint64_t count, StepCallback&& callback)
        {
            if (count > std::numeric_limits<std::uint64_t>::max() - completedSteps_)
            {
                throw std::overflow_error("Completed step counter would overflow");
            }
            for (std::uint64_t i = 0; i < count; ++i)
            {
                callback(fixedDeltaTime_);
                ++completedSteps_;
            }
        }

        [[nodiscard]] double FixedDeltaTime() const
        {
            return fixedDeltaTime_;
        }

        [[nodiscard]] std::uint64_t CompletedSteps() const
        {
            return completedSteps_;
        }

        [[nodiscard]] double SimulatedTime() const
        {
            return static_cast<double>(completedSteps_) * fixedDeltaTime_;
        }

    private:
        double fixedDeltaTime_;
        std::uint64_t completedSteps_ = 0;
    };
}

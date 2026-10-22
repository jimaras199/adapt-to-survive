#include <iostream>
#include "ats/math/Vector2.hpp"

int main()
{
    ats::math::Vector2 position{ 10.0, 20.0 };
    ats::math::Vector2 movement{ 3.0, -2.0 };

    const ats::math::Vector2 newPosition = position + movement;

    std::cout << "Adapt to Survive\n";
    std::cout << "Original position: "
        << position.x << ", " << position.y << '\n';

    std::cout << "New position: "
        << newPosition.x << ", " << newPosition.y << '\n';

    return 0;
}

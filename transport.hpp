#pragma once

#include <random>

enum class CollisionType
{
    Absorbed,
    Scattered
};

enum class StepOutcome
{
    Scattered,
    Reflected,
    Transmitted,
    Absorbed
};

double sampleFreePath(double sigmaTotal, std::mt19937_64 &rng);
CollisionType handleCollision(double sigmaAbs, double sigmaTotal, std::mt19937_64 &rng);
StepOutcome stepNeutron(double &position, int &direction, double slabThickness, double sigmaAbs, double sigmaTotal, std::mt19937_64 &rng);

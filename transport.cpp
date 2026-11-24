#include "transport.hpp"
#include <cmath>

static std::uniform_real_distribution<double>
    dist(0.0, 1.0);

double sampleFreePath(double sigmaTotal, std::mt19937_64 &rng)
{
    double r = 1.0 - dist(rng);
    return -std::log(r) / sigmaTotal;
}

CollisionType handleCollision(double sigmaAbs, double sigmaTotal, std::mt19937_64 &rng)
{
    double u = dist(rng);

    if (u < sigmaAbs / sigmaTotal)
    {
        return CollisionType::Absorbed;
    }
    else
    {
        return CollisionType::Scattered;
    }
}

StepOutcome stepNeutron(double &position, int &direction, double slabThickness, double sigmaAbs, double sigmaTotal, std::mt19937_64 &rng)
{
    double step = sampleFreePath(sigmaTotal, rng);
    position += direction * step;

    if (position < 0.0)
    {
        return StepOutcome::Reflected;
    }
    else if (position > slabThickness)
    {
        return StepOutcome::Transmitted;
    }
    else
    {
        if (handleCollision(sigmaAbs, sigmaTotal, rng) == CollisionType::Absorbed)
        {
            return StepOutcome::Absorbed;
        }
        else
        {
            direction *= -1;
            return StepOutcome::Scattered;
        }
    }
}
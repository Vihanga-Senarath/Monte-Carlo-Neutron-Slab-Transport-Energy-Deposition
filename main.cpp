#include "transport.hpp"
#include "io.hpp"
#include <iostream>
#include <random>
#include <vector>

int main()
{
    std::random_device rd;
    std::mt19937_64 rng(rd());

    double slabThickness = 10.0;
    double sigmaAbs = 0.1;
    double sigmaTotal = 0.5;
    double position;
    int direction;
    double neutronEnergy;

    int numBins = 100000;
    double binWidth = slabThickness / numBins;
    std::vector<double> energyDeposit(numBins, 0.0);

    int numNeutrons = 100000;
    int absorbedCount = 0;
    int transmittedCount = 0;
    int reflectedCount = 0;

    for (int i = 0; i < numNeutrons; i++)
    {
        position = 0.0;
        direction = 1;
        neutronEnergy = 2.5;

        StepOutcome outcome = StepOutcome::Scattered;
        while (outcome == StepOutcome::Scattered)
        {
            outcome = stepNeutron(position, direction, slabThickness, sigmaAbs, sigmaTotal, rng);
        }

        switch (outcome)
        {
        case StepOutcome::Absorbed:
        {
            int bin = static_cast<int>(position / binWidth);
            if (bin >= 0 && bin < numBins)
            {
                energyDeposit[bin] += neutronEnergy;
            }
            absorbedCount++;
            break;
        }
        case StepOutcome::Transmitted:
        {
            transmittedCount++;
            break;
        }
        case StepOutcome::Reflected:
        {
            reflectedCount++;
            break;
        }
        }
    }

    std::cout << "Absorbed Count: " << absorbedCount << std::endl;
    std::cout << "Transmitted Count: " << transmittedCount << std::endl;
    std::cout << "Reflected Count: " << reflectedCount << std::endl;

    if (!writeDepositProfileCsv("deposit_profile.csv", slabThickness, energyDeposit))
    {
        return 1;
    }
    
    return 0;
}
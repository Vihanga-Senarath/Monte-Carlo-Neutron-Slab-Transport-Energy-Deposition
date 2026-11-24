#include "config.hpp"
#include "transport.hpp"
#include "io.hpp"
#include <iostream>
#include <random>
#include <vector>

int main()
{
    SimulationConfig simulationConfig;

    std::random_device rd;
    std::mt19937_64 rng(rd());

    double binWidth = simulationConfig.binWidth();
    std::vector<double> energyDeposit(simulationConfig.numBins, 0.0);

    std::size_t absorbedCount = 0;
    std::size_t transmittedCount = 0;
    std::size_t reflectedCount = 0;

    for (std::size_t i = 0; i < simulationConfig.numNeutrons; i++)
    {
        double position = 0.0;
        int direction = 1;
        double neutronEnergy = simulationConfig.neutronEnergy;

        StepOutcome outcome = StepOutcome::Scattered;
        while (outcome == StepOutcome::Scattered)
        {
            outcome = stepNeutron(position, direction, simulationConfig.slabThickness, simulationConfig.sigmaAbs, simulationConfig.sigmaTotal, rng);
        }

        switch (outcome)
        {
        case StepOutcome::Absorbed:
        {
            int bin = static_cast<int>(position / binWidth);
            if (bin >= 0 && bin < simulationConfig.numBins)
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

    if (!writeDepositProfileCsv(simulationConfig.outputCsv, simulationConfig.slabThickness, energyDeposit))
    {
        return 1;
    }

    return 0;
}
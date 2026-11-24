#include "config.hpp"
#include "transport.hpp"
#include "io.hpp"
#include <iostream>
#include <random>
#include <vector>

int main()
{
    SimulationConfig simulationConfig;

    std::vector<CrossSectionPoint> crossSections;
    const int targetMt = 2;
    if (!loadEndfMf3Table("h-002_He_003.endf", targetMt, crossSections))
    {
        return 1;
    }

    std::cout << "Loaded " << crossSections.size() << " points for MT=" << targetMt
              << " (first E=" << crossSections.front().energyEv
              << " eV, sigma=" << crossSections.front().crossSectionBarns << " b)\n";

    constexpr double mevToEv = 1.0e6;
    constexpr double barnsToCm2 = 1.0e-24;

    const double energyEv = simulationConfig.neutronEnergy * mevToEv;
    const double sigmaBarns = interpolateSigmaBarns(crossSections, energyEv);
    const double sigmaCm2 = sigmaBarns * barnsToCm2;
    const double macroscopicSigmaCmInv = simulationConfig.numberDensityCm3 * sigmaCm2;

    const double activeSigmaTotal = macroscopicSigmaCmInv;
    const double activeSigmaAbs = (simulationConfig.sigmaTotal > 0.0)
                                      ? activeSigmaTotal * (simulationConfig.sigmaAbs / simulationConfig.sigmaTotal)
                                      : 0.0;

    std::cout << "At E=" << energyEv << " eV: sigma=" << sigmaBarns
              << " b, Sigma_t=" << activeSigmaTotal
              << " cm^-1, Sigma_a=" << activeSigmaAbs << " cm^-1\n";

    std::random_device rd;
    std::mt19937_64 rng(rd());

    const double binWidth = simulationConfig.binWidth();
    std::vector<double> energyDeposit(simulationConfig.numBins, 0.0);

    std::size_t absorbedCount = 0;
    std::size_t transmittedCount = 0;
    std::size_t reflectedCount = 0;

    for (std::size_t i = 0; i < simulationConfig.numNeutrons; ++i)
    {
        double position = 0.0;
        int direction = 1;
        double neutronEnergy = simulationConfig.neutronEnergy;

        StepOutcome outcome = StepOutcome::Scattered;
        while (outcome == StepOutcome::Scattered)
        {
            outcome = stepNeutron(position, direction, simulationConfig.slabThickness, activeSigmaAbs, activeSigmaTotal, rng);
        }

        switch (outcome)
        {
        case StepOutcome::Absorbed:
        {
            const int bin = static_cast<int>(position / binWidth);
            if (bin >= 0 && bin < simulationConfig.numBins)
            {
                energyDeposit[bin] += neutronEnergy;
            }
            ++absorbedCount;
            break;
        }
        case StepOutcome::Transmitted:
        {
            ++transmittedCount;
            break;
        }
        case StepOutcome::Reflected:
        {
            ++reflectedCount;
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
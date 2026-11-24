#pragma once

#include <cstddef>
#include <string>

struct SimulationConfig
{
    std::size_t numNeutrons = 100000;
    double slabThickness = 10.0; // cm
    int numBins = 10;
    double neutronEnergy = 2.5;       // MeV
    double numberDensityCm3 = 1.0e19; // atoms/cm^3
    double sigmaAbs = 0.1;            // cm^-1 (baseline/fallback)
    double sigmaTotal = 0.5;          // cm^-1 (baseline/fallback)
    std::string outputCsv = "energy_deposit_profile.csv";

    double binWidth() const
    {
        return (numBins > 0) ? (slabThickness / numBins) : 0.0;
    }
};
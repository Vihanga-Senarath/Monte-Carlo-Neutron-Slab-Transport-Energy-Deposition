#pragma once

#include <cstddef>
#include <string>

struct SimulationConfig {
    double slabThickness = 10.0;
    double sigmaAbs = 0.1;
    double sigmaTotal = 0.5;
    double neutronEnergy = 2.5;
    int numBins = 100000;
    std::size_t numNeutrons = 100000;
    std::string outputCsv = "deposit_profile.csv";

    double binWidth() const
    {
        return (numBins > 0) ? (slabThickness / numBins) : 0.0;
    }
};
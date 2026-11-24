#include "io.hpp"
#include <fstream>
#include <iostream>

bool writeDepositProfileCsv(const std::string &filename, double slabThickness, const std::vector<double> &energyDeposit)
{
    std::ofstream outFile(filename);
    if (!outFile.is_open())
    {
        std::cerr << "Error: could not open output file '" << filename << "'\n";
        return false;
    }

    outFile << "bin_index,bin_start_cm,bin_end_cm,energy_mev\n";

    const int numBins = static_cast<int>(energyDeposit.size());
    const double binWidth = (numBins > 0) ? (slabThickness / numBins) : 0.0;

    for (int b = 0; b < numBins; ++b)
    {
        const double binStart = b * binWidth;
        const double binEnd = (b + 1) * binWidth;
        outFile << b << "," << binStart << "," << binEnd << "," << energyDeposit[b] << "\n";
    }

    return true;
}
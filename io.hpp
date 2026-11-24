#pragma once

#include <string>
#include <vector>

struct CrossSectionPoint
{
    double energyEv = 0.0;
    double crossSectionBarns = 0.0;
};

bool writeDepositProfileCsv(const std::string &filename, double slabThickness, const std::vector<double> &energyDeposit);

bool loadEndfMf3Table(const std::string &filename, int targetMt, std::vector<CrossSectionPoint> &outPoints);

double interpolateSigmaBarns(const std::vector<CrossSectionPoint> &table, double energyEv);
#pragma once

#include <string>
#include <vector>

bool writeDepositProfileCsv(const std::string &filename, double slabThickness, const std::vector<double> &energyDeposit);
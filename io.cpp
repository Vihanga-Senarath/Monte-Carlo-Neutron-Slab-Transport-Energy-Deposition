#include "io.hpp"
#include <algorithm> // std::min
#include <cctype>    // std::isspace
#include <fstream>
#include <iostream>

namespace
{
    bool parseEndfFloat(const std::string &rawField, double &outValue)
    {
        std::string s;
        s.reserve(rawField.size() + 2);

        for (std::size_t i = 0; i < rawField.size(); ++i)
        {
            char c = rawField[i];
            if (std::isspace(static_cast<unsigned char>(c)))
            {
                continue;
            }

            if ((c == '+' || c == '-') && i > 0)
            {
                char prev = s.empty() ? '\0' : s.back();
                if (prev != 'e' && prev != 'E')
                {
                    s.push_back('e');
                }
            }
            s.push_back(c);
        }

        if (s.empty())
        {
            return false;
        }

        try
        {
            std::size_t processed = 0;
            double val = std::stod(s, &processed);
            if (processed == 0)
            {
                return false;
            }
            outValue = val;
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool extractMfMt(const std::string &line, int &mf, int &mt)
    {
        if (line.size() < 75)
        {
            return false;
        }

        try
        {
            std::string mfStr = line.substr(70, 2);
            std::string mtStr = line.substr(72, 3);
            mf = std::stoi(mfStr);
            mt = std::stoi(mtStr);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    std::vector<double> extractLineFloats(const std::string &line)
    {
        std::vector<double> values;
        const std::size_t fieldWidth = 11;
        const std::size_t maxFields = 6;

        for (std::size_t f = 0; f < maxFields; ++f)
        {
            std::size_t start = f * fieldWidth;
            if (start >= line.size())
            {
                break;
            }
            std::size_t len = std::min(fieldWidth, line.size() - start);
            std::string field = line.substr(start, len);

            double val = 0.0;
            if (parseEndfFloat(field, val))
            {
                values.push_back(val);
            }
        }
        return values;
    }
}

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

bool loadEndfMf3Table(const std::string &filename, int targetMt, std::vector<CrossSectionPoint> &outPoints)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
    {
        std::cerr << "Error: could not open ENDF file '" << filename << "'\n";
        return false;
    }

    outPoints.clear();

    std::string line;
    int targetDataLinesSeen = 0;
    std::vector<double> rawPairsStream;

    while (std::getline(inFile, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        int mf = 0;
        int mt = 0;
        if (!extractMfMt(line, mf, mt))
        {
            continue;
        }

        if (mf != 3 || mt != targetMt)
        {
            if (targetDataLinesSeen > 0 && (mf != 3 || mt != targetMt))
            {
                break;
            }
            continue;
        }

        if (mt == 0)
        {
            continue;
        }

        std::vector<double> floats = extractLineFloats(line);
        if (floats.empty())
        {
            continue;
        }

        if (targetDataLinesSeen < 3)
        {
            ++targetDataLinesSeen;
            continue;
        }
        ++targetDataLinesSeen;

        for (double v : floats)
        {
            rawPairsStream.push_back(v);
        }
    }

    if (rawPairsStream.size() < 2)
    {
        std::cerr << "Error: no MF=3, MT=" << targetMt
                  << " cross-section pairs found in '" << filename << "'\n";
        return false;
    }

    for (std::size_t i = 0; i + 1 < rawPairsStream.size(); i += 2)
    {
        CrossSectionPoint pt;
        pt.energyEv = rawPairsStream[i];
        pt.crossSectionBarns = rawPairsStream[i + 1];
        outPoints.push_back(pt);
    }

    return !outPoints.empty();
}

double interpolateSigmaBarns(const std::vector<CrossSectionPoint> &table,
                             double energyEv)
{
    if (table.empty())
    {
        return 0.0;
    }

    if (energyEv <= table.front().energyEv)
    {
        return table.front().crossSectionBarns;
    }

    if (energyEv >= table.back().energyEv)
    {
        return table.back().crossSectionBarns;
    }

    const auto it = std::lower_bound(
        table.begin(),
        table.end(),
        energyEv,
        [](const CrossSectionPoint &pt, double targetEnergy)
        {
            return pt.energyEv < targetEnergy;
        });

    if (it == table.begin())
    {
        return table.front().crossSectionBarns;
    }

    if (it == table.end())
    {
        return table.back().crossSectionBarns;
    }

    const CrossSectionPoint &high = *it;
    const CrossSectionPoint &low = *(it - 1);
    const double dE = high.energyEv - low.energyEv;

    if (dE <= 0.0)
    {
        return low.crossSectionBarns;
    }

    const double fraction = (energyEv - low.energyEv) / dE;
    return low.crossSectionBarns + fraction * (high.crossSectionBarns - low.crossSectionBarns);
}
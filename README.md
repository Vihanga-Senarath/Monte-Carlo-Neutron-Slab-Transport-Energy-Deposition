# Monte Carlo Neutron Slab Transport & Energy Deposition

A C++ Monte Carlo prototype simulating 1D neutron transport through a finite slab (`[0.0, slabThickness]`), tracking particle outcomes (**Absorbed**, **Transmitted**, **Reflected**) and spatial energy deposition across depth bins as part of the **Neutron Deposit Study** capstone.

## Project Structure

- `config.hpp` - `SimulationConfig` struct holding slab geometry, cross sections, neutron energy, binning, particle count, and output CSV path.
- `transport.hpp` / `transport.cpp` - Core Monte Carlo transport routines (`sampleFreePath`, `handleCollision`, `stepNeutron`) and collision/step outcome enums.
- `io.hpp` / `io.cpp` - CSV output writer (`writeDepositProfileCsv`) formatting `bin_index,bin_start_cm,bin_end_cm,energy_mev`.
- `main.cpp` - Simulation setup, neutron history loop, outcome tallies, and I/O orchestration.
- `plot_deposit.py` - Python visualization script using `csv.DictReader` and `matplotlib` `plt.step(..., where="post")` to inspect energy deposition profiles.

## What This Prototype Models

- **Exponential Free-Path Sampling (`sampleFreePath`)**: Draws step lengths $s = -\ln(r) / \Sigma_t$ with $r \in (0.0, 1.0]$ using `std::mt19937_64` and a uniform real distribution.
- **Collision Branching (`handleCollision`)**: Evaluates absorption vs. scattering at each collision inside the slab using probability $P_a = \Sigma_a / \Sigma_t$.
- **1D Boundary & Scatter Stepping (`stepNeutron`)**: Advances neutron position along `direction` (`+1` or `-1`), checks front (`x < 0.0` $\rightarrow$ `Reflected`) and back (`x > slabThickness` $\rightarrow$ `Transmitted`) boundaries, and reverses `direction` on scatter.
- **History Loop & Spatial Tallies (`main`)**: Simulates `numNeutrons` histories, tallies outcome counts, and accumulates `neutronEnergy` into `numBins` depth bins upon absorption.

## Build and Run

Compile with `g++` (C++17 or later):

```bash
g++ -std=c++17 -O2 main.cpp -o main
./main
```

On Windows PowerShell:

```powershell
g++ -std=c++17 -O2 main.cpp -o main; .\main.exe
```

## Sample Run (`1000` Neutrons)

With `slabThickness = 10.0`, `sigmaAbs = 0.1`, `sigmaTotal = 0.5`, `neutronEnergy = 2.5` MeV, and `10` bins:

- **Absorbed Count**: `474`
- **Transmitted Count**: `33`
- **Reflected Count**: `493`
- **Total Deposited Energy**: `1185 MeV` ($474 \times 2.5\text{ MeV}$), concentrated near the front face (`[0, 1) cm`: `362.5 MeV`) and tapering with depth (`[9, 10) cm`: `7.5 MeV`).

## Roadmap & Next Steps

This prototype establishes the initial Monte Carlo slab transport baseline on the way toward the **Neutron Deposit Study** capstone and broader computational nuclear physics goals:

- **2D Spatial Grid**: Extend particle state to `(x, y)` and tally deposit energy across 2D bins.
- **Performance & HPC**: Parallelize histories with `std::thread` or OpenMP and accelerate linear algebra with Eigen.
- **Physics & Data Integration**: Incorporate macroscopic cross sections $\Sigma = n\sigma$ from ENDF/B-VIII.0 data and validate against reference models.
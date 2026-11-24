# Monte Carlo Neutron Slab Transport & Energy Deposition

A C++ Monte Carlo prototype simulating 1D neutron transport through a finite slab (`[0.0, slabThickness]`), tracking particle outcomes (**Absorbed**, **Transmitted**, **Reflected**) and spatial energy deposition across depth bins as part of the **Neutron Deposit Study** capstone.

## Project Structure

- `config.hpp` - `SimulationConfig` struct holding slab geometry, cross sections, neutron energy, atomic number density (`numberDensityCm3`), binning, particle count, and output CSV path.
- `transport.hpp` / `transport.cpp` - Core Monte Carlo transport routines (`sampleFreePath`, `handleCollision`, `stepNeutron`) and collision/step outcome enums.
- `io.hpp` / `io.cpp` - CSV output writer (`writeDepositProfileCsv`), ENDF/B-VIII.0 `MF=3` cross-section table loader (`loadEndfMf3Table`), and linear cross-section interpolation (`interpolateSigmaBarns`).
- `main.cpp` - Simulation setup, ENDF cross-section loading/interpolation, macroscopic cross-section calculation ($\Sigma = n\sigma$), neutron history loop, outcome tallies, and I/O orchestration.
- `plot_deposit.py` - Python visualization script using `csv.DictReader` and `matplotlib` `plt.step(..., where="post")` to inspect energy deposition profiles.

## What This Prototype Models

- **Exponential Free-Path Sampling (`sampleFreePath`)**: Draws step lengths $s = -\ln(r) / \Sigma_t$ with $r \in (0.0, 1.0]$ using `std::mt19937_64` and a uniform real distribution.
- **Collision Branching (`handleCollision`)**: Evaluates absorption vs. scattering at each collision inside the slab using probability $P_a = \Sigma_a / \Sigma_t$.
- **1D Boundary & Scatter Stepping (`stepNeutron`)**: Advances neutron position along `direction` (`+1` or `-1`), checks front (`x < 0.0` $\rightarrow$ `Reflected`) and back (`x > slabThickness` $\rightarrow$ `Transmitted`) boundaries, and reverses `direction` on scatter.
- **ENDF/B-VIII.0 `MF=3` Lookup & Unit Conversion (`io.cpp`, `main.cpp`)**: Parses `(E [eV], sigma [b])` pairs from a requested `MT` channel (normalizing FORTRAN exponent notation and skipping `TAB1` control records), linearly interpolates $\sigma(E)$, converts barns to $\text{cm}^2$ ($1\text{ b} = 10^{-24}\text{ cm}^2$), and computes $\Sigma = n\sigma$ using `numberDensityCm3`.
- **History Loop & Spatial Tallies (`main`)**: Simulates `numNeutrons` histories, tallies outcome counts, and accumulates `neutronEnergy` into `numBins` depth bins upon absorption.

## Input Data Requirement

Place the required ENDF file (currently `h-002_He_003.endf` in the working directory for `MT=2`) before running the executable; `main.cpp` exits with an error if the file cannot be opened or no `MF=3` pairs are found for the target `MT`. Note that `h-002_He_003.endf` is an incident $^3\text{He}$ evaluation used here to validate `MF=3` parsing and cross-section conversion before swapping in a neutron evaluation.

## Build and Run

Compile with `g++` (C++17 or later):

```bash
g++ -std=c++17 -O2 main.cpp transport.cpp io.cpp -o main
./main

On Windows PowerShell:

```powershell
g++ -std=c++17 -O2 main.cpp transport.cpp io.cpp -o main; .\main.exe
```

## Sample Run (`1000` Neutrons)

With `slabThickness = 10.0`, `sigmaAbs = 0.1`, `sigmaTotal = 0.5`, `neutronEnergy = 2.5` MeV, and `10` bins:

- **Absorbed Count**: `474`
- **Transmitted Count**: `33`
- **Reflected Count**: `493`
- **Total Deposited Energy**: `1185 MeV` ($474 \times 2.5\text{ MeV}$), concentrated near the front face (`[0, 1) cm`: `362.5 MeV`) and tapering with depth (`[9, 10) cm`: `7.5 MeV`).

## Roadmap & Next Steps

This prototype establishes the initial Monte Carlo slab transport baseline on the way toward the **Neutron Deposit Study** capstone and broader computational nuclear physics goals:

- **Dynamic Energy-Dependent Transport**: Evaluate $\sigma(E)$ and $\Sigma(E)$ dynamically inside the transport loop as particle energy changes across scattering steps, and add realistic scattering energy loss.
- **Neutron Evaluation & Reaction Channels**: Switch to a neutron ENDF evaluation and support additional reaction channels (MT) and angle/energy distributions.
- **2D Spatial Grid**: Extend particle state to `(x, y)` and tally deposit energy across 2D bins.
- **Performance & HPC**: Parallelize histories with `std::thread` or OpenMP and accelerate linear algebra with Eigen.
- **Physics & Data Integration**: Incorporate macroscopic cross sections $\Sigma = n\sigma$ from ENDF/B-VIII.0 data and validate against reference models.
import csv
import matplotlib.pyplot as plt

starts = []
energies = []

with open("deposit_profile.csv", newline="") as f:
    reader = csv.DictReader(f)
    for row in reader:
        starts.append(float(row["bin_start_cm"]))
        energies.append(float(row["energy_mev"]))

    plt.step(starts, energies, where="post", color="yellow")
    plt.xlabel("Depth in Slab (cm)")
    plt.ylabel("Deposited Energy (MeV)")
    plt.title("1D Neutron Energy Deposition Profile")
    plt.show()
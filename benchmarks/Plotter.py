import os
import sys
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

if len(sys.argv) > 1:
    # CSV passed as argument
    csv_file = sys.argv[1]

    ans = input("Use log scale? (y/n): ").strip().lower()
    use_log = ans in ("y", "yes")
else:
    # Take .csv as input
    user_input = input(
        "Enter CSV (or 'log <csv>' for log scale): "
    ).strip().split()

    if len(user_input) == 1:
        use_log = False
        csv_file = user_input[0]
    elif len(user_input) == 2 and user_input[0].lower() == "log":
        use_log = True
        csv_file = user_input[1]
    else:
        raise ValueError("Invalid input. Use: <file.csv> or log <file.csv>")

# Extract name of <code.cpp> [brute.cpp] [gen.cpp] from filename
base = os.path.basename(csv_file)
if not base.startswith("benchmark-") or not base.endswith(".csv"):
    raise ValueError("Invalid file name format. Expected benchmark-<name>-<brute>-<gen>.csv")

core = base[len("benchmark-"):-len(".csv")]

parts = core.split("-")

if len(parts) != 3:
    raise ValueError(
        "Invalid file name format. Expected benchmark-<code>-<brute>-<gen>.csv"
    )

code, brute, gen = parts

# Read .csv
df = pd.read_csv(csv_file)

required_cols = {"nodes", "code", "brute"}
if not required_cols.issubset(df.columns):
    raise ValueError(f"CSV must contain columns: {required_cols}")

# Takes mean if multiple number_of_nodes
grouped = df.groupby("nodes", as_index=False).mean()

nodes = grouped["nodes"]
code_time = grouped["code"]
brute_time = grouped["brute"]

if use_log:
    code_time = np.where(code_time > 0, np.log10(code_time), 0)
    brute_time = np.where(brute_time > 0, np.log10(brute_time), 0)

plt.figure(figsize=(10, 6))

plt.plot(nodes, code_time, label=f"{code}.cpp")
plt.plot(nodes, brute_time, label=f"{brute}.cpp")

plt.xlabel("Number of Nodes")
if(use_log):
    plt.ylabel("log10(Execution Time in ms)")
    plt.title(f"Performance Comparison using {gen}.cpp (Mean, Log Scale)")
else:
    plt.ylabel("Execution Time (in ms)")
    plt.title(f"Performance Comparison using {gen}.cpp (Mean)")

plt.legend()
plt.grid(True)
plt.tight_layout()

out_file = csv_file.replace(".csv", "_plot.png")
plt.savefig(out_file, dpi=200)
print(f"Plot saved to {out_file}")

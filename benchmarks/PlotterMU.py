import os
import sys
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

if len(sys.argv) > 1:
    # CSV passed as argument
    csv_file = sys.argv[1]
else:
    # Take .csv as input
    user_input = input(
        "Enter CSV: "
    ).strip().split()
    csv_file = user_input[0]

# Extract name of <code.cpp> [gen.cpp] from filename
base = os.path.basename(csv_file)
if not base.startswith("benchmark-") or not base.endswith("-MU.csv"):
    raise ValueError("Invalid file name format. Expected benchmark-<name>-<gen>-MU.csv")

core = base[len("benchmark-"):-len("-MU.csv")]

parts = core.split("-")

if len(parts) != 2:
    raise ValueError("Invalid file name format. Expected benchmark-<code>-<gen>-MU.csv")

code, gen = parts

# Read .csv
rows = []
with open(csv_file) as f:
    for line_no, line in enumerate(f, 1):
        line = line.strip()
        if not line:
            continue
        rows.append((line_no, line))

data = []

bad_lines = []

for line_no, line in rows:
    parts = [p.strip() for p in line.split(",")]

    try:
        nums = [float(p) for p in parts if p != ""]
        if len(nums) < 2:
            raise ValueError("Too few columns")

        nodes = nums[0]
        create = nums[1]
        updates = nums[2:]

        if len(updates) == 0:
            mean_update = 0.0
        else:
            mean_update = sum(updates) / len(updates)

        rel = (mean_update / create * 100) if create != 0 else 0

        data.append((nodes, rel))

    except Exception as e:
        bad_lines.append((line_no, line, str(e)))

if bad_lines:
    print(f"⚠️ Skipped {len(bad_lines)} malformed lines (showing up to 10):")
    for ln, text, err in bad_lines[:min(10,len(bad_lines))]:
        print(f"  line {ln}: {err} -> {text[:200]!r}")
else:
    print("✅ No malformed lines detected.")

df = pd.DataFrame(data, columns=["Nodes", "RelativeUpdatePercent"])

# Extract X,Y axis
grouped = df.groupby("Nodes", as_index=False)["RelativeUpdatePercent"].mean()

nodes = grouped["Nodes"]
ratio_percent = grouped["RelativeUpdatePercent"]
overall_mean = grouped["RelativeUpdatePercent"].mean()

# Plot
plt.figure(figsize=(10, 6))

plt.plot(nodes, ratio_percent, label="Update_Time / Create_Time (%)")
plt.axhline(
    y=overall_mean,
    color="red",
    linestyle="--",
    linewidth=2,
    label=f"Overall mean = {overall_mean:.2f}%"
)

plt.text(
    nodes.max(),
    overall_mean,
    f"{overall_mean:.2f}%",
    ha="right",
    va="bottom",
    fontsize=10,
    color="orange",       
    fontweight="bold"
)

plt.xlabel("Number of Nodes")
plt.ylabel("Update Time as % of Create Time")
plt.title(f"Relative Update Cost using {gen}.cpp (Mean)")

plt.legend()
plt.grid(True)
plt.tight_layout()

out_file = csv_file.replace(".csv", "_plot.png")
plt.savefig(out_file, dpi=200)
print(f"Plot saved to {out_file}")

import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv(
    "result.txt",
    header=None,
    names=["size", "method", "block_size", "grid_x", "grid_y", "time_sec", "gops", "correct"]
)

cpu_df = df[df["method"] == "CPU"].copy()
gpu_df = df[df["method"] == "GPU"].copy()

cpu_df = cpu_df.sort_values("size")
gpu_df = gpu_df.sort_values(["block_size", "size"])

block_sizes = sorted(gpu_df["block_size"].unique())

for block in block_sizes:
    gpu_block = gpu_df[gpu_df["block_size"] == block].copy()

    plt.figure(figsize=(10, 6))

    plt.plot(
        cpu_df["size"],
        cpu_df["time_sec"],
        marker="o",
        linewidth=2,
        label="None CUDA"
    )

    plt.plot(
        gpu_block["size"],
        gpu_block["time_sec"],
        marker="o",
        linewidth=2,
        label="With CUDA"
    )

    plt.title(f"BLOCK_SIZE = {block}", fontsize=18)
    plt.xlabel("Matrix size", fontsize=12)
    plt.ylabel("Time (s)", fontsize=12)
    plt.grid(True, alpha=0.3)
    plt.legend(loc="lower center", ncol=2, frameon=False)

    plt.tight_layout()
    plt.show()
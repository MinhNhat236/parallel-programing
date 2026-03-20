import matplotlib.pyplot as plt
import pandas as pd

def plot_mpi_by_matrix_size(filename="mpi_execution_times.txt"):
    df = pd.read_csv(filename, sep=" ", names=["Size", "Processes", "Time"])


    process_counts = sorted(df["Processes"].unique())

    plt.figure(figsize=(10, 6))
    
    for proc in process_counts:
        subset = df[df["Processes"] == proc]
        plt.plot(subset["Size"], subset["Time"], marker='o', label=f"{proc} processes")

    plt.title("MPI Performance by Matrix Size")
    plt.xlabel("Matrix Size (N x N)")
    plt.ylabel("Execution Time (seconds)")
    plt.grid(True)
    plt.xticks(sorted(df["Size"].unique()))
    plt.legend(title="MPI Processes")
    plt.tight_layout()
    plt.savefig("mpi_by_matrix_size.png")
    plt.show()

plot_mpi_by_matrix_size()
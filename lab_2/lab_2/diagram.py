import matplotlib.pyplot as plt

# Read data from file
sizes = []
times = {0: [], 2: [], 4: [], 8: []}  # Dictionary to store times for each thread count

with open("execution_times.txt", "r") as file:
    for line in file:
        size, num_threads, time = map(float, line.split())
        if int(num_threads) not in times:
            times[int(num_threads)] = []
        if int(size) not in sizes:
            sizes.append(int(size))
        times[int(num_threads)].append(time)

# Plot the graph
plt.figure(figsize=(10, 6))

# Plot for each thread count
for num_threads, time_list in times.items():
    label = "No threads" if num_threads == 0 else f"{num_threads} threads"
    plt.plot(sizes, time_list, label=label, marker='o')

# Set title and labels
plt.title("Execution Time vs Matrix Size")
plt.xlabel("Matrix Size (Size x Size)")
plt.ylabel("Execution Time (seconds)")
plt.legend()
plt.grid(True)

# Display the graph
plt.show()
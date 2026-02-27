import os
import re
import glob
import matplotlib.pyplot as plt

BASE_DIR = os.path.dirname(os.path.abspath(__file__))

RE_SIZE = re.compile(r"Matrix\s*size:\s*(\d+)\s*x\s*(\d+)", re.IGNORECASE)
RE_TIME = re.compile(r"Execution\s*time:\s*([0-9]*\.?[0-9]+)\s*seconds", re.IGNORECASE)

def parse_result_file(path: str):
    """Return (size, time) from a resultMatrix_*.txt file."""
    with open(path, "r", encoding="utf-8", errors="ignore") as f:
        text = f.read()

    m_size = RE_SIZE.search(text)
    m_time = RE_TIME.search(text)

    if not m_size or not m_time:
        raise ValueError(f"Cannot parse size/time in: {path}")

    n = int(m_size.group(1))
    t = float(m_time.group(1))
    return n, t

def main():
    pattern = os.path.join(BASE_DIR, "resultMatrix_*.txt")
    files = sorted(glob.glob(pattern))

    if not files:
        print(f"No files found: {pattern}")
        return

    data = []
    for fp in files:
        try:
            n, t = parse_result_file(fp)
            data.append((n, t))
        except Exception as e:
            print(f"Skip {os.path.basename(fp)}: {e}")

    if not data:
        print("No valid data to plot.")
        return

    data.sort(key=lambda x: x[0])
    sizes = [x[0] for x in data]
    times = [x[1] for x in data]

    plt.figure()
    plt.plot(sizes, times, marker="o")
    plt.xlabel("Size matrix")
    plt.ylabel("Time")
    plt.title("Time vs Size matrix")
    plt.xticks(sizes)
    plt.grid(True, axis="y", linestyle="--", alpha=0.4)
    plt.show()

if __name__ == "__main__":
    main()
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys

# Define file path (adjust if you run this from root or build folder)
# If you run from root, file is likely in build/latencies.csv
FILE_PATH = 'build/latencies.csv'

def plot_latency_histogram(file_path):
    try:
        print(f"Reading data from {file_path}...")
        df = pd.read_csv(file_path)
        data = df['Latency_NS']

        # Calculate Statistics
        mean_val = np.mean(data)
        p50 = np.percentile(data, 50)
        p99 = np.percentile(data, 99)
        p99_9 = np.percentile(data, 99.9)

        print(f"--- Statistics ---")
        print(f"Count: {len(data)}")
        print(f"Mean:  {mean_val:.2f} ns")
        print(f"P50 (Median): {p50:.2f} ns")
        print(f"P99:   {p99:.2f} ns")
        print(f"P99.9: {p99_9:.2f} ns")

        # Plot Histogram
        plt.figure(figsize=(10, 6))
        
        # We focus on the 99th percentile to zoom in on the main cluster
        # (Filtering out extreme outliers for a better chart)
        filtered_data = data[data < p99 * 2] 

        plt.hist(filtered_data, bins=100, color='#007acc', alpha=0.7, edgecolor='black')
        
        # Add vertical lines for stats
        plt.axvline(mean_val, color='red', linestyle='dashed', linewidth=1, label=f'Mean: {mean_val:.0f}ns')
        plt.axvline(p99, color='orange', linestyle='dashed', linewidth=1, label=f'P99: {p99:.0f}ns')

        plt.title('Order Execution Latency Distribution (C++ Engine)')
        plt.xlabel('Latency (nanoseconds)')
        plt.ylabel('Frequency (Count)')
        plt.legend()
        plt.grid(axis='y', alpha=0.5)

        # Save plot
        plt.savefig('build/latency_histogram.png')
        print("Chart saved as 'latency_histogram.png'")
        # plt.show() # Uncomment if you have a GUI environment

    except FileNotFoundError:
        print(f"Error: Could not find file at {file_path}")
        print("Did you run ./LOB_Benchmark first?")

if __name__ == "__main__":
    # Allow command line argument for file path
    if len(sys.argv) > 1:
        FILE_PATH = sys.argv[1]
    plot_latency_histogram(FILE_PATH)
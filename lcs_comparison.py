import time
import tracemalloc
from suffix_trees import STree
import numpy as np


# Function to find LCS using Generalized Suffix Tree
def lcs_suffix_tree(strings):
    # Start tracing memory allocation
    tracemalloc.start()

    # Build the generalized suffix tree from the list of strings
    stree = STree.STree(strings)

    # Find the longest common substring using the suffix tree's lcs() method
    lcs = stree.lcs()

    # Measure memory usage
    current, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()

    return lcs, peak


# Function to find LCS using Dynamic Programming
def lcs_dynamic_programming(s1, s2):
    # Start tracing memory allocation
    tracemalloc.start()

    m, n = len(s1), len(s2)
    dp = np.zeros((m + 1, n + 1), dtype=int)
    length = 0
    lcs_end = 0

    for i in range(1, m + 1):
        for j in range(1, n + 1):
            if s1[i - 1] == s2[j - 1]:
                dp[i][j] = dp[i - 1][j - 1] + 1
                if dp[i][j] > length:
                    length = dp[i][j]
                    lcs_end = i

    lcs = s1[lcs_end - length: lcs_end]

    # Measure memory usage
    current, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()

    return lcs, peak


# Function to compare LCS methods and measure time and space
def compare_lcs_methods(s1, s2):
    strings = [s1, s2]

    # Measure time and space for suffix tree approach
    start_time = time.time()
    lcs_tree, suffix_tree_memory = lcs_suffix_tree(strings)
    suffix_tree_time = time.time() - start_time

    # Measure time and space for dynamic programming approach
    start_time = time.time()
    lcs_dp, dp_memory = lcs_dynamic_programming(s1, s2)
    dp_time = time.time() - start_time

    # Print the results for both approaches
    print(f"Longest Common Substring (Suffix Tree): {lcs_tree}")
    print(f"Time taken (Suffix Tree): {suffix_tree_time:.6f} seconds")
    print(f"Memory used (Suffix Tree): {suffix_tree_memory / 1024:.2f} KB")
    print(f"Longest Common Substring (Dynamic Programming): {lcs_dp}")
    print(f"Time taken (Dynamic Programming): {dp_time:.6f} seconds")
    print(f"Memory used (Dynamic Programming): {dp_memory / 1024:.2f} KB")


# Main function to read files and find LCS
def main(file1, file2):
    # Read the contents of the files
    with open(file1, 'r') as f1:
        s1 = f1.read().strip()

    with open(file2, 'r') as f2:
        s2 = f2.read().strip()

    # Compare LCS methods
    compare_lcs_methods(s1, s2)


if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Usage: python lcs_comparison.py <file1> <file2>")
        sys.exit(1)

    file1 = sys.argv[1]
    file2 = sys.argv[2]

    main(file1, file2)

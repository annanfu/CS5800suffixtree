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

# Function to compare LCS methods and measure time and space
def compare_lcs_methods(strings):
    # Measure time and space for suffix tree approach
    start_time = time.time()
    lcs_tree, suffix_tree_memory = lcs_suffix_tree(strings)
    suffix_tree_time = time.time() - start_time

    # Dynamic Programming for more than two strings would require a more complex solution,
    # but let's focus on comparing the suffix tree method.

    # Print the results for suffix tree approach
    print(f"Longest Common Substring (Suffix Tree): {lcs_tree}")
    print(f"Time taken (Suffix Tree): {suffix_tree_time:.6f} seconds")
    print(f"Memory used (Suffix Tree): {suffix_tree_memory / 1024:.2f} KB")

# Main function to read files and find LCS
def main(files):
    strings = []
    
    # Read the contents of all the files
    for file in files:
        with open(file, 'r') as f:
            strings.append(f.read().strip())

    # Compare LCS methods
    compare_lcs_methods(strings)

# Example usage
if __name__ == "__main__":
    import sys
    if len(sys.argv) < 3:
        print("Usage: python lcs_comparison.py <file1> <file2> [<file3> ... <fileN>]")
        sys.exit(1)

    files = sys.argv[1:]
    main(files)

# CS5800suffixtree

**About/Overview**

This project demonstrates the analysis of implementation comparison between suffix tree algorithm and other algorithms(dynamic programming/brute force)in C. The main purpose is to compare the performance in time and space of the suffix tree approach against traditional methods in 3 applications: longest repeated substring, longest common substring and substring pattern searching across various dataset sizes and pattern lengths.

**How To Run**

To compile and run the project, you will find three different C files, each tailored for specific applications of suffix tree performance testing. You can compile each file separately and run them with or without an optional argument for specifying the input file. If no file is specified, the program will use test cases(which might need to uncomment the test cases lines in main).
The optional_input_file are datasets sourced from human genomic sequence of chromosome 21 with different sizes. For example:
"testfile100" is the dataset of size 100

1. Substring pattern searching: To run the c file, use the following command in your terminal:
```
gcc -o pattern_search_comparison_annanfu pattern_search_comparison_annanfu.c
./pattern_search_comparison_annanfu [optional_testfile_size]
```
[Longest repeated substring]

[Longest common substring]

**How to Use the Program**

After running the program, it reads the input file(optional), constructs a suffix tree of the input, and then performs the applications for the given pattern with suffix tree and the other algorithm. The results display the comparison of time taken to build the tree, time and memory used to search/find the pattern for two methods, and other information about the result (such as positions and number of occurrences). 

**Assumptions**

- The input files are plain text files without any special encoding, consisting of 'a', 'c', 't', 'g' which are patterns of genomic sequence.
- 3 pattern lengths: 3, 10, 100.
- 6 dataset sizes: 100, 1000, 10000, 100000, 1000000, 10000000.
- The suffix tree algorithm is implemented based on the tutorial on the GeeksforGeeks website. Different implementations or optimizations might yield varying performance results, particularly concerning time efficiency and memory usage.
- Memory usage during the execution is tracked using the cmemcounter.h library, developed by Daniel Lemire. 

**Limitations**

The current implementation may not efficiently handle extremely larger dataset size and pattern input due to memory constraints.
Suffix tree construction can be memory-intensive, which might limit its applicability for constrained environments.
The program is designed for educational and benchmarking purposes and might require optimizations for production-level tasks.

**Citations**
https://www.ncbi.nlm.nih.gov/nuccore/CM000683.2/
https://github.com/lemire/CMemoryUsage
https://www.geeksforgeeks.org/suffix-tree-application-1-substring-check/
https://www.geeksforgeeks.org/suffix-tree-application-2-searching-all-patterns/


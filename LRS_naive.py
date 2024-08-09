# Final Project: Suffix Tree_Longest Repeated Substring
# Comparing Solution: Naive


def longest_repeated_substring_naive(text):
    n = len(text)
    longest_substr = ""
    for i in range(n):
        for j in range(i + 1, n):
            k = 0
            while (i + k < n) and (j + k < n) and (text[i + k] == text[j + k]):
                k += 1
            if k > len(longest_substr):
                longest_substr = text[i:i + k]
    return longest_substr


text = "bananas"
print("Longest Repeated Substring (Naive):",longest_repeated_substring_naive(text))

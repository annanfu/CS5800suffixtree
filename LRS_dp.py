# Final Project: Suffix Tree_Longest Repeated Substring
# Comparing Solution: Dynamic Programming

def longest_repeated_substring_dp(s):
    n = len(s)
    # Create and initialize the DP table
    dp = [[0] * (n + 1) for _ in range(n + 1)]
    longest_end = 0
    longest_length = 0

    # Build the table and find the longest repeated substring
    for i in range(1, n + 1):
        for j in range(i + 1, n + 1):
            # If characters match and are not the same index
            if s[i - 1] == s[j - 1]:
                dp[i][j] = dp[i - 1][j - 1] + 1

                # Ensure substrings do not overlap
                if dp[i][j] > longest_length and (i - dp[i][j]) < j:
                    longest_length = dp[i][j]
                    longest_end = i
            else:
                dp[i][j] = 0

    # Longest repeated substring
    if longest_length > 0:
        return s[longest_end - longest_length: longest_end]
    else:
        return ""


# Example usage
text = "bananas"
print("Longest Repeated Substring (DP):", longest_repeated_substring_dp(text))

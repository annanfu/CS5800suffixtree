// A C program to implement Ukkonen's Suffix Tree Construction
// And find all locations of a pattern in string
#include "cmemcounter.h" // Memory usage tracking
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h> // Time tracking
#define MAX_CHAR 256

struct SuffixTreeNode
{
    // children stores the edge labels to children nodes
    struct SuffixTreeNode *children[MAX_CHAR];

    // pointer to other node via suffix link
    struct SuffixTreeNode *suffixLink;

    /*(start, end) interval specifies the edge, by which the node is connected to its parent node. Each edge will
     connect two nodes,  one parent and one child, and (start, end) interval of a given edge  will be stored
     in the child node. Let's say there are two nods A and B connected by an edge with indices (5, 8) then this
     indices (5, 8) will be stored in node B. */
    int start;
    int *end;

    /*for leaf nodes, it stores the index of suffix for the path from root to leaf, For non-leaf node, it will be -1.*/
    int suffixIndex;
};

typedef struct SuffixTreeNode Node;

char text[10000002];    // Input string

Node *root = NULL; // Pointer to root node

/*lastNewNode will point to the newly created internal node, waiting for it's suffix link to be set, which might get
  a new suffix link (other than root) in next extension of same phase. lastNewNode will be set to NULL when last
  newly created internal node (if there is any) got it's suffix link reset to new internal node created in next
  extension of same phase. */
Node *lastNewNode = NULL;
Node *activeNode = NULL;

/*activeEdge is represented as an input string character index (not the character itself)*/
int activeEdge = -1;
int activeLength = 0;

// remainingSuffixCount tells how many suffixes yet to be added in tree
int remainingSuffixCount = 0;
int leafEnd = -1; // allows multiple nodes to share the same end index value, enabling efficient updates and reducing memory usage
int *rootEnd = NULL; // pointer to the end of the root node
int *splitEnd = NULL; // splitEnd is used to split edge when Rule 2 applies
int size = -1; // Length of input string

Node *newNode(int start, int *end)
{
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node)
    {
        perror("Failed to allocate node");
        exit(EXIT_FAILURE);
    }
    int i;
    for (i = 0; i < MAX_CHAR; i++)
        node->children[i] = NULL;

    /*For root node, suffixLink will be set to NULL For internal nodes, suffixLink will be set to root
    by default in  current extension and may change in next extension*/
    node->suffixLink = root;
    node->start = start;
    node->end = end;

    /*suffixIndex will be set to -1 by default and actual suffix index will be set later for leaves
      at the end of all phases*/
    node->suffixIndex = -1;
    return node;
}

int edgeLength(Node *n)
{
    if (n == root)
        return 0;
    return *(n->end) - (n->start) + 1;
}

int walkDown(Node *currNode)
{
    /*activePoint change for walk down (APCFWD) using Skip/Count Trick  (Trick 1). If activeLength is greater
     than current edge length, set next  internal node as activeNode and adjust activeEdge and activeLength
     accordingly to represent same activePoint*/
    if (activeLength >= edgeLength(currNode))
    {
        activeEdge += edgeLength(currNode);
        activeLength -= edgeLength(currNode);
        activeNode = currNode;
        return 1;
    }
    return 0;
}

void extendSuffixTree(int pos)
{
    /*Extension Rule 1, this takes care of extending all leaves created so far in tree*/
    leafEnd = pos;

    /*Increment remainingSuffixCount indicating that a new suffix added to the list of suffixes yet to be
    added in tree*/
    remainingSuffixCount++;

    /*set lastNewNode to NULL while starting a new phase, indicating there is no internal node waiting for
    it's suffix link reset in current phase*/
    lastNewNode = NULL;

    // Add all suffixes (yet to be added) one by one in tree
    while (remainingSuffixCount > 0)
    {
        if (activeLength == 0)
            activeEdge = pos; // APCFALZ

        // There is no outgoing edge starting with
        // activeEdge from activeNode
        if (activeNode->children[text[activeEdge]] == NULL)
        {
            /*A new leaf edge is created in above line starting from  an existing node (the current activeNode), and
            if there is any internal node waiting for its suffix link get reset, point the suffix link from that last
            internal node to current activeNode. Then set lastNewNode to NULL indicating no more node waiting for 
            suffix link reset.*/
            activeNode->children[text[activeEdge]] = newNode(pos, &leafEnd);
            if (lastNewNode != NULL)
            {
                lastNewNode->suffixLink = activeNode;
                lastNewNode = NULL;
            }
        }
        // There is an outgoing edge starting with activeEdge from activeNode
        else
        {
            // Get the next node at the end of edge starting with activeEdge
            Node *next = activeNode->children[text[activeEdge]];
            if (walkDown(next))
            {
                // Start from next node (the new activeNode)
                continue;
            }
            /*Extension Rule 3 (current character being processed is already on the edge)*/
            if (text[next->start + activeLength] == text[pos])
            {
                // If a newly created node waiting for it's suffix link to be set, then set suffix link
                // of that waiting node to current active node
                if (lastNewNode != NULL && activeNode != root)
                {
                    lastNewNode->suffixLink = activeNode;
                    // no new internal node needs to be created in this extension
                    lastNewNode = NULL;
                }
                // APCFER3
                activeLength++;
                /*STOP all further processing in this phase and move on to next phase*/
                break;
            }

            /*We will be here when activePoint is in middle of the edge being traversed and current character
            being processed is not on the edge (we fall off the tree). In this case, we add a new internal node
            and a new leaf edge going out of that new node. This is Extension Rule 2, where a new leaf edge and a new
            internal node get created*/
            splitEnd = (int *)malloc(sizeof(int));
            if (!splitEnd)
            {
                fprintf(stderr, "Memory allocation failed for splitEnd\n");
                exit(EXIT_FAILURE);
            }

            // This gives the index of the last character in the substring that has been matched so far.
            // Essentially, it points to the character just before the mismatch.
            *splitEnd = next->start + activeLength - 1;
            // New internal node
            Node *split = newNode(next->start, splitEnd);
            activeNode->children[text[activeEdge]] = split;

            // New leaf coming out of new internal node
            split->children[text[pos]] = newNode(pos, &leafEnd);
            // update the start of the next node to be the character after the split
            next->start += activeLength; 
            split->children[text[next->start]] = next; // next is the original node that was split

            /*We got a new internal node here. If there is any internal node created in last extensions of same
              phase which is still waiting for it's suffix link reset, do it now.*/
            if (lastNewNode != NULL)
            {
                /*suffixLink of lastNewNode points to current newly created internal node*/
                lastNewNode->suffixLink = split;
            }

            /*Make the current newly created internal node waiting for it's suffix link reset (which is pointing to root
            at present). If we come across any other internal node (existing or newly created) in next extension of same
            phase, when a new leaf edge gets added (i.e. when Extension Rule 2 applies is any of the next extension
            of same phase) at that point, suffixLink of this node will point to that internal node.*/
            lastNewNode = split;
        }

        /* One suffix got added in tree(Rule 2), decrement the count of suffixes yet to be added.*/
        remainingSuffixCount--;
        if (activeNode == root && activeLength > 0) // APCFER2C1
        {
            activeLength--;
            activeEdge = pos - remainingSuffixCount + 1;
        }
        else if (activeNode != root) // APCFER2C2
        {
            activeNode = activeNode->suffixLink;
        }
    }
}

void print(int i, int j)
{
    int k;
    for (k = i; k <= j; k++)
        printf("%c", text[k]);
}

// Print the suffix tree as well along with setting suffix index
// So tree will be printed in DFS manner
// Each edge along with it's suffix index will be printed
void setSuffixIndexByDFS(Node *n, int labelHeight)
{
    if (n == NULL)
        return;

    if (n->start != -1) // A non-root node
    {
        // Print the label on edge from parent to current node
        // Uncomment below line to print the edge label
        // print(n->start, *(n->end));
    }
    // assume the current node is a leaf node
    int leaf = 1;
    int i;
    // iterate through all children of current node
    for (i = 0; i < MAX_CHAR; i++)
    {
        // if child is not NULL, set leaf to 0 as it has a child
        if (n->children[i] != NULL)
        {
            // Uncomment below lines to print the edge label
            // if the node was considered a leaf before discovering it has children,
            // the suffix index is printed before marking it as not a leaf(prefix of a suffix).
            // if (leaf == 1 && n->start != -1)
            // printf(" [%d]\n", n->suffixIndex);

            // Current node is not a leaf as it has outgoing    
            // edges from it.
            leaf = 0;
            setSuffixIndexByDFS(n->children[i], labelHeight +
                                                    edgeLength(n->children[i]));
        }
    }
    if (leaf == 1)
    {
        n->suffixIndex = size - labelHeight;
        // Uncomment below line to print suffix index
        // printf(" [%d]\n", n->suffixIndex);
    }
}

void freeSuffixTreeByPostOrder(Node *n)
{
    if (n == NULL)
        return;
    for (int i = 0; i < MAX_CHAR; i++)
    {
        if (n->children[i] != NULL)
        {
            freeSuffixTreeByPostOrder(n->children[i]);
            n->children[i] = NULL;
        }
    }

    if (n->suffixIndex == -1 && n->end != NULL && n->end != rootEnd)
    {
        free(n->end);
    }

    free(n);
}

/*Build the suffix tree and print the edge labels along with suffixIndex. suffixIndex for leaf edges will be >= 0 and
for non-leaf edges will be -1*/
void buildSuffixTree()
{
    size = strlen(text);
    int i;
    rootEnd = (int *)malloc(sizeof(int));
    *rootEnd = -1;

    /*Root is a special node with start and end indices as -1, as it has no parent from where an edge comes to root*/
    root = newNode(-1, rootEnd);

    activeNode = root; // First activeNode will be root
    for (i = 0; i < size; i++)
        extendSuffixTree(i);
    int labelHeight = 0;
    setSuffixIndexByDFS(root, labelHeight);
}

/*traverses an edge in the suffix tree and checks if the substring str matches the characters on that edge.
*str: the substring to search
*idx: the index of the first character of the substring to search
*start: the index of the first character of the edge
*end: the index of the last character of the edge
*/
int traverseEdge(char *str, int idx, int start, int end)
{
    int k = 0;
    // Traverse the edge with character by character matching
    for (k = start; k <= end && str[idx] != '\0'; k++, idx++)
    {
        if (text[k] != str[idx])
            return -1; // no match
    }
    if (str[idx] == '\0')
        return 1; // entire substring matches the edge
    return 0;     // more characters yet to match
}

/*DFS traversal of the suffix tree to count the leaf nodes returns the number of leaf nodes of the given node
the suffixIndex represents the position of the substring found in the text*/
int doTraversalToCountLeaf(Node *n)
{
    if (n == NULL)
        return 0;
    // if the node is a leaf node, count 1
    if (n->suffixIndex > -1)
    {
        // Uncomment below line to print found positions
        // printf("\nFound at position: %d", n->suffixIndex);
        return 1;
    }
    int count = 0;
    int i = 0;
    for (i = 0; i < MAX_CHAR; i++)
    {
        if (n->children[i] != NULL)
        {
            count += doTraversalToCountLeaf(n->children[i]);
        }
    }
    return count;
}

/*Wrapper for doTraversalToCountLeaf to count leaf nodes.*/
int countLeaf(Node *n)
{
    if (n == NULL)
        return 0;
    return doTraversalToCountLeaf(n);
}

/*The function to search a pattern in the suffix tree.
*n: the current node
*str: the pattern to search
*idx: the index of the first character of the pattern to search
*/
int doTraversal(Node *n, char *str, int idx)
{
    if (n == NULL)
    {
        return -1; // no match
    }
    int res = -1;
    // If node n is not root node, then traverse edge from node n's parent to node n.
    if (n->start != -1)
    {
        res = traverseEdge(str, idx, n->start, *(n->end));
        if (res == -1) // no match
            return -1;
        if (res == 1) // match
        {
            printf("\nsubstring count: %d", countLeaf(n));
            return 1; // match
        }
    }
    // if res == 0, idx will be updated by edgeLength(n) to move to the next edge
    // which means the current edge is completely matched and there are more characters to match
    // if n is root, edgeLength(n) = 0, so idx will be updated by 0

    idx = idx + edgeLength(n);
    // If there is an edge from node n going out
    // with current character str[idx], traverse that edge
    if (n->children[str[idx]] != NULL)
        return doTraversal(n->children[str[idx]], str, idx);
    else
        return -1; // no match
}

void checkForSubString(char *str)
{
    int res = doTraversal(root, str, 0);
    if (res == 1)
        printf("\nPattern <%s> is a Substring\n", str);
    else
        printf("\nPattern <%s> is NOT a Substring\n", str);
}

// Function to read the content of a file into a string using fgets
char *readFileWithFgets(const char *filename)
{
    // Open the file in read mode
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not open file %s for reading.\n", filename);
        exit(1); // Exit the program with an error code
    }

    // Move the file pointer to the end of the file to determine the file size
    fseek(file, 0, SEEK_END);
    long length = ftell(file); // Get the current position of the file pointer, which is the file size
    fseek(file, 0, SEEK_SET);  // Move the file pointer back to the beginning of the file

    // Allocate memory to store the file content, including a null terminator and space for the '$'
    char *content = malloc(length + 2); // +2 to accommodate the '$' and null terminator
    if (content == NULL)
    {
        fprintf(stderr, "Memory allocation failed for file content.\n");
        fclose(file); // Important to close the file before exiting
        exit(1);      // Exit the program with an error code
    }

    // Initialize the content string to be empty
    int content_length = 0; // Keep track of the length of content
    content[0] = '\0';

    // Initialize a buffer to read chunks of the file
    char buffer[1024];

    // Read the file content chunk by chunk using fgets
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        int buffer_length = strlen(buffer);
        if (content_length + buffer_length >= length + 1)
        { // Check for overflow possibility
            fprintf(stderr, "Buffer overflow prevented.\n");
            break;
        }
        memcpy(content + content_length, buffer, buffer_length);
        content_length += buffer_length;
    }

    // Append the '$' character at the end of the content
    content[content_length] = '$';
    content[content_length + 1] = '\0'; // Ensure the string is null-terminated

    // Close the file
    fclose(file);

    // Return the content string containing the entire file content plus the '$'
    return content;
}

void bruteForceSearch(char *text, char *pattern)
{
    int m = strlen(pattern);
    int n = strlen(text);
    int count = 0;

    for (int i = 0; i <= n - m; i++)
    {
        int j;
        for (j = 0; j < m; j++)
        {
            if (text[i + j] != pattern[j])
                break;
        }

        if (j == m)
        {
            // Uncomment below line to print found positions
            // printf("\nFound at position: %d", i);
            count++;
        }
    }

    printf("\nsubstring count: %d", count);
    if (count > 0)
        printf("\nPattern <%s> is a Substring\n", pattern);
    else
        printf("\nPattern <%s> is NOT a Substring\n", pattern);
}

// driver program to test above functions

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        // Read the file content
        char *fileTextInput = readFileWithFgets(argv[1]);
        size_t input_size = strlen(fileTextInput);
        printf("\nRead text from file: %s (size: %zu bytes)\n", argv[1], input_size);
        printf("\nInput size: %zu\n", input_size - 1);

        // Example pattern search input
        char *pattern = "atc";  // small pattern
        // char *pattern = "aaagtgctgg";    // medium pattern
        // char *pattern = "atccacccgccttggcctcctaaagtgctgggattacaggtgttagccaccacgtccagctgttaatttttatttaataagaatgacagagtgagggcc";  // large pattern

        printf("Pattern to search: <%s>\n", pattern);

        strcpy(text, fileTextInput);

        // Suffix Tree Implementation
        printf("\n--- Suffix Tree Implementation ---");

        // Reset memory usage tracking for suffix tree
        malloced_memory_usage = 0;
        size_t initial_memory_used_suffix_tree = malloced_memory_usage;

        // Suffix Tree Construction
        clock_t start = clock();
        buildSuffixTree();
        clock_t end = clock();
        double suffix_tree_build_time = ((double)(end - start)) / CLOCKS_PER_SEC;

        // Pattern search using suffix tree
        start = clock();
        checkForSubString(pattern);
        end = clock();
        double suffix_tree_search_time = ((double)(end - start)) / CLOCKS_PER_SEC;

        size_t memory_used_during_suffix_tree = malloced_memory_usage - initial_memory_used_suffix_tree;

        // Brute Force Implementation
        printf("\n--- Brute Force Implementation ---");

        // Reset memory usage tracking for brute force
        malloced_memory_usage = 0;
        size_t initial_memory_used_brute_force = malloced_memory_usage;

        // Brute Force Search
        start = clock();
        bruteForceSearch(text, pattern);
        end = clock();
        double brute_force_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        size_t memory_used_during_brute_force = malloced_memory_usage - initial_memory_used_brute_force;

        // Print Results
        printf("\n--- Comparison ---");
        printf("\nTime used for the building the suffix tree: %lf seconds", suffix_tree_build_time);
        printf("\nTime used for the pattern search (Suffix Tree): %lf seconds", suffix_tree_search_time);
        printf("\nMemory used for suffix tree implementation: %zu bytes\n", memory_used_during_suffix_tree);

        printf("\nTime used for the pattern search (Brute Force): %lf seconds", brute_force_time);
        printf("\nMemory used for brute force implementation: %zu bytes", memory_used_during_brute_force);

        // Free the dynamically allocated memory
        free(fileTextInput);
        freeSuffixTreeByPostOrder(root);
    }

    /* Uncomment the following simple test cases
    // Should uncomment the printf statments in setSuffixIndexByDFS to print the built suffix tree
    // Test case 1

    textInput = "aF34Uo6Gk1KTpApzPvfhqgmyPURtHqNdyJrlbdnEsFcczF7wQy0EwLZQZrFsvjFa0t0sMoNHgbhbAmiwCSS5h7UcRplRWfDZ3J6d$";
    strcpy(text, textInput);
    printf("\nPrint the Suffix Tree for text: %s\n", textInput);
    buildSuffixTree();
    pattern = "ab";
    printf("\nText: %s, Pattern to search: %s\n", textInput, pattern);
    checkForSubString(pattern);
    freeSuffixTreeByPostOrder(root);

    // Test case 2

    char *textInput = "ABCABXABCD$";
    strcpy(text, textInput);
    printf("\nPrint the Suffix Tree for text: %s\n", textInput);
    buildSuffixTree();
    char *pattern = "ABC";
    printf("\nText: %s, Pattern to search: %s\n", textInput, pattern);
    checkForSubString(pattern);
    freeSuffixTreeByPostOrder(root);

    // Test case 3
    char *textInput = "GEEKSFORGEEKS$";
    strcpy(text, textInput);
    printf("\nPrint the Suffix Tree for text: %s\n", textInput);
    buildSuffixTree();
    char *pattern = "GEEKS";
    printf("\nText: %s, Pattern to search: %s\n", textInput, pattern);
    checkForSubString(pattern);
    char *pattern = "GEEK1";
    printf("\nText: %s, Pattern to search: %s\n", textInput, pattern);
    checkForSubString(pattern);
    char *pattern = "FOR";
    printf("\nText: %s, Pattern to search: %s\n", textInput, pattern);
    checkForSubString(pattern);
    freeSuffixTreeByPostOrder(root);

    // Test case 4
    char *textInput = "AABAACAADAABAAABAA$";
    strcpy(text, textInput);
    printf("\nPrint the Suffix Tree for text: %s\n", textInput);
    buildSuffixTree();
    char *pattern = "AABA";
    printf("\nText: %s, Pattern to search: %s\n", textInput, pattern);
    checkForSubString(pattern);
    char *pattern = "AA";
    printf("\nText: %s, Pattern to search: %s\n", textInput, pattern);
    checkForSubString(pattern);
    char *pattern = "AAE";
    printf("\nText: %s, Pattern to search: %s\n", textInput, pattern);
    checkForSubString(pattern);
    freeSuffixTreeByPostOrder(root);

    // Test case 5
    char *textInput = "AAAAAAAAA$";
    strcpy(text, textInput);
    printf("\nPrint the Suffix Tree for text: %s\n", textInput);
    buildSuffixTree();
    char *pattern = "AAAA";
    printf("\nText: %s, Pattern to search: %s\n", textInput, pattern);
    checkForSubString(pattern);
    char *pattern = "AA";
    printf("\nText: %s, Pattern to search: %s\n", textInput, pattern);
    checkForSubString(pattern);
    char *pattern = "A";
    printf("\nText: %s, Pattern to search: %s\n", textInput, pattern);
    checkForSubString(pattern);
    char *pattern = "AB";
    printf("\nText: %s, Pattern to search: %s\n", textInput, pattern);
    checkForSubString(pattern);
    freeSuffixTreeByPostOrder(root);
    */

    return 0;
}
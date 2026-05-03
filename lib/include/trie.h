#ifndef TRIE
#define TRIE

/*
    For flexibility, we will use void* isEnd to denote the end of a string.
    isEnd will be void* notEndPtr when the current node is not the end of a 
    string, and will point to some data associated with the string when the
    current node is the end of a string. notEndPtr points to a uint8_t set
    to 1.
    Case sensitive, supports letters, numbers and underscores.
    We use (void*)-1 for errors in this library, which might not work on some embedded
    systems. Altrnate would be to have another reserved pointer like notEndPtr.
    All strings passed should be '\0' terminating.
*/

#define CHILDREN_NUM 63

typedef struct TrieNode{
    const void* isEnd;
    struct TrieNode* children[CHILDREN_NUM];
} TrieNode;

typedef struct{
    TrieNode* root;
    void* notEndPtr;
} Trie;

// Will create a trie
Trie* createTrie();

/*
    Will insert a string into the trie. isEnd of that node will point to data.
    If the string is already in the trie, will return a pointer to the current
    isEnd of that node, and trie will not be updated.
    Otherwise, in case of successful insertion, will return notEndPtr.
    If an invalid character is in the string, or in case of any other error,
    it will return (void*)-1.
*/
void* insertElementTrie(Trie* trie, const char* string, const void* data);

/*
    If string is found in trie, will return the data stored at that node.
    Otherwise, will return notEndPtr.
    If an invalid character is in the string, or in case of any other error,
    it will return (void*)-1.
*/
void* findElementTrie(const Trie* trie, const char* string);

/*
    returns a pointer to the deleted string's data.
    if string not in trie, will return notEndPtr.
    If an invalid character is in the string, or in case of any other error,
    it will return (void*)-1.
*/
void* deleteElementTrie(Trie* trie, const char* string);

/*
    To free up all data of trie.
*/
void killTrie(Trie* trie);

#endif

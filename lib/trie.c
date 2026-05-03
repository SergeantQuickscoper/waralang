#include <trie.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

TrieNode* initNode(void* notEndPtr){
    TrieNode* node = malloc(sizeof(TrieNode));
    node->isEnd = notEndPtr;
    for(size_t i = 0; i<CHILDREN_NUM; i++){
        *(node->children+i) = NULL;
    }
}

Trie* createTrie(){
    Trie* trie = malloc(sizeof(Trie));
    trie->notEndPtr = malloc(sizeof(uint8_t));
    trie->root = initNode(trie->notEndPtr);
};

// finds index of character in children array
static inline uint8_t getCharIdx(const char c){
    if(c>='a' && c<='z'){
        return c-'a';
    }
    else if(c>='A' && c<='Z'){
        return c-'A' + 26;
    }
    else if(c>='0' && c<='9'){
        return c-'0' + 52;
    }
    else if(c=='_'){
        return 62;
    }
    else{
        return UINT8_MAX;
    }
}

void* insert(Trie* trie, const char* string, const void* data){
    if(trie == NULL){
        fprintf(stderr, "trie error. can't insert on a NULL you idiot\n");
    }
    TrieNode* curNode = trie->root;
    for(size_t i = 0; i<strlen(string); i++){
        uint8_t id = getCharIdx(*(string+i));
        if(id>=CHILDREN_NUM){
            fprintf(stderr, "trie error. Invalid character ");
            fprintf(stderr, "%c in string.\n only ", *(string+i));
            fprintf(stderr, "letters, numbers and underscores accepted.\n");
            return (void*)-1;
        }
        if(curNode->children+id == NULL){
            *(curNode->children+id) = initNode(trie->notEndPtr);
        }
    }
}
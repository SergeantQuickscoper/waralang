#include <trie.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

TrieNode* initTrieNode(void* notEndPtr){
    TrieNode* node = malloc(sizeof(TrieNode));
    node->isEnd = notEndPtr;
    for(size_t i = 0; i<CHILDREN_NUM; i++){
        *(node->children+i) = NULL;
    }
    return node;
}

Trie* createTrie(){
    Trie* trie = malloc(sizeof(Trie));
    trie->notEndPtr = malloc(sizeof(uint8_t));
    trie->root = initTrieNode(trie->notEndPtr);
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

void* insertElementTrie(Trie* trie, const char* string, const void* data){
    if(trie == NULL){
        fprintf(stderr, "trie error. can't insert on a NULL trie you idiot\n");
        return (void*)-1;
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
        if(curNode->children[id] == NULL){
            curNode->children[i] = initTrieNode(trie->notEndPtr);
        }
        curNode = curNode->children[id];
    }
    if(curNode->isEnd!=trie->notEndPtr){
        return (void*)curNode->isEnd;
    }
    curNode->isEnd = data;
    return trie->notEndPtr;
}

void* findElementTrie(const Trie* trie, const char* string){
    if(trie == NULL){
        fprintf(stderr, "trie error. can't find in a NULL trie you idiot\n");
        return (void*)-1;
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
        if(curNode->children[id] == NULL){
            return trie->notEndPtr;
        }
        curNode = curNode->children[id];
    }
    return (void*)curNode->isEnd;
}

uint8_t isEmpty(TrieNode* node, void* notEndPtr){
    for(size_t i = 0; i<CHILDREN_NUM; i++){
        if(node->children[i]!=notEndPtr){
            return 0;
        }
    }
    return 1;
}

void* deleteRecursive(TrieNode* node, const char* string, size_t i, void* notEndPtr, void** res){
    if(node==NULL){
        *res = notEndPtr;
        return node;
    }

    if(string[i]=='\0'){
        *res = node->isEnd;
        node->isEnd = notEndPtr;
        if(isEmpty(node, notEndPtr)){
            free(node);
            node = NULL;
        }
        return node;
    }

    uint8_t childidx = getCharIdx(string[i]);
    node->children[childidx] = deleteRecursive(node->children[childidx], string, i+1, notEndPtr, res);
    if(isEmpty(node, notEndPtr) && node->isEnd == notEndPtr){
        free(node);
        node = NULL;
    }
    return node;
}

void* deleteElementTrie(Trie* trie, const char* string){
    if(trie == NULL){
        fprintf(stderr, "trie error. can't delete on a NULL trie you idiot\n");
        return (void*)-1;
    }
    void* res;
    deleteRecursive(trie->root, string, 0, trie->notEndPtr, &res);
    return res;
}

void killRecursive(TrieNode* node){
    if(node==NULL){
        return;
    }
    for(size_t i = 0; i<CHILDREN_NUM; i++){
        killRecursive(node->children[i]);
    }
    free(node);
}

void killTrie(Trie* trie){
    if(trie==NULL){
        fprintf(stderr, "trie error. can't kill a NULL trie you idiot\n");
        return;
    }
    killRecursive(trie->root);
    free(trie->notEndPtr);
    free(trie);
}

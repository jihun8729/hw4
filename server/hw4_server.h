#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 300
#define MAX_CLNT 256
#define ALPHABET_SIZE 53



int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    int isleaf;
    int frequency;
} TrieNode;

typedef struct{
	int frequency;
	char word[100];
}result;

typedef struct{
	TrieNode *root;
	int count;
	int sockfd;
	result search_result[30];
}client_info;


TrieNode *createNode();
void readFile(TrieNode *root);
void insert(TrieNode *root, char* str, int search_count);
void search_word(TrieNode *root, char *search, char *buf, int location, result *res, int *count);
int compare(const void *a, const void *b);
void *handle_clnt(void * arg);
void send_msg(char * msg, int len);
void error_handling(char * msg);

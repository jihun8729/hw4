#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <termios.h>

#define BUF_SIZE 300
#define NAME_SIZE 20
#define ALPHABET_SIZE 52
#define COLOR_RED	"\033[38;2;255;0;0m"
#define COLOR_RESET	"\033[0m"

void *send_msg(void * arg);
void *recv_msg(void * arg);
void error_handling(char * msg);


typedef struct{
	int frequency;
	char word[100];
}result;

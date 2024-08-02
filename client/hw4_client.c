#include "hw4_client.h"


	
int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread;
	void * thread_return;

	if (argc != 3) {
		printf("Usage : %s <IP> <port> \n", argv[0]);
		exit(1);
	 }
	
	sock = socket(PF_INET, SOCK_STREAM, 0);
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));
	  
	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");
	
	pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
	pthread_join(snd_thread, &thread_return);
	close(sock);  
	return 0;
}
	
void *send_msg(void * arg)   // send thread main
{
	struct termios saved;
	struct termios new;
    result re[10];
	int sock = *((int*)arg);
	int count;
	char ch;
	char word[100];
	
	int index=0;
	tcgetattr(STDIN_FILENO,&saved);
	new = saved;
	new.c_lflag &= ~ICANON;
	new.c_lflag &= ~ECHO;
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &new);
	printf("Search Word: ");
	
	while (1) 
	{
		int str_len = read(STDIN_FILENO,&ch,1);
		memset(re,0,sizeof(re));
		// if (ch =='q'||ch=='Q') 
		// {
		// 	tcsetattr(STDIN_FILENO, TCSANOW, &saved);
		// 	close(sock);
		// 	exit(0);
		// }
		if(ch == 127){
			if(index>0){
				index--;
				word[index]='\0';
				printf("\r");
				printf("\033[K");
				printf("Search Word: %s",word);				
			}
		}else{
			word[index]=ch;
			printf("%c",ch);
			index++;
			word[index] = '\0';
		}
		if(index==0) continue; 
		printf("\n");
		
		write(sock, word , strlen(word)+1);
		read(sock,&count,sizeof(int));
		
        for(int i=0; i<count&&i<10; i++){
            read(sock,&re[i],sizeof(result));
			char *start;
			char *text = re[i].word;
			while((start = strstr(text,word))!=NULL){ //search단어가 받아온 단어내에서 시작위치 찾기
				fwrite(text,sizeof(char),start-text,stdout);
				printf("%s",COLOR_RED);
				fwrite(word,sizeof(char),strlen(word),stdout); //검색하는 단어가 포함 된 부분
				printf("%s",COLOR_RESET);
				text = start + strlen(word); 
			}
            printf("%s %d\n",text,re[i].frequency);
        }
		//printf("%d, %d\n",count,index);
		printf ("\x1b[%dA", count); //출력한만큼 커서를 위로 올리고
		printf("\x1B[2;%dH",index+14); // search word: + 마지막으로 입력한 문자뒤로 커서 이동
		printf("\x1B[J"); //커서 뒤에 있는 내요들 다 지우기
		
	}
	tcsetattr(STDIN_FILENO, TCSANOW, &saved);
	return NULL;
}
	

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

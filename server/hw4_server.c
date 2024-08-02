#include "hw4_server.h"

int main(int argc, char *argv[]){
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	pthread_t t_id;
	TrieNode *root = createNode();
	readFile(root);
	
	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	

	pthread_mutex_init(&mutx, NULL);
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET; 
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));
	
	if (bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");
	
	while (1)
	{
		clnt_adr_sz = sizeof(clnt_adr);
		
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);
		client_info *cl = (client_info *)malloc(sizeof(client_info));
		cl->sockfd = clnt_sock;
		cl->root = root;

		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++] = clnt_sock;
		pthread_mutex_unlock(&mutx);
	
		pthread_create(&t_id, NULL, handle_clnt, (void*)cl);
		pthread_detach(t_id);
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
	}
	close(serv_sock);
	return 0;
}
	
void *handle_clnt(void * arg)
{
	client_info *cl = (client_info *)arg;
	int str_len = 0, i;
	
	char search[BUF_SIZE];
	char buf[BUF_SIZE];
	
	while ((str_len = read(cl->sockfd, search, sizeof(search))) != 0){
		cl->count=0;
		printf("%s\n",search);
		memset(buf,0,sizeof(buf));
		search_word(cl->root,search,buf,0,cl->search_result,&cl->count); //단어 검색
		qsort(cl->search_result,cl->count,sizeof(result),compare); // cl->count순으로 sorting
		write(cl->sockfd,&cl->count,sizeof(int)); //클라이언트에게  파일 개수 전송
		for(int i=0; i<cl->count && i<10; i++){ //count가 10보다 크면 10개까지만 전송
			send(cl->sockfd,&cl->search_result[i],sizeof(cl->search_result[i]),0);
	 		//printf("%s %d\n",cl->search_result[i].word,cl->search_result[i].frequency);
		}
	}
		
	
	pthread_mutex_lock(&mutx);
	for (i = 0; i < clnt_cnt; i++)   // remove disconnected client
	{
		if (cl->sockfd == clnt_socks[i])
		{
			while (i++ < clnt_cnt-1)
				clnt_socks[i] = clnt_socks[i+1];
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx);
	close(cl->sockfd);
	
	return NULL;
}

void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

TrieNode *createNode() { // 노드 생성
    TrieNode *node = (TrieNode *)malloc(sizeof(TrieNode));
    node->isleaf = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) { 
        node->children[i] = NULL;
    }
    return node;
}

void insert(TrieNode *root, char* str, int search_count){
	TrieNode *curr = root;
	int index;
	
	while(*str){

		// 알파벳에 맞게 index 설정
		if(*str == ' '){
			index = 52;
		}else if(*str>='a'&&*str<='z'){ //소문자
			index = *str - 'a';
		}else if(*str>='A'&&*str<='Z'){ //대문자
			index = *str -'A' + 26;
		}

		if(curr->children[index] == NULL){ //노드가 NULL이면 생성
			curr->children[index] = createNode();
		}
		curr = curr->children[index];
		str++; //다음 문자로
	}
	curr->isleaf=1;
	curr->frequency = search_count;
}

void readFile(TrieNode *root){
	FILE *fp = fopen("test.txt","rb");
	char line[BUF_SIZE];
	char word[BUF_SIZE];
	int search_count=0;
	while (fgets(line, sizeof(line), fp)) { //파일에서 한줄씩 읽기
        char *check = strrchr(line, ' '); // 마지막 공백을 찾기
        if (check) {
            *check = '\0';  
        	check++;
            search_count = atoi(check);
            strcpy(word, line);
            insert(root, word, search_count);
        }
    }
	fclose(fp);
}

void search_word(TrieNode *root, char *search, char *buf, int location, result *res, int *count){
	
	if(!root) return;
	
	if(root->isleaf==1){	// 단어인 경우
		if(strstr(buf,search)){ //검색단어가 일부라도 포함되었다면
			buf[location] = '\0';
			strcpy(res[*count].word,buf);
			res[*count].frequency = root->frequency;
			(*count)++;		
		}
		
	}

	for(int i=0; i<ALPHABET_SIZE; i++){
		if(root->children[i]){
			
			if(i== 52){
				buf[location] = ' ';
			}else if(i<26){
				buf[location] = 'a' + i;
			}else{
				buf[location] = 'A' + i - 26;
			}
			//printf("%s\n",buf);
			search_word(root->children[i],search,buf,location+1, res, count);
			
		}
		
	}
	buf[location] = '\0';
}

int compare(const void *a, const void *b){
	return(((result*)b)->frequency-((result*)a)->frequency);
}

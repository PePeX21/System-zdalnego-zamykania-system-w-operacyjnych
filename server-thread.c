#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <pthread.h>


#define MAX_CONNECTIONS  10

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int connections_cfd[MAX_CONNECTIONS];
char connections_caddr[MAX_CONNECTIONS][15];
int counter = 0;

// deskryptor oraz ip klijenta
struct cln {
	int cfd;
	struct sockaddr_in caddr;
};

// wlasny string compare
int comparer(char* str1, char* str2){
	
	for(int i = 0; i < strlen(str2); i++){
		if(str1[i] != str2[i]){
			return 0;
		}
	}
	return 1; 
}

// nowy watek
void* cthread(void* arg) {
    struct cln* c = (struct cln*)arg;
    char buffer[100];
    char* message;
    int j, n;
    
    printf("\nnew connection: %s na dyskryptorze: %d\n", inet_ntoa((struct in_addr)c->caddr.sin_addr), c->cfd);
    
    pthread_mutex_lock(&mutex); // sekcja krytyczna  
    strcpy(connections_caddr[counter], inet_ntoa((struct in_addr)c->caddr.sin_addr));
    connections_cfd[counter] = c->cfd;
    counter ++;  
    pthread_mutex_unlock(&mutex); //

    while(1){
 
	bzero(buffer,100);
	if( read(c->cfd, &buffer, 100) > 0){	
	
	    	message = (char*) malloc((strlen(buffer)-2)*sizeof(char));
    		strncpy(message, buffer, (strlen(buffer)-2));
	
		if (comparer(message, "request_for_connection") == 1){
			char msg_connections_caddr[25];
			int size_of_msg;
			
			printf("%s \t\t\t\t\t|1|\n", message);

        		pthread_mutex_lock(&mutex); // sekcja krytyczna
			j = 21;
			n = 0;
			while (j > 0){
				n = write(c->cfd,"connection_successed\n", 21);
				j = j - n;
			}
		
			for(int i = 0; i < counter; i++){ // aktualizowanie polaczen dla klijenta
				for(int j = 0; j < counter; j++){
					strcpy(msg_connections_caddr, "new_caddr_");
					strcat(msg_connections_caddr, connections_caddr[j]);
					size_of_msg = strlen(msg_connections_caddr) + 1; //bo w write dosylam znak nowej lini
				
					n = 0;
					while (size_of_msg > 0){
						n = write(connections_cfd[i], strcat(msg_connections_caddr, "\n"),size_of_msg);
						size_of_msg = size_of_msg - n;
					}
				}
			}
			pthread_mutex_unlock(&mutex); //
		}
		else if (comparer(message, "kill") == 1) {
			char msg_connections_caddr[28];
			int size_of_msg;
	
		
			printf("successful authorized with password: %s \t\t|2|\n", message);
			
			pthread_mutex_lock(&mutex); // sekcja krytyczna
			j = 21;
			n = 0;
			while (j > 0){
				n = write(c->cfd,"authorized_successed\n",21);
				j = j - n;
			}
			
			bzero(buffer, 100);
			read(c->cfd, &buffer, 100); 
				
			message = (char*) malloc((strlen(buffer)-2)*sizeof(char));
	    		strncpy(message, buffer, (strlen(buffer)-2));
    		
	    		printf("shutdown computer with ip: %s Accept \t|2|\n", message);
    		
	    		for(int i = 0; i < counter; i++){ // aktualizowanie polaczen dla klijenta
				for(int j = 0; j < counter; j++){
					strcpy(msg_connections_caddr, "remove_caddr_");
					strcat(msg_connections_caddr, message);
					size_of_msg = strlen(msg_connections_caddr) + 1; //bo w write dosylam znak nowej lini
				
					n = 0;
					while (size_of_msg > 0){
						n = write(connections_cfd[i], strcat(msg_connections_caddr, "\n"),size_of_msg);
						size_of_msg = size_of_msg - n;
					}
				}
			}
	    		for(int i = 0; i < counter; i++){ // aktualizowanie tablicy polaczen dla serwera
	    			if (comparer(message, connections_caddr[i]) == 1) {
	    				j = 9;
					n = 0;
					while (j > 0){	// wysylanie wiadomosci o zamknieciu systemu
						n = write(connections_cfd[i], "shutdown\n",9);
						j = j - n;
					}
	    				close(connections_cfd[i]);
	    				if ( i == counter - 1){ 
	    					counter--; 						
	    				}
	    				else{
	    					strcpy(connections_caddr[i], connections_caddr[counter - 1]);
	   					connections_cfd[i] = connections_cfd[counter - 1] ;
	   					counter--;
	    				}
	    			}
			}    		    		
			pthread_mutex_unlock(&mutex); //
		}
		else if (comparer(message, "exit") == 1){
			char msg_connections_caddr[28];
			int size_of_msg;
	
			printf("loged out computer with ip: %s \t\t|3|\n", inet_ntoa((struct in_addr)c->caddr.sin_addr));
		
			pthread_mutex_lock(&mutex); // sekcja krytyczna
			for(int i = 0; i < counter; i++){ // aktualizowanie polaczen dla klijenta
				for(int j = 0; j < counter; j++){
					strcpy(msg_connections_caddr, "remove_caddr_");
					strcat(msg_connections_caddr, inet_ntoa((struct in_addr)c->caddr.sin_addr));
					size_of_msg = strlen(msg_connections_caddr) + 1; //bo w write dosylam znak nowej lini
				
					n = 0;
					while (size_of_msg > 0){
						n = write(connections_cfd[i], strcat(msg_connections_caddr, "\n"),size_of_msg);
						size_of_msg = size_of_msg - n;
					}
				}
			}
    			for(int i = 0; i < counter; i++){ // aktualizowanie tablicy polaczen dla serwera
    				if (c->cfd == connections_cfd[i]) {
    					if ( i == counter - 1){ 
    						counter--; 					
    					}
    					else{
    						strcpy(connections_caddr[i], connections_caddr[counter - 1]);
   						connections_cfd[i] = connections_cfd[counter - 1] ;
   						counter--;
    					}
    				}
			}	
			close(c->cfd);
    			free(c);
    			pthread_mutex_unlock(&mutex);
    			return EXIT_SUCCESS; //	
		} 
		else{
	
			printf("unsuccessful authorized with password:  %s \t\t|4|\n", message);
		
			pthread_mutex_lock(&mutex); // sekcja krytycnza
			j = 20;
			n = 0;
			while (j > 0){
				n = write(c->cfd,"authorized_denialed\n",20);
				j = j - n;
			}

			bzero(buffer, 100);
			read(c->cfd, &buffer, 100);
				
			message = (char*) malloc((strlen(buffer)-2)*sizeof(char));
	    		strncpy(message, buffer, (strlen(buffer)-2));
	
			printf("shutdown computer with ip: %s Refused \t|4|\n", message);
			pthread_mutex_unlock(&mutex); //
		}
	    }
	}
}

// MAIN
int main(int argc, char *argv[])
{
	pthread_t tid;
	socklen_t slt;
	int sfd = socket(PF_INET,SOCK_STREAM,0);
	struct sockaddr_in saddr;

	int on = 1;
	setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,(char*)&on, sizeof(on));

	saddr.sin_family=PF_INET;
	saddr.sin_port=htons(1234);
	saddr.sin_addr.s_addr=INADDR_ANY;

	bind(sfd,(struct sockaddr*)&saddr, sizeof(saddr));

	listen(sfd,MAX_CONNECTIONS);


	while(1){
		struct cln* c = malloc(sizeof(struct cln));
		slt = sizeof(c->caddr);
		c->cfd = accept(sfd, (struct sockaddr*)&c->caddr, &slt);
		pthread_create(&tid, NULL, cthread, c);
		pthread_detach(tid);
	}
	close(sfd);
	return EXIT_SUCCESS;
}


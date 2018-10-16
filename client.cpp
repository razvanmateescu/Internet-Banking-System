#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <cstdlib>
using namespace std;
#define BUFLEN 256

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    fd_set read_fds;
    fd_set tmp_fds;
    int fdmax, j;
    char buffer[BUFLEN];
    string log;
    string log2;
    int card,x;
    int pin;
    char sir[100];
    char filename[100];
    int ok[1000000];
    int nr=0;
    char res[BUFLEN];
    bool verifica=false;
    if (argc < 3) {
       fprintf(stderr,"Usage %s server_address server_port\n", argv[0]);
       exit(0);
    }
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);
	  sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    FD_SET(0, &read_fds);
    FD_SET(sockfd, &read_fds);
    fdmax = sockfd;
    if (connect(sockfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
int pid = getpid();
sprintf(filename,"%s%d%s","client",pid,".log");
ofstream fout(filename);

    while(1){
  		//citesc de la tastatura
      tmp_fds = read_fds;
      select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
      for (j = 0; j <= fdmax; j++) {
        if (FD_ISSET(j, &tmp_fds)) {
          if (j == 0) {
            memset(buffer, 0, BUFLEN);
            fgets(buffer, BUFLEN-1, stdin);
	    string str(buffer);
	    istringstream ss(str);
	    ss>>log;
	    ss>>card;
	    ss>>pin;
	    ss.clear();
	    strcpy(sir,log.c_str());
	    if(log=="login")
	    {
            printf("Am citit de la tastatura\n");
            n = send(sockfd, buffer, strlen(buffer), 0);
		}
	  if(log=="logout")                                                       // parsez sirul citit d ela tastatura si trimit la server mesajul corespunzator in functie de comanda
	{  strcpy(res,log.c_str());
	   cout<<res<<" ";
	   verifica=false;
	    printf("Am citit de la tastatura\n");
            n = send(sockfd, res, strlen(res), 0);
	}
	  if(log=="listsold")
	  {
	    strcpy(res,log.c_str());
	    cout<<res<<" ";
	    printf("Am citit de la tastatura\n");
            n = send(sockfd, res, strlen(res), 0);
	}
	if(log.length()==1)
	{
		strcpy(res,log.c_str());
		cout<<log<<" ";
		printf("Am citit de la tastatura\n");
		n = send(sockfd, res, strlen(res), 0);
	}
	if(log=="transfer")
	{
		printf("Am citit de la tastatura\n");
            n = send(sockfd, buffer, strlen(buffer), 0);
	}
	if(log=="quit")
	{
		strcpy(res,log.c_str());
		printf("Am citit de la tastatura\n");
		n = send(sockfd, res, strlen(res), 0);                          // ca sa ies din client cand primesc comanda quit trimit mesaj serverului si ies cu exit.
		exit(0);
	}
	}
          else if (j == sockfd) {
	    string strprimire(buffer);
            memset(buffer, 0, BUFLEN);
            n = recv(sockfd, buffer, sizeof(buffer), 0);                             // cand primesc de la server mesajul scriu in fisier mesajul trimis si dupa cel primit
            if (n == 0)
              return 1;
            else {
              printf("[CLIENT] am citit de la server\n");
              printf("Mesajul %s\n", buffer);
		string primeste(buffer);
		istringstream iss(primeste);
		iss>>log2;
		iss>>log2;
		fout<<strprimire<<buffer<<endl<<endl;
            }
          }
        }
      }
    }
    fout.close();
    close(sockfd);
    return 0;
}

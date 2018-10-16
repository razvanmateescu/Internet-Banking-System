#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdlib>
using namespace std;
#define MAX_CLIENTS	10
#define BUFLEN 256

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen;
     char buffer[BUFLEN];
     char bufferext[BUFLEN];
     char res[BUFLEN];
     struct sockaddr_in serv_addr, cli_addr;
     int n, i, j,m;
     fd_set read_fds;	//multimea de citire folosita in select()
     fd_set tmp_fds;	//multime folosita temporar 
     int fdmax;		//valoare maxima file descriptor din multimea read_fds
     string numec;
     string prenumec;
     string parolac;
     string log;
     int cardc,pinc;
     double soldc;
     vector<string> nume;
     vector<string> prenume;
     vector<string> parola;
     vector<int> card;
     vector<int> pin;
     vector<double> sold;
     int blocat[1000000];
     int ok[1000000];
	int nrpin[MAX_CLIENTS];
     char sir[100];
     bool conect=false;
     int conectat[MAX_CLIENTS];
     int logat[MAX_CLIENTS];
     int cardprimit,pinprimit,cardprimittr,cardprimit2;
     int cardprimitvec[MAX_CLIENTS];
     double sumacitita;
     if (argc < 2) {
         fprintf(stderr,"Usage : %s port\n", argv[0]);
         exit(1);
     }

    ifstream f("users_data_file");
     f>>m;
	for (int i=0;i<m;i++)
	{
		f>>numec>>prenumec>>cardc>>pinc>>parolac>>soldc;
		nume.push_back(numec);
		prenume.push_back(prenumec);
		card.push_back(cardc);
		pin.push_back(pinc);
		parola.push_back(parolac);
		sold.push_back(soldc);
	}

     //golim multimea de descriptori de citire (read_fds) si multimea tmp_fds 
     FD_ZERO(&read_fds);
     FD_ZERO(&tmp_fds);
     
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     
     portno = atoi(argv[1]);

     memset((char *) &serv_addr, 0, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;	// foloseste adresa IP a masinii
     serv_addr.sin_port = htons(portno);
     
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0) 
              error("ERROR on binding");
     
     listen(sockfd, MAX_CLIENTS);

     //adaugam noul file descriptor (socketul pe care se asculta conexiuni) in multimea read_fds
     FD_SET(sockfd, &read_fds);                                                          
     FD_SET(0, &read_fds);                                                    // adaug socketul pentru stdin
     fdmax = sockfd;

     // main loop
	while (1) {
	
		tmp_fds = read_fds; 
		if (select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) == -1) 
			error("ERROR in select");
	
		for(i = 0; i <= fdmax; i++) {
		       conect=false;
			if (FD_ISSET(i, &tmp_fds)) {
				if(i==0)
				{ 
				memset(bufferext, 0, BUFLEN);
           			 fgets(bufferext, BUFLEN-1, stdin);                       //daca citesc de la tastatura quit inchid serverul
				if(bufferext=="quit")
				{
				exit(0);
				}
				}
				if (i == sockfd) {
					// a venit ceva pe socketul inactiv(cel cu listen) = o noua conexiune
					// actiunea serverului: accept()
					clilen = sizeof(cli_addr);
					if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1) {
						error("ERROR in accept");
					} 
					else {
						//adaug noul socket intors de accept() la multimea descriptorilor de citire
						FD_SET(newsockfd, &read_fds);
						if (newsockfd > fdmax) { 
							fdmax = newsockfd;
						}
					}
					printf("Noua conexiune de la %s, port %d, socket_client %d\n ", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), newsockfd);
				}
					
				else {
					// am primit date pe unul din socketii cu care vorbesc cu clientii
					//actiunea serverului: recv()
					memset(buffer, 0, BUFLEN);
					if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0) {
						if (n == 0) {
							//conexiunea s-a inchis
							printf("selectserver: socket %d hung up\n", i);
						} else {
							error("ERROR in recv");
						}
						close(i); 
						FD_CLR(i, &read_fds); // scoatem din multimea de citire socketul pe care 
					} 
					
					else { //recv intoarce >0
						printf ("Am primit de la clientul de pe socketul %d, mesajul: %s\n", i, buffer);
            printf("[SERVER] trimit mesajul inapoi\n");
	    string str(buffer);
	    istringstream iss(str);                   //parsez sirul primit impartit in cate 1 cuvant
	    iss>>log;
		cout<<log<<" ";
	    strcpy(sir,log.c_str());
	    if (log == "login")                        //daca am primit comanda login verific pe rand daca este conectat la socket vreun client daca nu conectez altfel trimit msg de eroare corespunzatoare
	{
	    iss>>cardprimit;
	    iss>>pinprimit;
	    cardprimitvec[i]=cardprimit;
	    if (find(card.begin(),card.end(),cardprimit) != card.end() && find(pin.begin(),pin.end(),pinprimit) != pin.end())
	    {
		if (ok[cardprimit]==1)
	    {  strcpy(res,"IBANK> -2 : Sesiune deja deschisa");
	       int x = atoi(buffer);
           	 printf("X-Ul = %d\n", x);
           	 send(i, res, strlen(res), 0);
		}
		else if(conectat[i]==1)
		 {  strcpy(res,"IBANK> -2 : Sesiune deja deschisa");
	       int x = atoi(buffer);
           	 printf("X-Ul = %d\n", x);
           	 send(i, res, strlen(res), 0);
		}
		else if(blocat[cardprimit]!=1) {    
		conectat[i]=1;
		ok[cardprimit]=1;
		logat[i]=1;
		int pos=find(card.begin(),card.end(),cardprimit) - card.begin();
		string a=nume.at(pos);
		string b=prenume.at(pos);
		strcpy(res,"IBANK>");
		strcat(res," ");
		strcat(res,"Welcome");
		strcat(res," ");
		strcat(res,a.c_str());
		strcat(res," ");
		strcat(res,b.c_str());
                int x = atoi(buffer);
                printf("X-Ul = %d\n", x);
                send(i, res, strlen(res), 0);
		}
		else {
		strcpy(res,"IBANK> -5 : Card blocat");                             //dupa 3 incercari de pin esuate intr o sesiune blochez cardul altfel la logout resetez contuarul la 0
		int x = atoi(buffer);
           	 printf("X-Ul = %d\n", x);
           	 send(i, res, strlen(res), 0); 
		   }
		}
	    else if (find(card.begin(),card.end(),cardprimit) == card.end())
		{ strcpy(res,"IBANK> -4 : Numar card inexistent");
		int x = atoi(buffer);
           	 printf("X-Ul = %d\n", x);
           	 send(i, res, strlen(res), 0);
		}
	    else if (find(pin.begin(),pin.end(),pinprimit) == pin.end() && nrpin[i]<2)
		{ nrpin[i]++;
		  strcpy(res,"IBANK> -3 : Pin gresit"); 
		int x = atoi(buffer);
           	 printf("X-Ul = %d\n", x);
           	 send(i, res, strlen(res), 0); 
               }
		 else if (find(pin.begin(),pin.end(),pinprimit) == pin.end() && nrpin[i]==2) {
		strcpy(res,"IBANK> -5 : Card blocat");
		blocat[cardprimit]=1;
		int x = atoi(buffer);
           	 printf("X-Ul = %d\n", x);
           	 send(i, res, strlen(res), 0); 
               }
		}
		 if (log == "logout")                                     // daca am primit logout si nu am client autentificat trimit mesaj corespuncator daca nu ies si resetez clientul
		{
		     if (conectat[i]!=1)
			{strcpy(res,"-1: Clientul nu este autentificat");
			int x = atoi(buffer);
           	 printf("X-Ul = %d\n", x);
           	 send(i, res, strlen(res), 0); }
			else
			{
			cardprimit2=cardprimitvec[i];
			ok[cardprimit2]=0;
			if(nrpin[i]<2)
			nrpin[i]=0;
			strcpy(res,"IBANK: Clientul a fost deconectat");
			int x = atoi(buffer);
           		printf("X-Ul = %d\n", x);
           	 	send(i, res, strlen(res), 0);
			conectat[i]=0;	
		}
		}
		if(log == "listsold" && conectat[i]==1)                  //daca am un client conectat ii vizualizez soldul curent
		{        cardprimit2=cardprimitvec[i];
			 int pozitie=find(card.begin(),card.end(),cardprimit2) - card.begin();
			double bani=sold.at(pozitie);
			strcpy(res,"IBANK>");
			char banis[100];
			sprintf(banis, "%.2f", bani);
			strcat(res," ");
			strcat(res,banis);
			int x = atoi(buffer);
           		printf("X-Ul = %d\n", x);
			send(i, res, strlen(res), 0);
		}
		else if(log == "listsold" && conectat[i]!=1)
		{ strcpy(res,"-1: Clientul nu este autentificat");
			int x = atoi(buffer);
           	 printf("X-Ul = %d\n", x);
           	 send(i, res, strlen(res), 0);
		}	
		if(log== "transfer" && conectat[i]==1 )           //daca primesc comanda de transfer si aceasta este acceptata scad valoarea din sold si o adaug la clientul destinatar           
		{ iss>>cardprimittr;				// daca nu am fonduri suficiente sau nu e cardul corespunzator sau nu accept comanda afisez msg corespunzator
		  iss>>sumacitita;
		  char transfer[100];
		  if (find(card.begin(),card.end(),cardprimittr) !=card.end())
		{
		  cardprimit2=cardprimitvec[i];
		  int pozitietransfer=find(card.begin(),card.end(),cardprimittr) - card.begin();
		  int celcetransfera=find(card.begin(),card.end(),cardprimit2) - card.begin();
		  string numetransfer=nume.at(pozitietransfer);
		  string prenumetransfer=prenume.at(pozitietransfer);
		  double soldavut=sold.at(celcetransfera);
		  if(soldavut>=sumacitita)
		   {
		  sprintf(transfer,"%.15g",sumacitita);
		        strcpy(res,"IBANK> Transfer");
			strcat(res," ");
			strcat(res,transfer);
			strcat(res," ");
			strcat(res,"catre");
			strcat(res," ");
			strcat(res,numetransfer.c_str());
			strcat(res," ");
			strcat(res,prenumetransfer.c_str());
			strcat(res,"? [y/n]");
			int x = atoi(buffer);
           		printf("X-Ul = %d\n", x);                         
           	 	send(i, res, strlen(res), 0);
		}
		else {
		strcpy(res,"IBANK> -8 : Fonduri insuficiente");
		int x = atoi(buffer);
           	 printf("X-Ul = %d\n", x);
           	 send(i, res, strlen(res), 0);
		}
		}
		else {
		 strcpy(res,"IBANK> -4 : Numar card inexistent");
		 int x = atoi(buffer);
           	 printf("X-Ul = %d\n", x);
           	 send(i, res, strlen(res), 0);
		}
		}
		else if (log== "transfer" && conectat[i]!=1 )
		   {
			strcpy(res,"-1: Clientul nu este autentificat");
			int x = atoi(buffer);
           	        printf("X-Ul = %d\n", x);
           	        send(i, res, strlen(res), 0);
		}		
		if(log.length()==1 && log =="y")                                                
		{ cardprimit2=cardprimitvec[i];
		  int scaderetransfer=find(card.begin(),card.end(),cardprimit2) - card.begin();
		  int adunaretransfer=find(card.begin(),card.end(),cardprimittr) - card.begin();
		  double soldinitial=sold.at(scaderetransfer);
		  double soldramas=soldinitial-sumacitita;
		  char transferat[100];
		  sold.at(scaderetransfer)=soldramas;
		  double soldinitialdest=sold.at(adunaretransfer);
		  double soldmarit=soldinitialdest+sumacitita;
		  sprintf(transferat,"%.15g",soldramas);
		  sold.at(adunaretransfer)=soldmarit;
		  strcpy(res,"IBANK> Transfer realizat cu succes");
		  int x = atoi(buffer);
           	  printf("X-Ul = %d\n", x);
           	  send(i, res, strlen(res), 0);
		}
		else if(log.length()==1 && log !="y")
		{
		  strcpy(res,"IBANK> -9 : Operatie anulata");
		  int x = atoi(buffer);
           	  printf("X-Ul = %d\n", x);
           	  send(i, res, strlen(res), 0);
		}
		
		
		 } 
				
		} 
			}
		}
     }


     close(sockfd);
   
     return 0; 
}

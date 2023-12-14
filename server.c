/*#include <stdio.h>
          #include <sys/socket.h>
          #include <arpa/inet.h>
          #include <stdlib.h>
          #include <string.h>
          #include <unistd.h>
          #include <netinet/in.h>




int init(){
    int socket_id = socket(AF_INET,SOCK_STREAM,0);
    if(socket_id < 0){
      printf("socket error\n");
      return -1;
    }
      else{
        printf("socket created\n");
        return socket_id;
      }
    }


int main(int argc, char *argv[]){
int socket_id = init();
int socket_id2 = init();
printf("%d\n", socket_id);
printf("%d\n", socket_id2);
return 0;

}
*/
//version tcp
/* The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#define  AJOUT 0
#define RETRAIT 1
#define SOLDE 2
#define DERNIERES_OPERATION 3


typedef struct Date{
   int jour;
   int mois;
   int annee;
}date_t;



typedef struct{
   int montant;
   int type_operation;
   date_t date_operation;
}operation_t;



typedef struct{
   int solde;
   int id_compte;
   operation_t dernieres_operations[10];

}compte_t;

typedef struct {
   int identifiant;
char password[256];
compte_t compte[2];


}client_t;



typedef struct{
   client_t clients[5];
   compte_t comptes[10];
} banque_t;
compte_t tab_compte[10];

client_t tab_client[5];



int find_client(int id){
   for(int i = 0; i<5; i++){
      if(tab_client[i].identifiant == id){
         return true; //1 si le client existe
      }
   }
   return false; //0 si le client n'existe pas
}

int correctpassword(client_t client, char *password){
   if(strcmp(client.password, password) == 0){
      return 1; //1 si le mot de passe est correct
   }
   return 0; //0 si le mot de passe est incorrect
}

int find_compte(client_t client, int id){
   for(int i = 0; i<2; i++){
      if(client.compte[i].id_compte == id){
         return 1; //1 si le compte est associé au client
      }
   }
   return 0; //0 si le compte n'est pas associé au client
}
          
void initializeBanque(banque_t *banque) {
    for (int i = 0; i < 5; i++) {
        banque->clients[i].identifiant = i + 1;
         //printf("banque->clients[%d].identifiant = %d\n", i, banque->clients[i].identifiant);
        sprintf(banque->clients[i].password, "password%d", i + 1);
        //printf("banque->clients[%d].password = %s\n", i, banque->clients[i].password);//2 comptes par client
         //printf("banque->clients[%d].id_compte[0] = %d\n", i, banque->clients[i].compte->id_compte);
         //printf("banque->clients[%d].id_compte[1] = %d\n", i, banque->clients[i].compte->id_compte);
         



        

        for (int j = 0; j < 2; j++) {
            banque->clients[i].compte[j].solde = (i + 1) * 1000 + j * 500;
            banque->clients[i].compte[j].id_compte = i * 2 + j + 1;
            //printf("banque->clients[%d].compte[%d].solde = %d\n", i, j, banque->clients[i].compte[j].solde);

            // Initialize last 10 operations for each account
            for (int k = 0; k < 10; k++) {
                banque->clients[i].compte[j].dernieres_operations[k].montant = (k + 1) * 100;
                banque->clients[i].compte[j].dernieres_operations[k].type_operation = AJOUT;
                banque->clients[i].compte[j].dernieres_operations[k].date_operation.jour = k;
                banque->clients[i].compte[j].dernieres_operations[k].date_operation.mois = k;
                banque->clients[i].compte[j].dernieres_operations[k].date_operation.annee = 2023;
                //printf("banque->clients[%d].compte[%d].dernieres_operations[%d].montant = %d\n", i, j, k, banque->clients[i].compte[j].dernieres_operations[k].montant);
            }
        }
    }

}




int main(int argc, char *argv[])
{
     banque_t mabanque;
       initializeBanque(&mabanque);

     int sockfd, newsockfd, portno;
     socklen_t clilen;         
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
    


     // create a socket
     // socket(int domain, int type, int protocol)
     sockfd =  socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        perror("ERROR opening socket");

     setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
     // clear address structure
     bzero((char *) &serv_addr, sizeof(serv_addr));

     portno = atoi(argv[1]);

     /* setup the host_addr structure for use in bind call */
     // server byte order
     serv_addr.sin_family = AF_INET;  

     // automatically be filled with current host's IP address
     serv_addr.sin_addr.s_addr = INADDR_ANY;  

     // convert short integer value for port must be converted into network byte order
     serv_addr.sin_port = htons(portno);

     // bind(int fd, struct sockaddr *local_addr, socklen_t addr_length)
     // bind() passes file descriptor, the address structure, 
     // and the length of the address structure
     // This bind() call will bind  the socket to the current IP address on port, portno
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              perror("ERROR on binding");

     // This listen() call tells the socket to listen to the incoming connections.
     // The listen() function places all incoming connection into a backlog queue
     // until accept() call accepts the connection.
     // Here, we set the maximum size for the backlog queue to 5.
     listen(sockfd,5);
     while(1){

     // The accept() call actually accepts an incoming connection
     clilen = sizeof(cli_addr);

     // This accept() function will write the connecting client's address info 
     // into the the address structure and the size of that structure is clilen.
     // The accept() returns a new socket file descriptor for the accepted connection.
     // So, the original socket file descriptor can continue to be used 
     // for accepting new connections while the new socker file descriptor is used for
     // communicating with the connected client.
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0) 
          perror("ERROR on accept");

     printf("server: got connection from %s port %d\n",
            inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));


     // This send() function sends the 13 bytes of the string to the new socket
       send(newsockfd, "Veuillez entrer votre Identifiant client!\n", 256, 0);

     n = read(newsockfd, buffer, sizeof(buffer) - 1);
     char buffer_sans_espace[256];
if (n < 0) {
    perror("ERROR reading from socket");
} else if (n == 0) {
    printf("Connection closed by server\n");
    break;
} else {
   printf("%d",atoi(buffer));
   
    printf("Client says: %s\n", buffer);

    sscanf(buffer, "%s", buffer_sans_espace);

    printf("client says without space %s", buffer_sans_espace); 

    printf("valeur %d", atoi(buffer_sans_espace));
    
    

}

       
       if(find_client(atoi(buffer)) == true){
       

       printf("Voici l'identifiant  : %d\n",atoi(buffer));

      send(newsockfd, "identifiant bien reçu, entrez maintenant votre mot de passe\n", 256,0);
       }
       else{
         printf("client introuvable %d", atoi(buffer));
         send(newsockfd, "identifiant incorrect\n", 256,0);

       }
        
       bzero(buffer,256);
      }
     memset(buffer,0,sizeof(buffer));



     

     close(newsockfd);
     close(sockfd);
     return 0; 
}
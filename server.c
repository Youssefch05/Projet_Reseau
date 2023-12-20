
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <locale.h>

#define AJOUT 0
#define RETRAIT 1
#define SOLDE 2
#define DERNIERES_OPERATION 3

typedef struct Date {
    int jour;
    int mois;
    int annee;
} date_t;

typedef struct {
    int montant;
    int type_operation;
    date_t date_operation;
} operation_t;

typedef struct {
    int solde;
    int id_compte;
    operation_t dernieres_operations[10];
} compte_t;

typedef struct {
    int identifiant;
    char password[256];
    compte_t compte[2];
} client_t;

typedef struct {
    client_t clients[5];
    compte_t comptes[10];
} banque_t;


char password_str[256];
char compteid_str[256];
char identifiant_str[256];
int id = 0;
// Function to find a client based on identifier
int find_client(int id, client_t* tab_client, int size) {
    for (int i = 0; i < size; i++) {
        if (tab_client[i].identifiant == id) {
            return 1; // 1 if the client exists
        }
    }
    return 0; // 0 if the client does not exist
}

// Function to initialize the bank data
void initializeBanque(banque_t* banque) {
    for (int i = 0; i < 5; i++) {
        banque->clients[i].identifiant = i + 1;
        sprintf(banque->clients[i].password, "password%d", i + 1);

        for (int j = 0; j < 2; j++) {
            banque->clients[i].compte[j].solde = (i + 1) * 1000 + j * 500;
            banque->clients[i].compte[j].id_compte = i * 2 + j + 1;
            printf("%d\n", banque->clients[i].compte[j].id_compte);

            // Initialize last 10 operations for each account
            for (int k = 0; k < 10; k++) {
                banque->clients[i].compte[j].dernieres_operations[k].montant = (k + 1) * 100;
                banque->clients[i].compte[j].dernieres_operations[k].type_operation = k % 3;
                banque->clients[i].compte[j].dernieres_operations[k].date_operation.jour = k;
                banque->clients[i].compte[j].dernieres_operations[k].date_operation.mois = k;
                banque->clients[i].compte[j].dernieres_operations[k].date_operation.annee = 2023;
            }
        }
    }
}

char *operation1O(int operation){
    switch (operation)
    {
    case 0:
        return "AJOUT";
        break;
    case 1:
        return "RETRAIT";
        break;
    case 2:
        return "SOLDE";
        break;
    case 3:
        return "DERNIERES OPERATIONS";
        break;
    default:
        return "NO OPERATION";  
        break;
    }

}



int main(int argc, char* argv[]) {
    banque_t mabanque;
    initializeBanque(&mabanque);
    setlocale(LC_ALL, "en_US.utf8");


    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    // create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        perror("ERROR opening socket");

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)); //même si le port est occupé on peut le réutiliser
    // clear address structure
    bzero((char*)&serv_addr, sizeof(serv_addr));

    portno = atoi(argv[1]);

    // setup the host_addr structure for use in bind call
    // server byte order
    serv_addr.sin_family = AF_INET;

    // automatically be filled with current host's IP address
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // convert short integer value for port must be converted into network byte order
    serv_addr.sin_port = htons(portno);

    // bind the socket
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        perror("ERROR on binding");

    // listen for incoming connections
    listen(sockfd, 5);

    while (1) {
        // accept an incoming connection
        clilen = sizeof(cli_addr);

        // this accept() function will write the connecting client's address info
        // into the address structure and the size of that structure is clilen.
        // the accept() returns a new socket file descriptor for the accepted connection.
        // so, the original socket file descriptor can continue to be used
        // for accepting new connections while the new socket file descriptor is used for
        // communicating with the connected client.
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);

        printf("server: got connection from %s port %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

        // this send() function sends the 13 bytes of the string to the new socket
        //send(newsockfd, "Veuillez entrer votre Identifiant client!\n", 256, 0);
        printf("%d\n", mabanque.clients[0].identifiant);
        printf("%s\n",mabanque.clients[0].password);

        bzero(buffer, 256);

        n = read(newsockfd, buffer, sizeof(buffer) - 1);
        strcpy(identifiant_str, buffer);
        
        if (n <= 0) {
            perror("ERROR reading from socket");
            
            continue;
        }
        buffer[n] = '\0'; // null-terminate the received data

        n = read(newsockfd, buffer, sizeof(buffer) - 1);
        if (n <= 0) {
            perror("ERROR reading from socket");
            continue;
        }
        buffer[n] = '\0'; // null-terminate the received data
        printf("%s\n", buffer);

        strcpy(password_str, buffer);


        n = read(newsockfd, buffer, sizeof(buffer) - 1);
        if (n <= 0) {
            perror("ERROR reading from socket");
            continue;
        }
        buffer[n] = '\0'; // null-terminate the received data
        printf("%s\n", buffer);

        strcpy(compteid_str, buffer);

        





        buffer[n] = '\0'; // null-terminate the received data
    

       if (find_client(atoi(identifiant_str), mabanque.clients, 5) &&
    strcmp(mabanque.clients[atoi(identifiant_str) - 1].password, password_str) == 0 &&
    (mabanque.clients[atoi(identifiant_str) - 1].compte[0].id_compte == atoi(compteid_str) || //cas du premier compte du client
     mabanque.clients[atoi(identifiant_str) - 1].compte[1].id_compte == atoi(compteid_str))) { // cas du second compte du client
    // Your code here

            printf("Client trouvé %d\n", atoi(buffer)) ;
            printf("Voici l'identifiant : %d\n", atoi(identifiant_str));
            printf("voici le mdp : %s\n", password_str);
            printf("voici le compte : %d\n", atoi(compteid_str));
            bzero(buffer, 256);
            n = write(newsockfd, "OK\n", 256);

            
            
        } else {
            printf("Client introuvable %d", atoi(buffer));
            bzero(buffer, 256);
            n = write(newsockfd, "KO\n", 256);
        }

        // clear the buffer for the next iteration
        bzero(buffer, 256);

        n = read(newsockfd, buffer, sizeof(buffer) - 1);
        if (n <= 0) {
            perror("ERROR reading from socket");
            exit(1);
        }
        buffer[n] = '\0'; // null-terminate the received data*
        char choix_str[256];
        strcpy(choix_str, buffer);
        printf("%s\n", choix_str);
        bzero(buffer, 256);
        if (n < 0)
            perror("ERROR writing to socket");

        
        char solde_str[256];
            sprintf(solde_str,  "%d", mabanque.clients[atoi(identifiant_str-1)].compte[atoi(compteid_str)].solde);
        /*n = read(newsockfd, buffer, sizeof(buffer) - 1);
        if (n <= 0) {
            perror("ERROR reading from socket");
            exit(1);
        }
        buffer[n] = '\0'; // null-terminate the received data
        
        
        int choix = atoi(buffer);
        printf("%d\n", choix);  
        char solde_str[256];
        sprintf(solde_str,  "%d", mabanque.clients[atoi(identifiant_str)].compte[atoi(compteid_str)].solde); 
        */    
            switch (atoi(choix_str))
        {
        
        case AJOUT:

            n = read(newsockfd, buffer, sizeof(buffer) - 1);
            //printf("%s\n", buffer);
            buffer[n] = '\0'; // null-terminate the received data
            int montant = atoi(buffer);

            printf("%d\n", montant);
            printf("%d\n", mabanque.clients[atoi(identifiant_str)-1].compte[atoi(compteid_str)].solde);

            mabanque.clients[atoi(identifiant_str)-1].compte[atoi(compteid_str)].solde += montant;

            printf("%d\n", mabanque.clients[atoi(identifiant_str)-1].compte[atoi(compteid_str)].solde); 

            


            
            char solde_str_maj[256] = {'\0'};
            printf("%s", solde_str_maj);

            sprintf(solde_str_maj,  "%d", mabanque.clients[atoi(identifiant_str-1)].compte[atoi(compteid_str)].solde);


            


           
           
            bzero(buffer, 256);
            n = write(newsockfd, solde_str_maj, strlen(solde_str_maj)+1);
             printf("le client %d a ajouté %d euros\n", atoi(identifiant_str), montant); 

            break;
        case RETRAIT:
            n = read(newsockfd, buffer, sizeof(buffer) - 1);
            //printf("%s\n", buffer);
            buffer[n] = '\0'; // null-terminate the received data
            int montant_retrait = atoi(buffer);
            printf("%d\n", montant_retrait);
            printf("%d\n", mabanque.clients[atoi(identifiant_str)-1].compte[atoi(compteid_str)].solde);
            mabanque.clients[atoi(identifiant_str) -1].compte[atoi(compteid_str)].solde -= montant_retrait;
            printf("%d\n", mabanque.clients[atoi(identifiant_str)-1].compte[atoi(compteid_str)].solde); 
            char solde_str_maj_retrait[256];
            sprintf(solde_str_maj_retrait,  "%d", mabanque.clients[atoi(identifiant_str-1)].compte[atoi(compteid_str)].solde);
           
            bzero(buffer, 256);
            n = write(newsockfd, solde_str_maj_retrait, strlen(solde_str_maj_retrait)+1);
             printf("le client %d a retiré %d euros\n", atoi(identifiant_str), montant_retrait); 

            break;
        case SOLDE:
            printf("le client %d a demandé son solde\n", atoi(identifiant_str));
            n = write(newsockfd, solde_str, 256);
            if (n < 0)
                perror("ERROR writing to socket");
        
            bzero(buffer, 256);
            break;
        case DERNIERES_OPERATION:

            printf("le client %d a demandé ses dernieres operations\n", atoi(identifiant_str));
            char dernieres_operations_str[4096];
            for (int i = 0; i < 10; i++)
            {
                sprintf(dernieres_operations_str, "%d %s %d/%d/%d\n", mabanque.clients[atoi(identifiant_str)-1].compte[atoi(compteid_str)].dernieres_operations[i].montant, operation1O(mabanque.clients[atoi(identifiant_str)-1].compte[atoi(compteid_str)].dernieres_operations[i].type_operation), mabanque.clients[atoi(identifiant_str)-1].compte[atoi(compteid_str)].dernieres_operations[i].date_operation.jour, mabanque.clients[atoi(identifiant_str)-1].compte[atoi(compteid_str)].dernieres_operations[i].date_operation.mois, mabanque.clients[atoi(identifiant_str)-1].compte[atoi(compteid_str)].dernieres_operations[i].date_operation.annee);
                n = write(newsockfd, dernieres_operations_str, strlen(dernieres_operations_str)+1);
                if(n< 0 )
                    perror("ERROR writing to socket");
                

            }

            


            if (n < 0)
                perror("ERROR writing to socket");
        
            bzero(buffer, 256);
            break;
        default:
            break;

        }


        
        
    }
close(newsockfd);
    // close the server socket
    close(sockfd);

    return 0;
}

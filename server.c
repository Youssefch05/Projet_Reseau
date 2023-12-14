
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>

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

            // Initialize last 10 operations for each account
            for (int k = 0; k < 10; k++) {
                banque->clients[i].compte[j].dernieres_operations[k].montant = (k + 1) * 100;
                banque->clients[i].compte[j].dernieres_operations[k].type_operation = AJOUT;
                banque->clients[i].compte[j].dernieres_operations[k].date_operation.jour = k;
                banque->clients[i].compte[j].dernieres_operations[k].date_operation.mois = k;
                banque->clients[i].compte[j].dernieres_operations[k].date_operation.annee = 2023;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    banque_t mabanque;
    initializeBanque(&mabanque);

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

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
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

        if (newsockfd < 0)
            perror("ERROR on accept");

        printf("server: got connection from %s port %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

        // this send() function sends the 13 bytes of the string to the new socket
        send(newsockfd, "Veuillez entrer votre Identifiant client!\n", 256, 0);

        n = read(newsockfd, buffer, sizeof(buffer) - 1);
        if (n <= 0) {
            perror("ERROR reading from socket");
            close(newsockfd);
            continue;
        }

        buffer[n] = '\0'; // null-terminate the received data

        // check if the client exists
        if (find_client(atoi(buffer), mabanque.clients, sizeof(mabanque.clients) / sizeof(mabanque.clients[0]))) {
            printf("Voici l'identifiant : %d\n", atoi(buffer));
            send(newsockfd, "Identifiant bien reçu, entrez maintenant votre mot de passe\n", 256, 0);
        } else {
            printf("Client introuvable %d", atoi(buffer));
            send(newsockfd, "Identifiant incorrect\n", 256, 0);
        }

        // clear the buffer for the next iteration
        bzero(buffer, 256);

        n = read(newsockfd, buffer, sizeof(buffer) - 1);
        printf("mot de passe reçu %s", buffer);
        if (n <= 0) {
            perror("ERROR reading from socket");
            close(newsockfd);
            continue;
        }

        buffer[n] = '\0'; // null-terminate the received data
        if(strcmp(buffer, mabanque.clients[atoi(buffer) - 1].password) == 0){
            printf("Mot de passe correct\n");
            send(newsockfd, "Mot de passe correct\n", 256, 0);
        }
        else{
            printf("Mot de passe incorrect\n");
            send(newsockfd, "Mot de passe incorrect\n", 256, 0);
            close(newsockfd);
            continue;
        }
        
    }
    close(newsockfd);
    // close the server socket
    close(sockfd);

    return 0;
}

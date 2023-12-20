#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <locale.h>

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "en_US.utf8");

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int ope_engagee = 0;    
    int connected = 0;
    //int id = 0;
    char answer[256];
    char montant[256]; // variables de debug pour voir ce que contient le buffer
    char operation[256];

    char identifiant[256], password[256], buffer[255], compteid[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        herror("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr_list[0], 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        herror("ERROR connecting");
        } 
    while(connected == 0){

    
        printf("entrez votre identifiant\n");
        
       
        bzero(identifiant,256);
        scanf(" %255s", identifiant);
        n = write(sockfd, identifiant, strlen(identifiant));
        if(n< 0 )
            herror("ERROR writing to socket");
        printf("Enter your password: \n");
        scanf(" %255s",password);
        n = write(sockfd, password, strlen(password)  );

        if(n< 0 )
            herror("ERROR writing to socket");
        printf("entrez l'identifiant du compte\n");
        scanf(" %255s", compteid);
        n = write(sockfd, compteid, strlen(compteid));
        if(n< 0 )
            herror("ERROR writing to socket");
        //bzero(buffer,256);
        n = read(sockfd, buffer, 255);
        printf("%s\n",buffer);
         strcpy(answer, buffer);
        if (n < 0) 
            herror("ERROR reading from socket");

        buffer[n] = '\0';


        
        if(strcmp(answer, "OK\n")==0){
            printf("Identifiants corrects\n");
            connected = 1;

            bzero(buffer,255);
        }
         else if(strcmp(answer, "KO\n")==0){
            printf("Identifiant, mdp ou idcompte incorrect\n");
            bzero(buffer,255);
            exit(0);
        }
       

        


        
        int i = 0;
        //printf("tour de boucle\n %d", i); 
        i++; 
    }

     //printf("Sortie de la boucle\n");           
        printf("vous êtes connecté\n");
    
    
    while(connected ==1){
        
        bzero(buffer,255);
        if(n< 0 )
            herror("ERROR writing to socket");
        bzero(buffer,255);
        if(n< 0 )
            herror("ERROR writing to socket""");

        if(ope_engagee == 0){

        printf("entrez votre operation 0 ajout, 1 retrait, 2 solde, 3 dernieres ope \n");     
        scanf(" %255s", operation);
        n = write(sockfd, operation, strlen(operation));
        ope_engagee = 1;
        continue;
        }
        
        if(n< 0 )
            herror("ERROR writing to socket");

       /* if(atoi(operation) == 0|| atoi(operation) == 1){
            printf("entrez le montant de l'operation\n");
            scanf(" %255s", montant);
            n = write(sockfd, montant, strlen(montant));   
            if(n< 0 )
                herror("ERROR writing to socket");
            n = read(sockfd, buffer, 255);
            printf("%s\n",buffer);
            ope_engagee = 0;
            continue;
        }
        */

       switch (atoi(operation))
       {
       case 0:
              printf("AJOUT: entrez le montant de l'operation\n");
                scanf(" %255s", montant);
                n = write(sockfd, montant, strlen(montant));  

                if(n< 0 )
                 herror("ERROR writing to socket");
                
                bzero(buffer,255); // on remet le buffer a*
                usleep(1000000);
                n = read(sockfd, buffer, 255);

                if (n < 0){
                    herror("ERROR reading from socket");
                }
                buffer[n] = '\0';    
                printf("%s\n",buffer);

                    printf("ajout reussi\n");
                ope_engagee = 0;
                break;  

         case 1:
                printf("RETRAIT: entrez le montant de l'operation\n");
                    scanf(" %255s", montant);
                n = write(sockfd, montant, strlen(montant));  

                if(n< 0 )
                 herror("ERROR writing to socket");
                
                bzero(buffer,255); // on remet le buffer a*
                usleep(1000000);
                n = read(sockfd, buffer, 255);

                if (n < 0){
                    herror("ERROR reading from socket");
                }
                buffer[n] = '\0';    
                printf("%s\n",buffer);

                    printf("retrait reussi\n");
                ope_engagee = 0;
                    break;
        
        case 2:
                printf("SOLDE: voici votre solde\n");
                    usleep(1000000);
                    n = read(sockfd, buffer, 255);
                    printf("%s\n",buffer);
                    ope_engagee = 0;

                    break;
        
        case 3:
                printf("DERNIERES OPERATIONS: voici vos dernieres operations\n");
                    usleep(1000000);
                    n = read(sockfd, buffer, 255);
                    printf("%s\n",buffer);
                    ope_engagee = 0;    

        break;
       
       default:
        break;
           }
}
        
    
    
    
    close(sockfd);
    return 0;
}

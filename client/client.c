#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>

#include <netinet/in.h>
#include <netinet/tcp.h>


// METHODS
void printMenu();

// MAIN
int main(int argc, char *argv[]) {

  char string[200];

  printMenu();

  // GET THE USER REQUEST
  while(strcmp(fgets( string, sizeof(string), stdin ) , "exit\n") != 0) {

    char* request = string;

    if(strcmp(request, "HELP\n") == 0 || strcmp(request, "HELP") == 0) {
      // HELP MENU REQUESTED
      printMenu();
    } else {
      // SEND A REQUEST

      // INIT SOCKET
      int sock = socket(PF_INET,SOCK_STREAM,0);
      if (sock==-1) {
        perror("creation socket");
        exit(1);
      }
      struct sockaddr_in point_connexion_serveur;
      point_connexion_serveur.sin_family = AF_INET; // pt cnx IPv4
      point_connexion_serveur.sin_port = htons(65000); // port format réseau
      point_connexion_serveur.sin_addr.s_addr = inet_addr("127.0.0.1"); // adresse format réseau
      if (connect(sock,(struct sockaddr *)&point_connexion_serveur,sizeof(point_connexion_serveur))==-1) {
        perror("connexion");
        exit(1);
      }

      printf("### ENVOIE : %s\n", request);
      write(sock, request, strlen(request) + 1);

      // GET THE RESPONSE FROM SERVER
      char c;
      char response[500] = "";
      while ( read(sock,&c,1) ) {
        strcat(response, &c);
        
        if(c == '\0') {
          break;
        }
      }
      printf("%s\n", response);
      if(strcmp(response, "NOSUCHFILE\n") == 0) {
        printf("NO FILE AVAILABLE FOR THIS CRITERIA(S).\n");
      } else {

        char* tmpResponse;
        tmpResponse = strdup(response);
        char* key = strtok(tmpResponse, " ");

        if(strcmp(key, "FILES") == 0) { // LIST OF FILES 
          printf("FILES : \n\t");
          for(int i = 0; i < sizeof(tmpResponse); i++) {
            if(response[i] == ',')
              printf("\n\t");
            else
              printf("%c\n", response[i]);
          }
        } else if(strcmp(key, "CONTENT") == 0) { // CONTENT OF FILE

        } else {
          printf("BAD REQUEST\n");
        }
      }

      close(sock);
    }

  }
  exit(0);
}



void printMenu() {
  printf("###############################\n");
  printf("Welcome in Files' Manager\n\n");

  printf("WANT C1,C2,C3\n");
  printf("\tCx : a flag of one word\n");
  printf("NEED ID\n");
  printf("\tID : the number of the file\n");
  printf("HELP\n");
  printf("\tPrint this message again\n");
  printf("exit\n");
  printf("\tExit Files' Manager\n");
  printf("###############################\n\n\n");
}
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

// DEF FILE STRUCT
typedef struct _file { 
  int id;
  char* tags;
} file;

int main(int argc, char *argv[]) {

  // SAMPLE TAB OF FILES
  file files[10];
  for(int i = 0; i < 10; i++) {

    file f;
    f.id = i+1;
    
    char aString[12];
    sprintf(aString, "pre_%d_suff", i);
    f.tags = malloc(sizeof(aString));
    strncpy(f.tags, aString, sizeof(aString));
    strcat(f.tags, ",aze");
    strcat(f.tags, ",123");
    // f.tags = s;
    
    files[i] = f;
    printf("%d : %s\n", files[i].id, files[i].tags);
  }


  // INIT SOCK
  int sock = socket(PF_INET,SOCK_STREAM,0);
  
  if (sock==-1) {
    perror("creation socket");
    exit(1);
  }

  struct sockaddr_in point_connexion_serveur;
  point_connexion_serveur.sin_family = AF_INET; // pt cnx IPv4
  point_connexion_serveur.sin_port = htons(65000); // port format réseau
  point_connexion_serveur.sin_addr.s_addr = htonl(INADDR_ANY); // depuis n'importe quel réseau

  if (bind(sock,(struct sockaddr *)&point_connexion_serveur,sizeof(point_connexion_serveur))==-1) {
    perror("bind");
    exit(1);
  }

  if (listen(sock,10)==-1) {
    perror("listen");
    exit(1);
  }

  struct sockaddr_in pt_client; // infos point de connexion du client
  socklen_t l;
  int sclient; // descripteur de communication avec le client

  // INFINIT LOOP
  while(1) {
    printf("le serveur attend une connexion\n");
    
    if ( (sclient = accept(sock,(struct sockaddr *)&pt_client,&l))==-1 ) {
      perror("accept");
      exit(1);
    }
    
    printf("le serveur a un nouveau client\n");
    int pid = fork();

    if (pid < 0){
        perror("Error on fork");
        exit(1);
    }

    if(pid == 0) {    
      printf("Ouverture du processus fils\n");

      char c;
      char request[100] = "";
      while ( read(sclient,&c,1) ) {
        strcat(request, &c);
        if(c == '\0') {
          break;
        }
      }

      char* tmpRequest;
      tmpRequest = strdup(request);
      char* key = strsep(&tmpRequest, " ");
      if(strcmp(key, "WANT") == 0) {

        // WANT REQUEST

        char* flags[3];
        int nbFlag = 0;
        
        char* token;
        char* argsTab;
        char* tofree;

        argsTab = strdup(tmpRequest);
        if (argsTab != NULL) {
          tofree = argsTab;
          while ((token = strsep(&argsTab, ",")) != NULL)
          {
            flags[nbFlag] = token;
            nbFlag++;
            printf("%s\n", token);
          }
          free(tofree);
        }

        char response[500] = "FILES ";
        for(int i = 0; i < 10; i++) {
          char* token2;
          char* argsTab2;
          char* tofree2;

          argsTab2 = strdup(files[i].tags);
          if (argsTab2 != NULL) {
            tofree2 = argsTab2;
            while ((token2 = strsep(&argsTab2, ",")) != NULL)
            {
              printf("---> %s\n", argsTab2);
              if(strcmp(flags[0], token2) == 0 || 
                strcmp(flags[1], token2) == 0 || 
                strcmp(flags[2], token2) == 0) {
                printf("### %s ###\n", token2);
                if(strcmp(response, "FILES ") != 0) {
                  strcat(response, ",");
                }

                strcat(response, token2);
              }
            }
            free(tofree2);
          }

          strcat(response, "\n");
          printf("------------------------------------\n%s\n------------------------------------\n", response);
          write(sclient, response, sizeof(response));
        }

      } else if(strcmp(key,"NEED") == 0) {

      } else {

      }

    }
  }
  
  close(sclient); 
  close(sock);
  for(int i = 0; i < 10; i++) {
    printf(">>>>>>>>>>>>>>>\n");
    free(files[i].tags);
  }

  exit(0);
}
/********************************************************************
 * Program: 		One-Time Pads
 * Author: Andrew 	Derringer
 * Last Modified: 	12/6/2019
 * Summary:		Params:	[1] Desired port
 * 			This client establishes a socket connection
 * 			on the passed port number and listens as a
 * 			background process. It establishes up to 5
 * 			connections where text and key are received
 * 			and used to encrypt message along with client
 * 			program signature.
********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#define BUF_S 100000


struct con_package {
   int conFD;
   int* active_threads;
};

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

void *enc_service(void *args) {
   struct con_package *pack;
   pack = (struct con_package*) args;
   int establishedConnectionFD = pack->conFD;
   int* active_threads = pack->active_threads;
   //int* FDptr = (int*) args;
   //int establishedConnectionFD = *FDptr;
   char buffer[BUF_S * 2];
   char chunk[BUF_S * 2];
   char sendBuffer[BUF_S * 2];
   int charsRead;

   (*active_threads)++;

   memset(buffer, '\0', sizeof(buffer));     
   while(1) {
      //printf("in the loop!");
      memset(chunk, '\0', sizeof(chunk));
      charsRead = recv(establishedConnectionFD, chunk, sizeof(chunk), 0); // Read the client's message from the socket
      if (charsRead < 0) {
         (*active_threads)--;
         pack->active_threads = NULL;
         free(pack);
         close(establishedConnectionFD);
         fprintf(stderr, "ERROR reading from socket\n");
         pthread_exit(NULL);
      } else if (strchr(chunk, '!') != NULL) {
         strcat(buffer, chunk);
         break;
      } else {
         strcat(buffer, chunk);
      }
   }
   //buffer[strlen(buffer)] = '\0';

   char* text = strtok(buffer, "*");
   char* key = strtok(NULL, "*");
   char* delim = strtok(NULL, "!");
   char* client;
   //printf("SERVER: I received this text from client: \"%s\"\n", text);
   //printf("SERVER: I received this key from client: \"%s\"\n", key);
   //printf("SERVER: I received this delim from client: \"%s\"\n", delim);
   //printf("SERVER: Connected to: %c\n", delim[0]);

    memset(sendBuffer, '\0', sizeof(sendBuffer)); 
   if ((client = strchr(delim, 'd')) == NULL) {
      strcpy(sendBuffer, "ERROR attempted connection with otp_dec_d$");
      charsRead = send(establishedConnectionFD, sendBuffer, sizeof(sendBuffer), 0); // Send success back
   } else {
      int textLength = strlen(text);
      //printf("Loop/enc length = %d\n", textLength);
      int i, temp, keyval;
      for (i = 0; i < textLength; i++) {
         temp = (text[i] - 64);
         keyval = (key[i] - 64);
         if (temp < 1) {
            temp = 27;
         }
         if (keyval < 1) {
            keyval = 27;
         }

         temp -= keyval;
         if (temp < 1) {
            temp += 27;
         }

         if (temp == 27) {
            temp = 32;
         } else {
            temp += 64;
         }
         sendBuffer[i] = (char) temp;
      }
      sendBuffer[strlen(sendBuffer)] = '!';

      //printf("SERVER: Sending this out: %s\n", sendBuffer);
      // Send a Success message back to the client
      charsRead = send(establishedConnectionFD, sendBuffer, sizeof(sendBuffer), 0); // Send success back
      if (charsRead < 0) {
         (*active_threads)--;
         pack->active_threads = NULL;
         free(pack);
         close(establishedConnectionFD);
         fprintf(stderr, "ERROR writing to socket\n");
         pthread_exit(NULL);
      }
   }
      close(establishedConnectionFD); // Close the existing socket which is connected to the client

      //pthread_detach(pthread_self());
      (*active_threads)--;
      pack->active_threads = NULL;
      pthread_detach(pthread_self());

}


int main(int argc, char *argv[]) {
   int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
   socklen_t sizeOfClientInfo;
   //char buffer[1000];
   struct sockaddr_in serverAddress, clientAddress;
   struct con_package *pack;
   pthread_t thread;
   int t = 0, *active_threads = &t;
   //*active_threads = 0;
   //printf("Active THread count made!\n");

   if (argc < 2) {
      fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1);
   } // Check usage & args

   // Set up the address struct for this process (the server)
   memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
   portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
   serverAddress.sin_family = AF_INET; // Create a network-capable socket
   serverAddress.sin_port = htons(portNumber); // Store the port number
   serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

   // Set up the socket
   listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
   if (listenSocketFD < 0) {
   error("ERROR opening socket");
   }

   // Enable the socket to begin listening
   if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {// Connect socket to port
      error("ERROR on binding");
   }

   listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

   while (1) {
      // Accept a connection, blocking if one is not available until one connects
      sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
      establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
      if (establishedConnectionFD < 0) {
         error("ERROR on accept");
      }

      pack = (struct con_package*) malloc(1 * sizeof(struct con_package));
      //printf("You made a pack!\n");
      pack->conFD = establishedConnectionFD;
      pack->active_threads = active_threads;
      //int* tempptr = malloc(sizeof(int));
      //*tempptr = establishedConnectionFD;
      if( *active_threads < 5) {
         if(pthread_create(&thread, NULL, enc_service, pack) != 0) {
            error("Error on thread create");
         }
      } else {
         fprintf(stderr, "ERROR too many concurrent socket connections\n");
      }
      //close(establishedConnectionFD);

      // Get the message from the client and display it
   }

   close(listenSocketFD); // Close the listening socket
   return 0;

}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

#define DESIRED_ROOMS 7
#define TOTAL_ROOMS 10
#define MIN_CONNECTIONS 3
#define MAX_CONNECTIONS 6

/*
const int DESIRED_ROOMS = 7;
const int TOTAL_ROOMS = 10;
const int MIN_CONNECTIONS = 3;
const int MAX_CONNECTIONS = 6;
*/

void generateDirectory(char* filename_buffer, const int size) {

   // citation: stackoverflow.com/questions/34260350/appending-pid-to-filename
   // citation: geeksforgeeks.org/snprintf-c-library/
   int stringLength = 0;
   char* filename = "derringa.rooms.";
   pid_t pid = getpid();

   // Takes a char buffer of specified size and places datatypes into it.
   // Does not allow overflow and returns size of combined string for more info.
   stringLength = snprintf(filename_buffer, size, "%s%d", filename, (int) pid);

   if (stringLength < 0 || stringLength > size) {
      fprintf(stderr, "%s\n", "File name was too large for buffer. Process aborted.");
      exit(1);
   } else {
      //printf("%s\n", filename_buffer);
      mkdir(filename_buffer, 0777);
   }
}

struct Room {
   char name[20];
   char roomType[12];
   /*
   struct Room *con1; char con1[20];
   struct Room *con2; char con2[20];
   struct Room *con3; char con3[20];
   struct Room *con4; char con4[20];
   struct Room *con5; char con5[20];
   struct Room *con6; char con6[20];
   */
   struct Room *connectedRooms[MAX_CONNECTIONS];
   int connections;
};

void generateRoomSet(struct Room **roomSet, const int size) {

   // size of and initialization of hard coded starting rooms
   char* possibleRooms[] = {"Library", "Dining_Room", "Observatory",
				"Throne_Room", "Zelda_Chambers", "Kings_Chambers",
				"Armory", "Guards_Chambers", "Sanctuary", "Jail"};

   // determines which 3 room types won't be used at random
   // pick an index, put it at the end, reduce scope and do it again
   int i, j, randIdx;
   for(i = TOTAL_ROOMS - 1; i >= size; i--) {
      randIdx = (rand() % (i + 1));
      possibleRooms[randIdx] = possibleRooms[i];
   }


   // iterate through room array and place new new room ptr in it
   // set its name from possible rooms array and room type   
   struct Room* newRoom;  // temp holder before placing ptr in arrayi
   for(i = 0; i < size; i++) {
      newRoom = (struct Room*)malloc(sizeof(struct Room));
      //newRoom->connectedRooms = {NULL};
      for (j = 0; j < MAX_CONNECTIONS; j++) {
        newRoom->connectedRooms[j] = NULL;
      }
      snprintf(newRoom->name, sizeof(newRoom->name), "%s", possibleRooms[i]);
      snprintf(newRoom->roomType, sizeof(newRoom->roomType), "MID_ROOM");
      //printf("You got here...\n");
      newRoom->connections = 0;
      roomSet[i] = newRoom;
   }

   // determine which rooms will be start and end randomly
   int startIdx = (rand() % (size + 1));
   int endIdx = (rand() % (size + 1));
   while (endIdx == startIdx) {  // start and end cant be the same room
      endIdx = (rand() % (size + 1));
   } // clear the buffer and allow new room type string to be entered
   memset(roomSet[startIdx]->roomType, 0, sizeof(roomSet[startIdx]->roomType));
   snprintf(roomSet[startIdx]->roomType, sizeof(roomSet[startIdx]->roomType), "START_ROOM");
   memset(roomSet[endIdx]->roomType, 0, sizeof(roomSet[endIdx]->roomType));
   snprintf(roomSet[endIdx]->roomType, sizeof(roomSet[endIdx]->roomType), "END_ROOM");
}

int sameRoom(struct Room *A, struct Room *B) {
   int same = 0;

   if (A == B) {
      same = 1;
   }

   return same;
}

int spaceLeft(struct Room *B) {
   int space = 0;

   if (B->connections < 6) {
      space = 1;
   }
   
   return space;
}

void connectRooms(struct Room *A, struct Room *B) {
   int i = 0;

   while (i < MAX_CONNECTIONS && A->connectedRooms[i] != NULL) {
      i++;
   }

   A->connectedRooms[i] = B;
   A->connections++;
} 

int alreadyConnected(struct Room *A, struct Room *B) {
   int linked = 0;
   int i = 0;

   while (i < MAX_CONNECTIONS && A->connectedRooms[i] != NULL) {
      if (A->connectedRooms[i] == B) {
         linked = 1;
         break;
      }
      i++;
   }

   return linked;
}

void addConnection(struct Room *A, struct Room *B) {

   if (sameRoom(A, B) == 0 && spaceLeft(B) == 1 && alreadyConnected(A, B) == 0) {
      connectRooms(A, B);
      connectRooms(B, A);
   }
}

void generateConnections(struct Room **roomSet, const int size, const int min, const int max) {
   int minConnections = (rand() % (max - min + 1)) + min;

   int i, randRoom;
   for (i = 0; i < size; i++) {
      while (roomSet[i]->connections < minConnections) {
         randRoom = (rand() % size);
         //printf("connecting room %d and room %d\n", i, randRoom);
         addConnection(roomSet[i], roomSet[randRoom]);
      }
   }

}

void freeRooms(struct Room **roomSet, const int size) {
   int i;
   for (i = 0; i < size; i++) {
      free(roomSet[i]);
   }

   free(roomSet);
}

void generateFile(char* filename_buffer, struct Room *A, const int size) {

   char ofile_buffer[size];
   int stringLength = 0;

   // Takes a char buffer of specified size and places datatypes into it.
   // Does not allow overflow and returns size of combined string for more info.
   stringLength = snprintf(ofile_buffer, size, "%s%s%s", filename_buffer, "/", A->name);

   if (stringLength < 0 || stringLength > size) {
      printf("File name was too large for buffer. Process aborted.\n");
   } else {
      //printf("file created: %s\n", ofile_buffer);
      //mkdir(filename_buffer);
   }

   int fd = open(ofile_buffer, O_WRONLY | O_CREAT, 0600);   
   //ssize_t nwrite;
   int j = 0;

   char printline_buffer[100];

   snprintf(printline_buffer, 100, "%s%s\n", "ROOM NAME: ", A->name);
   write(fd, printline_buffer, strlen(printline_buffer) * sizeof(char));
   memset(printline_buffer, 0, sizeof(printline_buffer));

   while (j < MAX_CONNECTIONS && A->connectedRooms[j] != NULL) {
      //printf("Connection %d: %s\n", j + 1, roomSet[i]->connectedRooms[j]->name);
      snprintf(printline_buffer, 100, "%s%d: %s\n", "CONNECTION ", j + 1, A->connectedRooms[j]->name);
      write(fd, printline_buffer, strlen(printline_buffer) * sizeof(char));
      memset(printline_buffer, 0, sizeof(printline_buffer));

      //write(fd, A->connectedRooms[j]->name , strlen(A->connectedRooms[j]->name) * sizeof(char));
      //write(fd, "\n", sizeof(char));
      j++;
   }
   snprintf(printline_buffer, 100, "%s%s\n", "ROOM TYPE: ", A->roomType);
   write(fd, printline_buffer, strlen(printline_buffer) * sizeof(char));
   memset(printline_buffer, 0, sizeof(printline_buffer));

   //write(fd, A->roomType, strlen(A->roomType) * sizeof(char));
   //write(fd, "\n", sizeof(char));

   close(fd);
}

int main () {

   srand(time(0));

   // create buffer for filename and pass it to generateDirectory
   const int BUFFER_SIZE = 60;
   char filename_buffer[BUFFER_SIZE];
   generateDirectory(filename_buffer, BUFFER_SIZE);

   // create array of room objects and randomly generate their features
   struct Room **roomSet = malloc(DESIRED_ROOMS * sizeof(struct Room*));
   generateRoomSet(roomSet, DESIRED_ROOMS);
   generateConnections(roomSet, DESIRED_ROOMS, MIN_CONNECTIONS, MAX_CONNECTIONS);

   // print statement to check on status of rooms
   int i;//, j;
   //char ofile_buffer[BUFFER_SIZE];
   for (i = 0; i < DESIRED_ROOMS ; i++) {
     /* printf("Room %d: %s (%s)\n", i + 1, roomSet[i]->name, roomSet[i]->roomType);
      j = 0;
      while (j < MAX_CONNECTIONS && roomSet[i]->connectedRooms[j] != NULL) {
         printf("Connection %d: %s\n", j + 1, roomSet[i]->connectedRooms[j]->name);
         j++;
      }*/
      generateFile(filename_buffer, roomSet[i], BUFFER_SIZE);
   }

   // frees dynamic array of pointers
   freeRooms(roomSet, DESIRED_ROOMS);
 
   return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h> //unused
#include <unistd.h> //unused
#include <sys/stat.h>
#include <sys/types.h> //unused
#include <string.h> 
#include <fcntl.h>
#include <dirent.h>

#define MAX_USERINPUT 20
#define FILEBYTE_SIZE 250
#define FILEBUFFER_SIZE 100
#define MAX_CONNECTIONS 6
#define TOTAL_ROOMS 7
#define MAX_FILESTRINGS 10

void getDirectory(char* directory_buffer) {

   struct stat dStat;
   struct dirent *dEntry;
   DIR *d = opendir(".");
   int latest = 0;

   if (d == NULL) {
      fprintf(stderr, "Could not open file\n");
      exit(1);
   }

   while ((dEntry = readdir(d)) != NULL) {

      memset(&dStat, 0, sizeof(dStat));
      if (stat(dEntry->d_name, &dStat) < 0) {
         fprintf(stderr, "Couldn't get stat from file\n");
         exit(1);
      }

     if (S_ISDIR(dStat.st_mode)) {
         if (dStat.st_mtime > latest && dEntry->d_name[0] != '.') {
            memset(directory_buffer, 0, sizeof(directory_buffer));
            snprintf(directory_buffer, FILEBUFFER_SIZE, "%s", dEntry->d_name);
            latest = dStat.st_mtime;
         }
      }
   }

   closedir(d);

}

struct Room {
   char name[20];
   char roomType[12];
   char connectedRooms[MAX_CONNECTIONS][20];
   int connections;
};

void populateRoom(char* file_path, struct Room **A) {
   int fd, byteSize;
   fd = open(file_path, O_RDONLY);

   if (fd < 0) {
      fprintf(stderr, "Could not open %s\n", file_path);
      exit(1);
   }

   byteSize = lseek(fd, 0, SEEK_END);
   lseek(fd, 0, SEEK_SET);
   
   char fileRead[FILEBYTE_SIZE];
   memset(fileRead, 0, sizeof(fileRead));
   read(fd, &fileRead, byteSize);
 
   int i = 1;
   int j = 1;
   char* fileStrings[MAX_FILESTRINGS]; 
   memset(fileStrings, 0, sizeof(fileStrings));

   char* token = strtok(fileRead, " \n");
   while (token != NULL) {
      if ( i % 3 == 0) {
         fileStrings[j] = token;
         j++;
      }
      token = strtok(NULL, " \n");
      i++;
   }

   fileStrings[0] = fileStrings[j - 1];
   fileStrings[j - 1] = NULL;

   struct Room *newRoom = (struct Room*)malloc(sizeof(struct Room));

   //for (i = 0; i < MAX_CONNECTIONS; i++) {
   //   newRoom->connectedRooms[i] = NULL;
   //}
 
   snprintf(newRoom->name, sizeof(newRoom->name), "%s", fileStrings[1]);
   snprintf(newRoom->roomType, sizeof(newRoom->roomType), "%s", fileStrings[0]);
   newRoom->connections = 0;

   i = 0;
   j = 2;
   while (j < MAX_FILESTRINGS && fileStrings[j] != NULL) {
      memset(newRoom->connectedRooms[i], 0, sizeof(newRoom->connectedRooms[i]));
      snprintf(newRoom->connectedRooms[i], sizeof(newRoom->connectedRooms[i]), "%s", fileStrings[j]);
      //newRoom->connectedRooms[i] = fileStrings[j];
      newRoom->connections++;
      i++;
      j++;
   }

   *A = newRoom;
   //printf("%s\n", newRoom->name);
   //for (j = 0; j < newRoom->connections; j++) {
   //   printf("%s\n", newRoom->connectedRooms[j]);
   //}
 

   close(fd);

}

void generateRooms(char *directory_buffer, struct Room **roomSet) {

   int length;
   char path[FILEBUFFER_SIZE];
   length = snprintf(path, FILEBUFFER_SIZE, "./%s", directory_buffer);

   if (length < 0 || length > FILEBUFFER_SIZE) {
      fprintf(stderr, "%s\n", "Path name was too large for buffer. Process aborted.");
      exit(1);
   } 

   struct stat dStat; 
   struct dirent *dEntry;
   DIR *d = opendir(path);
   char file_path[FILEBUFFER_SIZE];

   if (d == NULL) {
      fprintf(stderr, "Could not open file\n");
      exit(1);
   }

   int i = 0;
   int j;
   while ((dEntry = readdir(d)) != NULL) {

      memset(&dStat, 0, sizeof(dStat));
      memset(file_path, 0, sizeof(file_path));
      snprintf(file_path, FILEBUFFER_SIZE, "%s/%s", path, dEntry->d_name);

      if (stat(file_path, &dStat) < 0) {
         fprintf(stderr, "Couldn't get stat from file\n");
         exit(1);
      }

      if (S_ISREG(dStat.st_mode)) {
         populateRoom(file_path, &roomSet[i]);
         //printf("Name: %s\n", roomSet[i]->name);
         //for (j = 0; j < roomSet[i]->connections; j++) {
         //   printf("%s\n", roomSet[i]->connectedRooms[j]);
         //}
 
         i++;
      }
   } 

   closedir(d);

}

struct Room* getRoomByType(struct Room **roomSet, char* desiredType) {

   struct Room *desiredRoom = roomSet[0];

   int i, equal;
   for (i = 0; i < TOTAL_ROOMS; i++) {
      equal = strcmp(roomSet[i]->roomType, desiredType);
      if (equal == 0) {
         desiredRoom = roomSet[i];
      }
   }

   return desiredRoom;

}

struct Room* getRoomByName(struct Room **roomSet, char* desiredName) {

   struct Room *desiredRoom = NULL;

   int i, equal;
   for (i = 0; i < TOTAL_ROOMS; i++) {
      equal = strcmp(roomSet[i]->name, desiredName);
      if (equal == 0) {
         desiredRoom = roomSet[i];
      }
   }

   return desiredRoom;

}

int sameRoom(struct Room *A, struct Room *B) {
   int same = 0;

   if (A == B) {
      same = 1;
   }

   return same;
}

void printRoom(struct Room *A) {
   printf("CURRENT LOCATION: %s\n", A->name);
   printf("POSSIBLE CONNECTIONS: ");

   int i;
   for (i = 0; i < A->connections; i++) {
      if (i == A->connections - 1) {
         printf("%s.\n", A->connectedRooms[i]);
      } else {
         printf("%s, ", A->connectedRooms[i]);
      }
   }
}

int validInput(char* userInput, struct Room **roomSet) {
   int valid = 0;

   printf("WHERE TO? >");
   fgets(userInput, MAX_USERINPUT, stdin);

   if (userInput[strlen(userInput) - 1] != '\n') {
      int dropped = 0;
      while (fgetc(stdin) != '\n') {
         dropped++;
      }

      if (dropped > 0) {
         printf("\nYour input exceeded buffer space! Lets try this again.\n");
         return valid;
      }else {
         userInput[strlen(userInput) - 1] = '\0';
      }
   }
   userInput[strcspn(userInput, "\n")] = 0;

   int i;
   for (i = 0; i < TOTAL_ROOMS; i++) {
      if (strcmp(userInput, roomSet[i]->name) == 0) {
         valid = 1;
         return valid;
      }
   }
   //printf("%s", userInput);
   printf("\nYou didn't enter an available room! Lets try this again.\n");
   return valid;

}

void freeRooms(struct Room **roomSet, const int size) {
   int i;
   for (i = 0; i < size; i++) {
      free(roomSet[i]);
   }

   free(roomSet);
}

int main () {

   char directory_buffer[FILEBUFFER_SIZE];
   getDirectory(directory_buffer);

   struct Room **roomSet = malloc(TOTAL_ROOMS * sizeof(struct Room*));
   generateRooms(directory_buffer, roomSet);

   char userInput[MAX_USERINPUT];
   int steps = 0;
   int roomsCapacity = 10;
   char **roomsVisited = malloc(roomsCapacity * sizeof(char*));
   

   struct Room *currRoom = getRoomByType(roomSet, "START_ROOM");
   struct Room *endRoom = getRoomByType(roomSet, "END_ROOM");

   while (sameRoom(currRoom, endRoom) == 0) {
      memset(userInput, 0, sizeof(userInput));
      printRoom(currRoom);
      if (validInput(userInput, roomSet) == 1) {
         currRoom = getRoomByName(roomSet, userInput);
         //printf("\nThat worked!\n");
         if (steps >= roomsCapacity) {
            roomsCapacity += 10;
            roomsVisited = realloc(roomsVisited, roomsCapacity * sizeof(char*));
         }
         roomsVisited[steps] = malloc(sizeof(char*));
         //snprintf(roomsVisited[steps], 20, "%s", currRoom->name);
         //strcpy(roomsVisited[steps], currRoom->name);
         roomsVisited[steps] = currRoom->name;
         steps++;
      }
      printf("\n");

     //currRoom = endRoom;
   }

   printf("You made it too the end! Go you!\n");
   printf("Rooms visited: %d\n", steps);
   printf("Path order: ");
   int i;
   for (i = 0; i < steps; i++) {
      if (i == steps - 1) {
         printf("%s\n", roomsVisited[i]);
      }else {
         printf("%s -> ", roomsVisited[i]);
      }
   }

   freeRooms(roomSet, TOTAL_ROOMS);
   free(roomsVisited);
 
   return 0;
}

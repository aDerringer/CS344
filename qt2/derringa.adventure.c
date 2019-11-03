/**********************************************************
 * Progam: 	File Adventures
 * Author: 	Andrew Derringer
 * Last 	Modified: 11/2/2019
 * Description:	Reads files from local directory to create
 * 		room structs. Start game and allow user
 * 		to move through rooms to the finish. Use of
 * 		multithreading to read current time,
 * 		store to file and read from file.
**********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <unistd.h> 
#include <sys/stat.h>
#include <sys/types.h> 
#include <string.h> 
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>

// global variabls for buffer and array sizes
#define MAX_USERINPUT 20
#define FILEBYTE_SIZE 250
#define FILEBUFFER_SIZE 100
#define MAX_CONNECTIONS 6
#define TOTAL_ROOMS 7
#define MAX_FILESTRINGS 10

// multithreading mutex available globally
pthread_mutex_t timeMutex;


/*******************************************
 * Name: 	Room
 * Summary: 	Manages strings printed together
 * 		to the same file.
 * Variable:	Unique name of room
 * Variable:	Room type start, mid, or end
 * Variable:	Array of connected room names
 * Variable:	Number of connections for efficient loops
*******************************************/
struct Room {
   char name[20];
   char roomType[12];
   char connectedRooms[MAX_CONNECTIONS][20];
   int connections;
};


/*******************************************
 * Function:	Find Newest Game Settings Directory
 * Summary:	Use of stat, DIR, and dirent structs
 * 		to access file information and find
 * 		the newest directory for pathing
 * 		to room struct files.
 * Param:	String buffer to hold directory name
 * Output:	Newest touched directory
*******************************************/
void getDirectory(char* directory_buffer) {

   struct stat dStat;
   struct dirent *dEntry;
   DIR *d = opendir(".");
   int latest = 0;

   if (d == NULL) {
      fprintf(stderr, "Could not open file\n");
      exit(1);
   }

   // while there are still files to read pass them into stat struct
   while ((dEntry = readdir(d)) != NULL) {

      memset(&dStat, 0, sizeof(dStat));
      if (stat(dEntry->d_name, &dStat) < 0) {
         fprintf(stderr, "Couldn't get stat from file\n");
         exit(1);
      }

     // if it's a directory and newest touched then use it
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


/*******************************************
 * Function: 	Initialized Room Struct
 * Summary:	Uses filepath string sent to it and
 * 		uninitialized room struct from array
 * 		to open file and use contents to
 * 		assign values to room variables
 * Param:	String file path
 * Param:	Uninitialized room struct
 * Output:	Initialize and populate room
*******************************************/
void populateRoom(char* file_path, struct Room **A) {

   int fd, byteSize;
   fd = open(file_path, O_RDONLY);

   if (fd < 0) {
      fprintf(stderr, "Could not open %s\n", file_path);
      exit(1);
   }

   // get size of entire file
   byteSize = lseek(fd, 0, SEEK_END);
   lseek(fd, 0, SEEK_SET);
   
   // buffer to hold everything read from file
   char fileRead[FILEBYTE_SIZE];
   memset(fileRead, 0, sizeof(fileRead));
   read(fd, &fileRead, byteSize);
 
   // use strtok to get inidivdual strings from file
   // and grab the desired strings for name
   // connections and type
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

   // the last string grabbed in type but we want to move
   // it up front so that the first index is always type
   // then name and then we can just iterate through until
   // null to get any number of connections.
   fileStrings[0] = fileStrings[j - 1];
   fileStrings[j - 1] = NULL;

   // make room and fill it with desired information in 3 steps
   struct Room *newRoom = (struct Room*)malloc(sizeof(struct Room));

   // 1) give room a name
   snprintf(newRoom->name, sizeof(newRoom->name), "%s", fileStrings[1]);

   // 2) give room a type
   snprintf(newRoom->roomType, sizeof(newRoom->roomType), "%s", fileStrings[0]);
   newRoom->connections = 0;

   // 3) give room a list of connected rooms
   i = 0;
   j = 2;
   while (j < MAX_FILESTRINGS && fileStrings[j] != NULL) {
      memset(newRoom->connectedRooms[i], 0, sizeof(newRoom->connectedRooms[i]));
      snprintf(newRoom->connectedRooms[i], sizeof(newRoom->connectedRooms[i]), "%s", fileStrings[j]);
      newRoom->connections++;
      i++;
      j++;
   }

   *A = newRoom;

   close(fd);

}

/*******************************************
 * Function:	Generate Rooms
 * Summary:	Gets directory to iterate through
 * 		its files and assign contents to
 * 		data held in room structs array
 * Param:	string buffer containing directory path
 * Param:	array of uninitialized rooms
 * Output:	Initialized array of rooms
*******************************************/
void generateRooms(char *directory_buffer, struct Room **roomSet) {

   // use passed string to open directory
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

   // iterate through files in directory and pass to function with room
   // struct from aray
   int i = 0;
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
         i++;
      }
   } 

   closedir(d);

}


/*******************************************
 * Function: 	Return room of certain type
 * Summary:	Takes validated user input of
 * 		a desired room by type and
 * 		iterates through list to return
 * 		pointer of that room.
 * Param:	Populated array of room structs
 * Param:	String of room type
 * Output:	Returns pointer of room type = string
*******************************************/
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


/*******************************************
 * Function: 	Return room of certain name
 * Summary:	Takes validated name input of
 * 		a desired room by type and
 * 		iterates through list to return
 * 		pointer of that room.
 * Param:	Populated array of room structs
 * Param:	String of room name
 * Output:	Returns pointer of room name = string
*******************************************/
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


/*******************************************
 * Function:	Same Room
 * Summary:	Bool function returning
 * 		whether 2 object pointers are equal
 * Param:	2 initialized room pointers
 * Return:	1=true 0=false
*******************************************/
int sameRoom(struct Room *A, struct Room *B) {
   int same = 0;

   if (A == B) {
      same = 1;
   }

   return same;
}


/*******************************************
 * Function:	Print room details
 * Summary:	Prints current room name and names
 * 		of all connected rooms
 * Param:	initialized room pointer
 * Output:	Menu for current room
*******************************************/
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


/*******************************************
 * Function:	Validate user input
 * Summary:	Checks if user input is either
 * 		a valid room name or time and
 * 		whether the user input exceeded
 * 		the buffer space.
 * Param:	Empty buffer to return user input
 * Param:	Initialized array of rooms
 * Return:	2=time 1=room name 0= invalid
*******************************************/
int validInput(char* userInput, struct Room **roomSet) {
   int valid = 0;

   // get user input and stuff it in userInput buffer
   printf("WHERE TO? >");
   fgets(userInput, MAX_USERINPUT, stdin);

   // check if character were dropped by looking for newline
   if (userInput[strlen(userInput) - 1] != '\n') {
      int dropped = 0;
      while (fgetc(stdin) != '\n') {
         dropped++;
      }

      // exceeded then print and let game loop in main reprint board
      if (dropped > 0) {
         printf("\nYour input exceeded buffer space! Lets try this again.\n");
         return valid;
      }else {
         userInput[strlen(userInput) - 1] = '\0';
      }
   }

   // grab only char before newline for comparison
   userInput[strcspn(userInput, "\n")] = 0;

   // iterate through rooms and find one that matches
   int i;
   for (i = 0; i < TOTAL_ROOMS; i++) {
      if (strcmp(userInput, roomSet[i]->name) == 0) {
         valid = 1;
         return valid;
      }
   }

   // check if user entered time to return 2
   if (strcmp(userInput, "time") == 0) {
      valid = 2;
      return valid;
   }

   // Only get here if input wasn't valid
   printf("\nYou didn't enter an available room! Lets try this again.\n");
   return valid;

}


/*******************************************
 * Function:	Deallocate Array
 * Summary:	Frees allocated array of allocations
 * Param:	Initialized dynamic array of dynamic structs
 * Ouput:	Frees memory from array
********************************************/
void freeRooms(struct Room **roomSet, const int size) {
   int i;
   for (i = 0; i < size; i++) {
      free(roomSet[i]);
   }

   free(roomSet);
}


/*******************************************
 * Function:	Write time file
 * Summary:	Gets current time and formats it before
 * 		passing buffer to a file created in the
 * 		function
 * Output:	A created or rewritten file containing current time
*******************************************/
void* writeTimeFile() {

   // variables to get time, structure it, and buffer to hold formated string
   char time_buffer[FILEBYTE_SIZE];
   time_t currTime;
   struct tm *formatTime;
   int fd;

   // place time in currTume, place that in formatTime tm struct
   // then use strftime to format output of tm time variables into buffer
   memset(time_buffer, 0, sizeof(time_buffer));
   time(&currTime);
   formatTime = localtime(&currTime);
   strftime(time_buffer, FILEBYTE_SIZE, "%I:%M%p %A, %B %d, %Y", formatTime);

   // set up file descriptor for write only and put time buffer in it
   // this will delete old contents every time.
   fd = open("timeOutput.txt", O_WRONLY | O_CREAT, 0600);   
   write(fd, time_buffer, strlen(time_buffer));
   lseek(fd, 0, SEEK_SET);

   close(fd);
 
   return NULL;

}


/*******************************************
 * Function:	Read time file
 * Summary:	Opens the time file that is always
 * 		created in a paired functionc all
 * 		before this and prints content to 
 * 		the screen
 * Output:	Print string of time to screen
*******************************************/
void readTimeFile() {
   
   char time_buffer[FILEBYTE_SIZE];
   int fd;

   memset(time_buffer, 0, sizeof(time_buffer));
   fd = open("timeOutput.txt", O_RDONLY);
  
   read(fd, &time_buffer, FILEBYTE_SIZE);

   printf("\n%s", time_buffer);
 
   close(fd);

}


/*******************************************
 * function:	Generate new thread
 * Summary:	locks the mutex and creates a new
 * 		thread to handle writing the file
 * 		then joins the threads and unlocks
 * 		the mutex
 * Output:	Calls file writing on new thread
*******************************************/
int generateThread() {

   int success = 1;

   // declare new thread
   pthread_t fileInThread;

   // lock mutex and start file in thread to write file of time
   pthread_mutex_lock(&timeMutex);
   if (pthread_create(&fileInThread, NULL, writeTimeFile, NULL) != 0) {
      printf("Thread Error...\n");
      success = 0;
   }

   // unlock the mutex and join the threads again
   pthread_mutex_unlock(&timeMutex);
   pthread_join(fileInThread, NULL);

   return success;

}

int main () {

   pthread_mutex_init(&timeMutex, NULL);

   // set up buffer for new directory and fill it 
   char directory_buffer[FILEBUFFER_SIZE];
   getDirectory(directory_buffer);

   // set up array of rooms and populate it
   struct Room **roomSet = malloc(TOTAL_ROOMS * sizeof(struct Room*));
   generateRooms(directory_buffer, roomSet);

   // set up parameters for game flow
   char userInput[MAX_USERINPUT];
   int choice;
   int steps = 0;
   int roomsCapacity = 10;
   char **roomsVisited = malloc(roomsCapacity * sizeof(char*));
   
   // get current room to begin game and end room to compare
   // and determine if game is over
   struct Room *currRoom = getRoomByType(roomSet, "START_ROOM");
   struct Room *endRoom = getRoomByType(roomSet, "END_ROOM");

   // starts every turn by checking if current and end room are the same
   while (sameRoom(currRoom, endRoom) == 0) {
      // clear any old input and print menu for current room
      memset(userInput, 0, sizeof(userInput));
      printRoom(currRoom);
      // validate user input for room or time or error message
      choice = validInput(userInput, roomSet);

      // if time else if a valid room was picked
      if (choice == 2) {
         if (generateThread() == 1) {
            readTimeFile();
         }
      }else if (choice == 1) {
         currRoom = getRoomByName(roomSet, userInput);

         if (steps >= roomsCapacity) {
            roomsCapacity += 10;
            roomsVisited = realloc(roomsVisited, roomsCapacity * sizeof(char*));
         }

         roomsVisited[steps] = malloc(sizeof(char*));
         roomsVisited[steps] = currRoom->name;
         steps++;
      }

      printf("\n");

   }

   // end of game information
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

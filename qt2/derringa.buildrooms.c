/**********************************************************
 * Progam: 	File Adventures
 * Author: 	Andrew Derringer
 * Last 	Modified: 10/31/2019
 * Description:	Running this file generates a unique directory
 * 		tagged with the current process ID and populates
 * 		it with 7 files used to generate room structures
 * 		elsewhere. There are 10 hardcoded possible rooms
 * 		of which 7 are chosen at random and connected
 * 		at random to 3 to 6 other rooms. Finally one room
 * 		is identified as a starting room and another as a
 * 		finishing room.
**********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

// Global definitions for generating arrays of room structs
#define DESIRED_ROOMS 7
#define TOTAL_ROOMS 10
#define MIN_CONNECTIONS 3
#define MAX_CONNECTIONS 6

/*******************************************
 * Name: 	Room
 * Summary: 	Manages strings printed together
 * 		to the same file.
 * Variable:	Unique name of room
 * Variable:	Room type start, mid, or end
 * Variable:	Array of connected room structs
 * Variable:	Number of connections for efficient loops
*******************************************/
struct Room {
   char name[20];
   char roomType[12];
   struct Room *connectedRooms[MAX_CONNECTIONS];
   int connections;
};


/*******************************************
 * Function: 	Directory Generator
 * Summary: 	Combines process ID and desired 
 * 		directory name into buffer,
 * 		makes directory, and sets
 * 		permissions.
 * Param:	Empty buffer from main
 * Param:	Size of buffer passed
 * Output:	Makes directory
*******************************************/
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
      mkdir(filename_buffer, 0777);
   }
}


/*******************************************
 * Function:	Generate Set of Rooms
 * Summary:	Eliminates 3 of 10 hard coded rooms
 * 		and generates 7 room structs with 
 * 		remaining names.
 * Param:	Array of room object points
 * Param:	Length of array
 * Output:	7 random rooms allocated, named
 * 		and available to calling functions
*******************************************/
void generateRoomSet(struct Room **roomSet, const int size) {

   // size of and initialization of hard coded starting rooms
   char* possibleRooms[] = {"Library", "Dining_Room", "Observatory",
				"Throne_Room", "Zeldas_Chambers", "Kings_Chambers",
				"Armory", "Guards_Chambers", "Sanctuary", "Jail"};

   // determines which 3 room types won't be used at random
   // pick an index, put it at the end, reduce scope and do it again
   int i, j, randIdx;
   for(i = TOTAL_ROOMS - 1; i >= size; i--) {
      randIdx = (rand() % (i + 1));
      possibleRooms[randIdx] = possibleRooms[i];
   }


   // iterate through room array and place new room ptr in it
   // set its name from possible rooms array and roomtype 
   struct Room* newRoom;  // temp holder before placing ptr in arrayi
   for(i = 0; i < size; i++) {

      // allocate space for new room and initialize it's variables
      newRoom = (struct Room*)malloc(sizeof(struct Room));
 
      for (j = 0; j < MAX_CONNECTIONS; j++) {
        newRoom->connectedRooms[j] = NULL;
      }
      snprintf(newRoom->name, sizeof(newRoom->name), "%s", possibleRooms[i]);
      snprintf(newRoom->roomType, sizeof(newRoom->roomType), "MID_ROOM");
      newRoom->connections = 0;

      roomSet[i] = newRoom;
   }

   // determine which rooms will be start and end randomly
   int startIdx = (rand() % (size));
   int endIdx = (rand() % (size));
   while (endIdx == startIdx) {  // start and end cant be the same room
      endIdx = (rand() % (size + 1));
   } // clear the buffer and allow new room type string to be entered
   memset(roomSet[startIdx]->roomType, 0, sizeof(roomSet[startIdx]->roomType));
   snprintf(roomSet[startIdx]->roomType, sizeof(roomSet[startIdx]->roomType), "START_ROOM");
   memset(roomSet[endIdx]->roomType, 0, sizeof(roomSet[endIdx]->roomType));
   snprintf(roomSet[endIdx]->roomType, sizeof(roomSet[endIdx]->roomType), "END_ROOM");

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
 * Function:	Open Connections Check
 * Summary:	Bool function returning
 * 		whether a room object has space
 * 		left to connect again.
 * Param:	Initialized room objecy pointer
 * Return:	1=yes 2=no
*******************************************/
int spaceLeft(struct Room *B) {

   int space = 0;

   if (B->connections < 6) {
      space = 1;
   }
   
   return space;

}


/*******************************************
 * Function:	Connect Rooms
 * Summary:	Iterates to find first empty
 * 		position in A room struct's array
 * 		of connected rooms and adds B.
 * Param:	Room struct adding a connection
 * Param:	Room struct being added
 * Output:	Connected room struct.
*******************************************/
void connectRooms(struct Room *A, struct Room *B) {
   int i = 0;

   // find the first null array index to add new connection
   // could also iterate with connection member variable
   while (i < MAX_CONNECTIONS && A->connectedRooms[i] != NULL) {
      i++;
   }

   A->connectedRooms[i] = B;
   A->connections++;
} 



/*******************************************
 * Function:	Checking for connection
 * Summary:	Bool uses 2 struct room pointers to see
 * 		if they are already connected.
 * Param:	Room struct being scanned
 * Param:	Room struct being compared
 * Return:	1=yes 0=no
*******************************************/
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


/*******************************************
 * Function:	Link Together Rooms
 * Summary:	Calls other function to confirm that
 * 		there is room to connect, they're not the
 * 		same room, and that they aren't already
 * 		connected. Then connect
 * Param:	2 initialized room structs to be connected
*******************************************/
void addConnection(struct Room *A, struct Room *B) {

   // if all requirements are met then connect both ways
   if (sameRoom(A, B) == 0 && spaceLeft(B) == 1 && alreadyConnected(A, B) == 0) {
      connectRooms(A, B);
      connectRooms(B, A);
   }
}


/*******************************************
 * Function:	Generate all room connections
 * Summary:	Loops through each room and while
 * 		they don't have the minimum number
 * 		of connections keep adding.
 * Param:	Array of rooms with no connections
 * Param:	size of array
 * Param:	Min connection developer allows
 * Param:	Max connections allowed
 * Output:	Passed array of rooms all connected
*******************************************/
void generateConnections(struct Room **roomSet, const int size, const int min, const int max) {

   // generate min connections this round from global min and max constants
   int minConnections = (rand() % (max - min + 1)) + min;

   int i, randRoom;
   
   // loop through each room in object and add more connections while their
   // connection count is too low
   for (i = 0; i < size; i++) {
      while (roomSet[i]->connections < minConnections) {
         randRoom = (rand() % size);
         addConnection(roomSet[i], roomSet[randRoom]);
      }
   }
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

void generateFile(char* filename_buffer, struct Room *A, const int size) {

   char ofile_buffer[size];
   int stringLength = 0;

   // Takes a char buffer of specified size and places datatypes into it.
   // Does not allow overflow and returns size of combined string for more info.
   stringLength = snprintf(ofile_buffer, size, "%s/%s", filename_buffer, A->name);

   if (stringLength < 0 || stringLength > size) {
      printf("File name was too large for buffer. Process aborted.\n");
   }   

   // Opens file descriptor and prep buffer then used to write to file
   // write accomplished in 3 steps
   int fd = open(ofile_buffer, O_WRONLY | O_CREAT, 0600);   
   int j = 0;
   char printline_buffer[100];

   // 1) write structure room name to file and clear buffer
   snprintf(printline_buffer, 100, "%s%s\n", "ROOM NAME: ", A->name);
   write(fd, printline_buffer, strlen(printline_buffer) * sizeof(char));
   memset(printline_buffer, 0, sizeof(printline_buffer));

   // 2) in loop write each connection to file and clear buffer
   while (j < MAX_CONNECTIONS && A->connectedRooms[j] != NULL) {
      snprintf(printline_buffer, 100, "%s%d: %s\n", "CONNECTION ", j + 1, A->connectedRooms[j]->name);
      write(fd, printline_buffer, strlen(printline_buffer) * sizeof(char));
      memset(printline_buffer, 0, sizeof(printline_buffer));
      j++;
   }

   // 3) write room type to file and lcear buffer for cleanup
   snprintf(printline_buffer, 100, "%s%s\n", "ROOM TYPE: ", A->roomType);
   write(fd, printline_buffer, strlen(printline_buffer) * sizeof(char));
   memset(printline_buffer, 0, sizeof(printline_buffer));

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

   // loop through each room and output features to file
   int i;
   for (i = 0; i < DESIRED_ROOMS ; i++) {
      generateFile(filename_buffer, roomSet[i], BUFFER_SIZE);
   }

   // frees dynamic array of pointers
   freeRooms(roomSet, DESIRED_ROOMS);
 
   return 0;
}

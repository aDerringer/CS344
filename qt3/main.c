#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/stat.h>
#include <sys/types.h> 
#include <string.h> 
#include <signal.h>
#include "helpers.h"

#define MAX_LINE 2048
#define MAX_ARR 512

int useAMP = 1;

int main() {

   int exit = 0;
   int status = 0;
   //char cwd[MAX_LINE];
   char input_buffer[MAX_LINE];
   char* args_arr[MAX_ARR];
   int args_size;

   

   while(exit == 0) {

      printf(":");
      fflush(stdout);
      if (validInput(input_buffer, MAX_LINE) == 1) {
         args_size = string_to_arr(input_buffer, args_arr);      

      }

      if(strcmp(args_arr[0], "exit") == 0 && args_size == 1) {
         exit = 1;
      } else if (strcmp(args_arr[0], "cd") == 0) {
         nav_chcwd(args_arr, args_size);
         //printf("PATH: %s\n", getenv("PATH"));
         //printf("HOME: %s\n", getenv("HOME"));
         //printf("DIRECTORY: %s\n", getcwd(cwd, sizeof(cwd)));
         //memset(cwd, 0, sizeof(cwd));
      } else if (strcmp(args_arr[0], "status") == 0) {
         printf("exit value %d\n", status); 
      } else if (strcmp(args_arr[0], "#") != 0 && args_size > 0) {
         printf("going into fg or bg mode!\n");
         if (strcmp(args_arr[args_size - 1], "&") == 0 && useAMP == 1) {
            printf("working in bg...\n");
         } else {
            printf("working in fg...\n");
         }
      }
      //print_arr(args_arr, args_size);
      free_string_arr(args_arr, args_size);
      //args_size = 0;
   }

   printf("Terminating...\n");

   return 0;
}

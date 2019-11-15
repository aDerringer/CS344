#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"
#include <string.h>

int validInput(char* input_buffer, int max_size) {

   memset(input_buffer, 0, max_size);
   fgets(input_buffer, max_size, stdin); 

   // check if character were dropped by looking for newline
   if (input_buffer[strlen(input_buffer) - 1] != '\n') {
      int dropped = 0;
      while (fgetc(stdin) != '\n') {
         dropped++;
      }

      // exceeded then print and let game loop in main reprint board
      if (dropped > 0) {
         printf("\nYour input exceeded buffer space! Lets try this again.\n");
         return 0;
      }else {
         input_buffer[strlen(input_buffer) - 1] = '\0';
      }
   }

   // grab only char before newline for comparison
   input_buffer[strcspn(input_buffer, "\n")] = 0;

   return 1;
}

int string_to_arr(char* input_buffer, char** stringArr) {

   int i = 0;
   char* temp;

   char* token = strtok(input_buffer, " ");
   while (token != NULL) {
      stringArr[i] = malloc(strlen(token) + 1);
      strcpy(stringArr[i], token);
      token = strtok(NULL, " ");
      i++;
   }

   if (i >= 512) {
      printf("Too many arguments passed. Let's try this again.\n");
      free_string_arr(stringArr, i);
      return 0;
   }

   return i;
}

void free_string_arr(char** stringArr, int size) {
   int i;
   for (i = 0; i < size; i++) {
      if (stringArr[i] != NULL) {
         free(stringArr[i]);
      }
   }
}

void print_arr(char** stringArr, int size) {
   int i;
   for (i = 0; i < size; i++) {
      printf("%s\n", stringArr[i]);
   }
}

void nav_chcwd(char** args_arr, int args_size) {
   char cwd[2048];
   char abs_path[] = "/nfs";

   if (args_size == 1) {
      chdir(getenv("HOME"));
   } else if (args_size == 2) {
      getcwd(cwd, sizeof(cwd));
      cwd[strlen(cwd)] = '/';
      if (strstr(args_arr[1], abs_path) != NULL) {
         if (chdir(args_arr[1]) == -1) {
            printf("Error: Not a valid directory\n");
         }
      } else {
         strcat(cwd, args_arr[1]);
         if (chdir(cwd) == -1) {
            printf("Error: Not a valid Directory\n");
         }
      }
      memset(cwd, 0, sizeof(cwd));
   } else {
      printf("Error: Too many argument passed. Expected 0 or 1.\n");
   }
}

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <string.h>

#include "history.h"


int initialize_history(History* history, int size) {
    printf("test init \n");
  history->max_size = size;
  history->lines = malloc(size * sizeof(char*));
  int i;
  for (i = 0; i < size; ++i) {
    history->lines[i] = NULL;
  }

}

void add_to_history(History* history, char* commandline) {
  if (history->lines[history->begin] != NULL) {
    free(history->lines[history->begin]);
  }
  history->lines[history->begin] = commandline;
  history->begin = (history->begin + 1) % history->max_size;
}

void print_history(History* history) {
  int i;
  int begin = history->begin;
  for (i = 0; i < history->max_size; ++i) {
    if (history->lines[begin] != NULL) {
      printf("%s\n", history->lines[begin]);
    }
    ++begin;
    if (begin >= history->max_size) {
      begin = 0;
    }
  }
}
void free_history(History* history) {
  // int i;
  // for (i = 0; i < history->max_size; ++i) {
  //   if (history->lines[i] != NULL) {
  //     free(history->lines[i]);
  //   }
  // }
  free(history->lines);
}

// int main(void) {
//     History* test_history = (History *)malloc(sizeof(History));
//     printf("essai history eh oh %d \n",*test_history);
//     int size = 30;
//     const char * content = "test de contenue";

//     if(initialize_history(test_history,30)) {
//         printf("test \n");
//     }
        
//     add_to_history(test_history,"je suis la premiere ligne de l'historique");
//     add_to_history(test_history,"**** deuxieme ligne de l'historique***");
//     add_to_history(test_history,content);
//     print_history(test_history);
//     free_history(test_history);
//     return 0;
// };


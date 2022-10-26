#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <string.h>

#include "history.h"
#include "server2.h"

int initialize_history(History* history, int size) {
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

void print_history(SOCKET socket, History* history) {
  int i;
  int begin = history->begin;
  if (history->lines[begin] == NULL) {
      if (send(socket, "*** Historique Vide ***", strlen("*** Historique Vide ***"), 0) < 0)
      {
        perror("send()");
        exit(errno);
      }
  } else {
      if (send(socket, "*** Voici votre historique ***", strlen("*** Voici votre historique ***"), 0) < 0)
      {
        perror("send()");
        exit(errno);
      }
  }
  for (i = 0; i < history->max_size; ++i) {
    if (history->lines[begin] != NULL) {
      char buff_to_add[strlen(history->lines[begin])+2];
      strcpy(buff_to_add,history->lines[begin]);
      if (send(socket, buff_to_add, strlen(buff_to_add), 0) < 0)
      {
        perror("send()");
        exit(errno);
      }
    }
    ++begin;
    if (begin >= history->max_size) {
      begin = 0;
    }
  }
}
void free_history(History* history) {
  free(history->lines);
}


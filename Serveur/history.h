#ifndef HISTORY_H
#define HISTORY_H


typedef struct
{
    char** lines;
    int max_size;
    int begin;
}History;

int initialize_history(History* history, int size);
void add_to_history(History* history, char* commandline);
void print_history(History* history);
void free_history(History* history);
#endif /* guard */
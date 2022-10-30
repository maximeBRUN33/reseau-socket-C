#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <string.h>
#include <stdbool.h>
#include "server2.h"
#include "client2.h"
#include "history.h"

// const char **clients_messagerie;
// int nb_messageries = 0;
const int HISTORY_SIZE = 50;
History* history;
int actualGroupe;
Groupe groupes[MAX_GROUPES];

static void init(void)
{
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if (err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }
#endif
}

static void end(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}

static void app(void)
{
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   /* the index for the array */
   int actual = 0;
   actualGroupe = 0;
   int max = sock;
   /* an array for all clients */
   Client clients[MAX_CLIENTS];
   history = (History *)malloc(sizeof(History));
   initialize_history(history,HISTORY_SIZE);
   fd_set rdfs;

   while (1)
   {
      int i = 0;
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the connection socket */
      FD_SET(sock, &rdfs);

      /* add socket of each client */
      for (i = 0; i < actual; i++)
      {
         FD_SET(clients[i].sock, &rdfs);
      }

      if (select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if (FD_ISSET(STDIN_FILENO, &rdfs))
      {
         /* stop process when type on keyboard */
         break;
      }
      else if (FD_ISSET(sock, &rdfs))
      {
         /* new client */
         SOCKADDR_IN csin = {0};
         size_t sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if (csock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }

         /* after connecting the client sends its name */
         if (read_client(csock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }

         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);

         Client c = {csock};
         strncpy(c.name, buffer, BUF_SIZE - 1);
         clients[actual] = c;
         actual++;

         char message_connection[BUF_SIZE];
         message_connection[0]=0;
         strcpy(message_connection, buffer);
         strcat(message_connection, " connected !");
         send_message_to_all_clients(clients, c, actual, message_connection, 1);
         // read history if has one
         print_history(c.sock, history);
      }
      else
      {
         bool action = false;
         int i = 0;
         for (i = 0; i < actual; i++)
         {
            /* a client is talking */
            if (FD_ISSET(clients[i].sock, &rdfs))
            {
               Client client = clients[i];
               int c = read_client(clients[i].sock, buffer);
               /* client disconnected */
               if (c == 0)
               {
                  closesocket(clients[i].sock);
                  remove_client(clients, i, &actual);
                  strncpy(buffer, client.name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  send_message_to_all_clients(clients, client, actual, buffer, 1);
                  action = true;
               }
               if ((strstr(buffer, "Private to ") - buffer == 0)&&(!action))
               {
                  char receiverNamebuff[BUF_SIZE];
                  strncpy(receiverNamebuff, &buffer[11], BUF_SIZE - 11);
                  char receiverName[BUF_SIZE];
                  strncpy(receiverName, receiverNamebuff, (strstr(receiverNamebuff, " : ") - receiverNamebuff));
                  int length = strlen(receiverName) + 3;
                  char *message = &receiverNamebuff[length];
                  for (int j = 0; j < actual; j++)
                  {
                     if ((strcmp(clients[j].name, receiverName) == 0)&&(!action))
                     {
                        send_message_to_a_client(clients, client, clients[j], actual, message, 0);
                     }
                  }
                  action = 1;
               }
               if((strstr(buffer, "Voir clients") - buffer == 0)&&(!action))
               {
                  for(int j = 0; j < actual; j++)
                  {
                     puts(clients[j].name);
                  }
                  action = true;
               }
               if((strstr(buffer, "Voir groupes") - buffer == 0)&&(!action))
               {
                  for(int j = 0; j < actualGroupe; j++)
                  {
                     printf("%s (%d)",groupes[j].name, groupes[j].actualMembre);
                     for(int k = 0; k < groupes[j].actualMembre; k++)
                     {
                        printf("\nMembre Num %d : %s", k, groupes[j].membres[k].name);
                     }
                     printf("\n");
                  }
                  action = true;
               }
               if((strstr(buffer, "To ") - buffer == 0)&&(!action))
               {
                  char groupNamebuff[BUF_SIZE] = "\0";
                  strncpy(groupNamebuff, &buffer[3], BUF_SIZE - 3);
                  char groupName[BUF_SIZE] = "\0";
                  strncpy(groupName, groupNamebuff, (strstr(groupNamebuff, " : ") - groupNamebuff));
                  char message[BUF_SIZE] = "\0";
                  int debutMessage = 6 + strlen(groupName);
                  strncpy(message, &buffer[debutMessage], BUF_SIZE - debutMessage);
                  for(int j = 0; j < actualGroupe; j++)
                  {
                     if(strcmp(groupes[j].name, groupName) == 0)
                     {
                        for(int k = 0; k<groupes[j].actualMembre; k++)
                        {
                           if(strcmp(groupes[j].membres[k].name, client.name) == 0)
                           {
                              send_message_to_a_group(groupes[j].membres, client, groupes[j].actualMembre, message, groupName, 0);
                           }
                        }
                     }
                  }
                  action = true;
               }
               if((strstr(buffer, "Group ") - buffer == 0)&&(!action))
               {
                  char groupNamebuff[BUF_SIZE] = "\0";
                  strncpy(groupNamebuff, &buffer[6], BUF_SIZE - 6);
                  char groupName[BUF_SIZE] = "\0";
                  strncpy(groupName, groupNamebuff, (strstr(groupNamebuff, " : ") - groupNamebuff));
                  char membreListe[BUF_SIZE] = "\0";
                  int debutListe = 9 + strlen(groupName);
                  strncpy(membreListe, &buffer[debutListe], BUF_SIZE - debutListe);
                  char *delim = ", ";
                  unsigned count = 0;
                  char *token = strtok(membreListe,delim);

                  Client membres[MAX_MEMBRES];

                  while(token != NULL)
                  {
                     for(int k = 0; k<actual;k++)
                     {
                        if(strcmp(clients[k].name, token) == 0)
                        {
                           membres[count] = clients[k];
                           count++;
                        }
                     }
                     token = strtok(NULL,delim);
                  }
                  create_groupe(membres, groupName, count);
                  action = true;
               }
               if((strstr(buffer, "Add to ") - buffer == 0)&&(!action))
               {
                  char groupNamebuff[BUF_SIZE] = "\0";
                  strncpy(groupNamebuff, &buffer[7], BUF_SIZE - 7);
                  char groupName[BUF_SIZE] = "\0";
                  strncpy(groupName, groupNamebuff, (strstr(groupNamebuff, " : ") - groupNamebuff));
                  char membreName[BUF_SIZE] = "\0";
                  int debutListe = 10 + strlen(groupName);
                  strncpy(membreName, &buffer[debutListe], BUF_SIZE - debutListe);
                  for(int k = 0; k<actual;k++)
                  {
                     if(strcmp(clients[k].name, membreName) == 0)
                     {
                        add_groupe_member(clients[k], groupName);
                     }
                  }
                  action = true;
               }
               if((strstr(buffer, "Remove from ") - buffer == 0)&&(!action))
               {
                  char groupNamebuff[BUF_SIZE] = "\0";
                  strncpy(groupNamebuff, &buffer[12], BUF_SIZE - 12);
                  char groupName[BUF_SIZE] = "\0";
                  strncpy(groupName, groupNamebuff, (strstr(groupNamebuff, " : ") - groupNamebuff));
                  char membreName[BUF_SIZE] = "\0";
                  int debutListe = 15 + strlen(groupName);
                  strncpy(membreName, &buffer[debutListe], BUF_SIZE - debutListe);
                  for(int k = 0; k<actual;k++)
                  {
                     if(strcmp(clients[k].name, membreName) == 0)
                     {
                        delete_groupe_member(clients[k], groupName);
                     }
                  }
                  action = true;
               }
               if(!action)
               {
                  send_message_to_all_clients(clients, client, actual, buffer, 0);
               }
               break;
            }
         }
      }
   }

   clear_clients(clients, actual);
   free_history(history);
   free(history);
   end_connection(sock);
}

static void clear_clients(Client *clients, int actual)
{
   int i = 0;
   for (i = 0; i < actual; i++)
   {
      closesocket(clients[i].sock);
   }
}

static void remove_client(Client *clients, int to_remove, int *actual)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*actual)--;
}

static void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   if (from_server == 0)
      {
         strncpy(message, sender.name, BUF_SIZE - 1);
         strncat(message, " : ", sizeof message - strlen(message) - 1);
      }
   strncat(message, buffer, sizeof message - strlen(message) - 1);
   add_to_history(history,message);
   for (i = 0; i < actual; i++)
   {
      //puts(strncat(strncat(sender.name, " : ", sizeof sender.name - strlen(sender.name) - 1), buffer, sizeof sender.name - strlen(sender.name) + 3));
      /* we don't send message to the sender */
      if (sender.sock != clients[i].sock)
      {
         write_client(clients[i].sock, message); 
      }
   }
   puts(message);
}

static void send_message_to_a_group(Client *clients, Client sender, int actual, const char *buffer, const char *groupName, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if(sender.sock != clients[i].sock)
      {
         if(from_server == 0)
         {
            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " to ", sizeof message - strlen(message) - 1);
            strncat(message, groupName, sizeof message - strlen(message) - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(clients[i].sock, message);
      }
   }
   puts(message);
}

static void send_message_to_a_client(Client *clients, Client sender, Client receiver, int actual, const char *buffer, char from_server)
{
   int i = 0;
   char message[BUF_SIZE];
   char serverMessage[BUF_SIZE];
   message[0] = 0;
   serverMessage[0] = 0;
   for (i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if (receiver.sock == clients[i].sock)
      {
         if (from_server == 0)
         {
            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         strncat(serverMessage, "Pour ", BUF_SIZE-1);
         strncat(serverMessage, receiver.name, BUF_SIZE-1);
         strncat(serverMessage, " de ", BUF_SIZE-1);
         strncat(serverMessage, message, BUF_SIZE-1);
         write_client(clients[i].sock, serverMessage);
      }
   }
   puts(serverMessage);
}

static void create_groupe(Client *clients, const char *buffer, int nbMembre)
{
   Groupe groupe;
   memcpy(groupe.membres, clients, MAX_MEMBRES*sizeof(Client));
   strcpy(groupe.name, buffer);
   groupe.actualMembre = nbMembre;
   groupes[actualGroupe] = groupe;
   actualGroupe++;
   char message_acceuil[BUF_SIZE];
   message_acceuil[0]=0;
   strcpy(message_acceuil, "Nouveau Groupe ");
   strcat(message_acceuil, groupe.name);
   strcat(message_acceuil, " composé de");
   for(int i = 0; i<nbMembre; i++)
   {
      strcat(message_acceuil, " ");
      strcat(message_acceuil, groupe.membres[i].name);
   }
   for(int i = 0; i<nbMembre; i++)
   {
      write_client(groupe.membres[i].sock, message_acceuil);
   }
   puts(message_acceuil);
}
static void add_groupe_member(Client client, const char *buffer)
{
   for(int j = 0; j<actualGroupe; j++)
   {
      if(strcmp(groupes[j].name, buffer) == 0)
      {
         groupes[j].membres[groupes[j].actualMembre] = client;
         groupes[j].actualMembre ++;

         char message_acceuil[BUF_SIZE];
         message_acceuil[0]=0;
         strcpy(message_acceuil, client.name);
         strcat(message_acceuil, " a rejoint le groupe ");
         strcat(message_acceuil, groupes[j].name);
         strcat(message_acceuil, " composé de");
         for(int i = 0; i<groupes[j].actualMembre; i++)
         {
            strcat(message_acceuil, " ");
            strcat(message_acceuil, groupes[j].membres[i].name);
         }
         for(int i = 0; i<groupes[j].actualMembre; i++)
         {
            write_client(groupes[j].membres[i].sock, message_acceuil);
         }
         puts(message_acceuil);
      }
   }
}
static void delete_groupe_member(Client client, const char *buffer)
{
   for(int j = 0; j<actualGroupe; j++)
   {
      if(strcmp(groupes[j].name, buffer) == 0)
      {
         Client newMembres[MAX_MEMBRES];
         int newActualMembre = 0;
         for(int k = 0; k<groupes[j].actualMembre; k++)
         {
            if(strcmp(groupes[j].membres[k].name, client.name) != 0)
            {
               newMembres[newActualMembre] = groupes[j].membres[k];
               newActualMembre++;
            }
         }
         memcpy(groupes[j].membres, newMembres, MAX_MEMBRES*sizeof(Client));
         groupes[j].actualMembre = newActualMembre;

         char message_acceuil[BUF_SIZE];
         message_acceuil[0]=0;
         strcpy(message_acceuil, client.name);
         strcat(message_acceuil, " a quitté le groupe ");
         strcat(message_acceuil, groupes[j].name);
         strcat(message_acceuil, " composé de");
         for(int i = 0; i<groupes[j].actualMembre; i++)
         {
            strcat(message_acceuil, " ");
            strcat(message_acceuil, groupes[j].membres[i].name);
         }
         for(int i = 0; i<groupes[j].actualMembre; i++)
         {
            write_client(groupes[j].membres[i].sock, message_acceuil);
         }
         puts(message_acceuil);
      }
   }
}


static int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = {0};
   if (sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if (bind(sock, (SOCKADDR *)&sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   if (listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_client(SOCKET sock, char *buffer)
{
   int n = 0;

   if ((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   return n;
}

static void write_client(SOCKET sock, const char *buffer)
{
   if (send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

int main(int argc, char **argv)
{
   init();

   app();

   end();

   return EXIT_SUCCESS;
}

#ifndef SERVER_H
#define SERVER_H

#ifdef WIN32

#include <winsock2.h>

#elif defined(linux) || defined(__APPLE__)

#include <arpa/inet.h>
#include <netdb.h> /* gethostbyname */
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h> /* close */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

#error not defined for this platform

#endif

#define CRLF "\r\n"
#define PORT 1977
#define MAX_CLIENTS 100
#define MAX_GROUPES 10
#define MAX_MEMBRES 10

#define BUF_SIZE 1024

#include "client2.h"

typedef struct {
  char name[BUF_SIZE];
  Client membres[MAX_MEMBRES];
  int actualMembre;
} Groupe;

static void init(void);
static void end(void);
static void app(void);
int write_history(const char *content);
int read_history(char *file);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(SOCKET sock, char *buffer);
static void write_client(SOCKET sock, const char *buffer);
static void send_message_to_all_clients(Client *clients, Client client,
                                        int actual, const char *buffer,
                                        char from_server);
static void send_message_to_a_group(Client *clients, Client sender, int actual,
                                    const char *buffer, const char *groupName,
                                    char from_server);
static void send_message_to_a_client(Client *clients, Client client,
                                     Client clientReception, int actual,
                                     const char *buffer, char from_server);
static void create_groupe(Client *clients, const char *buffer, int nbMembre);
static void add_groupe_member(Client client, const char *buffer);
static void delete_groupe_member(Client client, const char *buffer);
static void remove_client(Client *clients, int to_remove, int *actual);
static void clear_clients(Client *clients, int actual);
static void send_usage_to_client(SOCKET sock);

#endif /* guard */

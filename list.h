/*
 * Chat Server with select
 */
#ifndef list_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for read/write/close
#include <sys/select.h>
#include <sys/types.h>

#define freeIfNotNull(x) \
    if ((x) != NULL)     \
    {                    \
        free(x);         \
    } //freeMalloc

typedef struct list_node
{
    char data[4096];
    char senderName[9];
    int socket;
    struct list_node *next;
} list_node;

typedef struct list
{
    list_node *head;
    list_node *tail;
} list;

typedef struct client
{
   int socket;
   struct client* next;
}client;

typedef struct clientList
{
    client *head;
    client *tail;
} clientList;

clientList* clientListInit();
void addClient(clientList* listClient, int socket);
void freeclient(client *node);
void freeClientList(clientList *list);
void deleteClient(clientList* listClient, int socket);
list *listInit();
void freeList(list *list);
list_node *createNewNode(int senderFd, char *data, int toSocket);
void freeNode(list_node *node);
void addNewMessage(list *mainList, list_node *newNode);
void addMessageAfterRead(list* mainMessageList, clientList* listClient, char* buf, int senderFd);
int listIsEmpty(list* mainList);
void writeToClient(list *listsMessage, fd_set *writeSet);
void printListMessage(list *listsMessage);
void printClientList(clientList *list);

#endif

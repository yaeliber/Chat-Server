/*
 * Chat Server with select
 */

#include "list.h"

clientList *clientListInit()
{
    clientList *newClientList = (clientList *)malloc(sizeof(clientList));
    if (newClientList == NULL)
        return NULL;
    newClientList->head = NULL;
    newClientList->tail = NULL;
    return newClientList;
}

void addClient(clientList *listClient, int socket)
{
    client *newClient = (client *)malloc(sizeof(client));
    if (newClient == NULL)
        return;
    //memset(newClient, '\0', sizeof(client));
    newClient->socket = socket;
    newClient->next = NULL;
    if (listClient->head == NULL)
    {
        listClient->head = newClient;
        listClient->tail = newClient;
        return;
    }
    else
    {
        listClient->tail->next = newClient;
        listClient->tail = newClient;
        return;
    }
}

void freeclient(client *node)
{
    freeIfNotNull(node);
}

void freeClientList(clientList *list)
{
    if (list == NULL)
        return;
    client *temp;
    while (list->head != NULL)
    {
        temp = list->head;
        list->head = list->head->next;
        freeclient(temp);
    }
    freeIfNotNull(list);
}

void deleteClient(clientList *listClient, int socket)
{
    client *temp = listClient->head;
    client *prev = NULL;
    client *current;
    while (temp != NULL)
    {
        current = temp;
        temp = temp->next;
        if (current->socket == socket)
        {
            if (prev == NULL)
            {
                listClient->head = listClient->head->next;
            }
            else
            {
                prev->next = temp;
            }

            if (current == listClient->tail)
                listClient->tail = prev;
            freeclient(current);
            return;
        }
        else
        {
            prev = current;
        }
    }
}

list *listInit()
{
    list *newList = (list *)malloc(sizeof(list));
    if (newList == NULL)
        return NULL;
    newList->head = NULL;
    newList->tail = NULL;
    return newList;
}

void freeList(list *list)
{
    if (list == NULL)
        return;
    list_node *temp;
    while (list->head != NULL)
    {
        temp = list->head;
        list->head = list->head->next;
        freeNode(temp);
    }
    freeIfNotNull(list);
}

list_node *createNewNode(int senderFd, char *data, int toSocket)
{
    list_node *newNode = (list_node *)malloc(sizeof(list_node));
    sprintf(newNode->senderName, "guest%d: ", senderFd); //?
    strcpy(newNode->data, data);
    newNode->next = NULL;
    newNode->socket = toSocket;
    return newNode;
}

void freeNode(list_node *node)
{
    freeIfNotNull(node);
}

void addNewMessage(list *mainList, list_node *newNode)
{
    if ((mainList == NULL) || (newNode == NULL))
        return;
    if (mainList->head == NULL)
    {
        mainList->head = newNode;
        mainList->tail = newNode;
        return;
    }

    else
    {
        mainList->tail->next = newNode;
        mainList->tail = newNode;
        return;
    }
}

void addMessageAfterRead(list *mainMessageList, clientList *listClient, char *buf, int senderFd)
{
    client *client1 = listClient->head;
    while (client1 != NULL)
    {
        if (client1->socket != senderFd)
        {
            addNewMessage(mainMessageList, createNewNode(senderFd, buf, client1->socket));
        }
        client1 = client1->next;
    }
}

int listIsEmpty(list *mainList)
{
    if (mainList->head == NULL)
        return 1;
    return 0;
}

void writeToClient(list *listsMessage, fd_set *writeSet)
{
    char buf[4096 + 9];
    list_node *tempNode = listsMessage->head;
    list_node *prev = NULL;
    list_node *current;
    while (tempNode != NULL)
    {
        current = tempNode;
        if (FD_ISSET(current->socket, writeSet))
        {
            printf("Server is ready to write to socket %d\n", current->socket);
            //printListMessage(listsMessage);
            strcpy(buf, current->senderName);
            strcat(buf, current->data);
            write(current->socket, buf, strlen(buf));
            if (prev == NULL)
            {
                listsMessage->head = tempNode->next;
                tempNode = tempNode->next;
                freeNode(current);
            }
            else
            {
                prev->next = tempNode->next; //Change order??
                prev = prev->next;
                tempNode = tempNode->next;
                freeNode(current);
            }
        }
        else
        {
            if (prev == NULL)
            {
                prev = tempNode;
                tempNode = tempNode->next;
                prev->next = NULL;
            }
            else
            {
                prev->next = tempNode;
                prev = prev->next;
                tempNode = tempNode->next;
                prev->next = NULL;
            }
        }
    }
}

void printListMessage(list *listsMessage)
{
    if (listsMessage == NULL)
        return;
    list_node *message = listsMessage->head;
    printf("list:\n");
    while (message != NULL)
    {
        printf("client%d %s", message->socket, message->data);
        message = message->next;
    }
}

void printClientList(clientList *list)
{
    if (list == NULL)
        return;
    client *client1 = list->head;
    printf("list:\n");
    while (client1 != NULL)
    {
        printf("client %d\n", client1->socket);
        client1 = client1->next;
    }
}

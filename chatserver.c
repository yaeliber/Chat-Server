/*
 * Chat Server with select
 */
#include "list.h"
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>

void signalHandler(int signum);

//global variables
clientList *clients;
list *messages;

int main(int argc, char **argv)
{
    signal(SIGINT, signalHandler);
    int port;
    port = atoi(argv[1]);
    if ((argc != 2) || (port <= 0))
    {
        perror("Error Using"); //
        exit(EXIT_FAILURE);
    }

    int mainSocket; //welcome socket
    struct sockaddr_in srv;

    mainSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mainSocket == -1)
    {
        perror("Error on socket");
        exit(EXIT_FAILURE);
    }

    //fill the server details
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY; //HTONL?
    srv.sin_port = htons(port);

    if (bind(mainSocket, (struct sockaddr *)&srv, sizeof(srv)) < 0) //|| == -1 its failure
    {                                                               //bind the port to the socket
        perror("Error on bind");
        exit(EXIT_FAILURE);
    }

    if (listen(mainSocket, 5) < 0)
    { //define the server to listen on the requests port
        perror("Error on listen");
        exit(EXIT_FAILURE);
    }

    clients = clientListInit();
    messages = listInit();
    if ((clients == NULL) || (messages == NULL))
    {
        perror("Error on malloc");
        exit(EXIT_FAILURE);
    }
    int newClient;
    int rc;
    char buf[4096];       //size?
    fd_set rfd, wfd, cfd; //read, write and copy
    FD_ZERO(&rfd);
    FD_ZERO(&wfd);
    FD_SET(mainSocket, &rfd);
    while (1)
    {
        cfd = rfd;
        wfd = rfd;
        if (listIsEmpty(messages) == 1)
            FD_ZERO(&wfd);
        if (select(getdtablesize(), &cfd, &wfd, NULL, NULL) < 0) //Wait as long as there is nothing to read
        {
            freeClientList(clients);
            freeList(messages);
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(mainSocket, &cfd))
        {
            newClient = accept(mainSocket, NULL, NULL);
            if (newClient > 0)
            {
                FD_SET(newClient, &rfd);
                addClient(clients, newClient);
                //printClientList(clients);
            }
            else
            {
                perror("Error on accept");
                exit(EXIT_FAILURE); ///?
            }
        }
        client *client1 = clients->head;
        client *next;
        while (client1 != NULL)
        {
            next = client1->next;
            if (FD_ISSET(client1->socket, &cfd))
            {
                printf("server is ready to read from socket %d\n", client1->socket);
                rc = read(client1->socket, buf, 4096);
                if (rc == 0)
                {
                    close(client1->socket);
                    FD_CLR(client1->socket, &rfd);
                    deleteClient(clients, client1->socket);
                    //printClientList(clients);
                }
                else if (rc < 0)
                {
                    freeClientList(clients);
                    freeList(messages);
                    perror("Error on read");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    buf[rc] = '\0';
                    //add messages
                    addMessageAfterRead(messages, clients, buf, client1->socket);
                    //printListMessage(messages);
                }
            }
            client1 = next;
        }
        writeToClient(messages, &wfd);
    }
    return (EXIT_SUCCESS);
}

void signalHandler(int signum)
{
    freeClientList(clients);
    freeList(messages);
    exit(EXIT_SUCCESS);
}

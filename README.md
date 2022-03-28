README

 Chat Server with select

===Description ===

Submitted files:
        chatserver.c- The server that handles the chat between the clients.
        list.h - The header file for the list.c implementation.
        list.c - A library implementing a linked list.
		README.txt 	- The readme file for the exercise.

The program is working according to a specific input:
		<port>	- indicates the port the server will listen to. input should be a positive integer larger than 1024.

Server termination:
			
			To terminate the server, use the SIGINT signal (^C) and wait for it to finish. any other form of
			termination will result in memory leak and/or corruption.

Clients disconnection:
		
			The server will handle a client disconnection through SIGPIPE signal sent after the client disconnects.
			


		
			



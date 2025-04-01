#ifndef DEC_SERVER
#define DEC_SERVER 

#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>



// This is the declaration.

void handle_shutdown(int sig);

void bind_to_port(int socket, int portNum);

int catch_signal(int sig, void (*handler)(int));

int open_listener_socket();

int read_in(int socket, char* buf, int len);

int sendThisdataToTheClient(int socket, char* s);

int findKeySize(char* theKey);

int findDataSize(char* theData); 

int listenAndBindToSocket(int listener_d, int portNum); 

char *decryptData(char *returnedEncryptedData, char *keyFile);

#endif



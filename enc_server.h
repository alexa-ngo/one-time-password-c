#ifndef ENC_SERVER
#define ENC_SERVER

#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define LETTER_OF_A 65
#define TOTAL_CHAR_OF_ALPHABET 26

// This is the implementation of the encryption server.

void handle_shutdown(int sig);

char *dataLongerThanOrEqualToKey(char *theData, int lengthOfData, char *theKey, int lengthOfKey);			

char *keyLongerThanData(char *theData, int lengthOfData, char *theKey, int lengthOfKey);			

void bind_to_port(int socket, int portNum);

int catch_signal(int sig, void (*handler)(int));

int open_listener_socket(); 

int read_in(int socket, char *buf, int len);

void handle_shutdown(int sig); 

int sendThisdataToTheClient(int socket, char *s);

int findKeySize(char *theKey);

int findDataSize(char *theData);

char *startEncryption(char *theData, char *theKey);

int listenAndBindToSocket(int portNum);

#endif

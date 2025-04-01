#ifndef ENC_CLIENT
#define ENC_CLIENT

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024

// This is the declaration of the encryption client.

// Usage: ./client.out <127.0.0.1> <8888> <dataString>
int main(int argc, char* argv[]);

#endif

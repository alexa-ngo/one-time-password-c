//#include <arpa/inet.h>
//#include <signal.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <sys/socket.h>
//#include <sys/stat.h>
//#include <sys/types.h>
#include <unistd.h>
#include "dec_server.h"

#define LETTER_OF_A 65
#define TOTAL_CHAR_OF_ALPHABET 26

// This is the implementation. 

void handle_shutdown(int sig) {
	fprintf(stderr, "Bye!\n");
	exit(0);
}	

void bind_to_port(int socket, int portNum) {
	struct sockaddr_in name;
	name.sin_family = PF_INET;
	name.sin_port = (in_port_t)htons(portNum); 
	name.sin_addr.s_addr = htonl(INADDR_ANY);	

	// Reuse the port and allow for us to rebind
	int reuse = 1;
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1) {
		fprintf(stderr, "Can't set the reuse option on the socket");	
		exit(1);
	}

	// Bind to a port
	int c = bind(socket, (struct sockaddr *) &name, sizeof(name));
	if (c == -1) {
		fprintf(stderr, "Can't bind to port.");
		exit(1);
	}
}

int catch_signal(int sig, void (*handler)(int)) {
	struct sigaction action;
	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	return sigaction (sig, &action, NULL);
}

int open_listener_socket() {
	int s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == -1) {
		fprintf(stderr,"Can't open socket");
		exit(1);
	}
	return s;
}

int read_in(int socket, char* buf, int len) {

	char* s = buf;
	int slen = len;
	int c = recv(socket, s, slen, 0);
	while ((c > 0) && (s[c-1] != '\n')) {
		s += c;
		slen -= c;
		c = recv(socket, s, slen, 0);
	}
	if (c < 0) {
		return c;
	} else if (c == 0) {
		buf[0] = '\0';
	} else {
		s[c-1] ='\0';
	}
	return len - slen;
}

int sendThisdataToTheClient(int socket, char* s) {
	int result = send(socket, s, strlen(s), 0);
	
	if (result  == -1) {
		fprintf(stderr, "Error in sending\n");
		exit(1);
	}
	// Return 0 if sending the data is successful
	return 0;
}

int findKeySize(char* theKey) {

	struct stat keyResult;
	int keySize;
	int keySizeResultCode = stat(theKey, &keyResult);
	
	if (keySizeResultCode == 0) {
		// We subract 1 to account for the NULL character
		keySize = keyResult.st_size - 1;
	}
	
	if (keySize == -1) {
		fprintf(stderr, "There isn't data inside the key file\n");
		exit(1);
	}
	
	return keySize;
}

int findDataSize(char* theData) {

	// The data is subtracted by 1 to account for the terminating character
	int dataSize = strlen(theData) - 1;

	// The dataSize is 0 even when there is no data.
	if (dataSize == -1) {
		fprintf(stderr, "There isn't data sent from the client\n");
		exit(1);
	}
	
	return dataSize;
} 

int listenAndBindToSocket(int listener_d, int portNum) {

	// Listen and bind to a socket
	listener_d = open_listener_socket();
	bind_to_port(listener_d, portNum);

	// Listen to the connection
	// The 10 right here represents the queue depth
	if (listen(listener_d, 10) == -1) {
		fprintf(stderr, "Can't listen\n");
		exit(1);
	}
	return listener_d;
}

char *decryptData(char *returnedEncryptedData, char *keyFile) {

	// The data size is one less
	int dataSize = findDataSize(returnedEncryptedData);
	int keySize = findKeySize(keyFile);

	char dataBuffer[dataSize + 1];
	char keyBuffer[keySize + 1];
	int idxOfData = 0;
	int idxOfKey = 0;
	
	// Open and read the data in the key file
	FILE* key;
	key = fopen(keyFile, "r");

	if (key == NULL) {
		fprintf(stderr, "The key.txt does not exist.");
		exit(1);
	}

	// fgets needs to have a plus one in the keySize length
	char* currKey = fgets(keyBuffer, keySize + 1, key); 

	// Stores the decrypted string
	char *outputMallocedDecrytedString = malloc(sizeof(char) * dataSize);

	for (int i = 0; i < dataSize + 1; i++) {
		int dataValue = returnedEncryptedData[i] - LETTER_OF_A;
		idxOfKey = idxOfData % keySize;
		int keyValue = currKey[idxOfKey] - LETTER_OF_A;
		int decCharValue = dataValue - keyValue;
	
		if (decCharValue < 0) {
			decCharValue = decCharValue + TOTAL_CHAR_OF_ALPHABET;
	
		}
		
		char c = (char)(decCharValue + LETTER_OF_A);
		outputMallocedDecrytedString[idxOfData] = c;
		idxOfData++;
	}
	return outputMallocedDecrytedString;
}





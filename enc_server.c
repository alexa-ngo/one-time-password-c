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

// This program encrypts a data string where
// the letter A has the value of 0, and the letter Z has the value of 25.
// The characters of a key are also converted to integers ranging from 0 to 25. 
// We will add the integer of each character of the data.txt with each character of the key. 
// If the sum of the character of the data and key is greater than 25, we will 
// use the mod operator of 26 to wrap around either the string of the data or the key to encrypt.

// The client should be activated with: ./client_a.out 127.0.0.1 PORT_NUM_123
// The server should be activated with: ./server_a.out PORT_NUM_123 key.txt


// Encrypt the data. If the data is longer than the key, loop through the key 	
char* dataLongerThanOrEqualToKey(char* theData, int lengthOfData, char* theKey, int lengthOfKey) {			
	char* outputMallocedDataStr = malloc(sizeof(char) * lengthOfData);
	
	int idxOfData = 0;
	int idxOfKey = 0;
	for (int i = 0; i < lengthOfData; i++) {
	
		int dataValue = theData[i] - LETTER_OF_A;
		idxOfKey = idxOfData % lengthOfKey;
		int keyValue = theKey[idxOfKey] - LETTER_OF_A;
		int encCharValue = dataValue + keyValue;
		
		if (encCharValue >= TOTAL_CHAR_OF_ALPHABET) {
			encCharValue = encCharValue % TOTAL_CHAR_OF_ALPHABET;
		}
		
		// If the encrypted value is greater than 26, roll over the character
		char c = (char)(encCharValue + LETTER_OF_A);
		
		// Assign each character to the malloc'd string
		outputMallocedDataStr[idxOfData] = c;	
		idxOfData++;
	}
	return outputMallocedDataStr;
}

// Encrypt the data. If the data is longer than the key, loop through the key 	
char* keyLongerThanData(char* theData, int lengthOfData, char* theKey, int lengthOfKey) {			
		
	char *outputMallocedKeyStr = malloc(sizeof(char) * lengthOfKey);

	int idxOfData = 0;
	int idxOfKey = 0;

	for (int i = 0; i < lengthOfKey - 1; i++) {
		
		int keyValue = theKey[i] - LETTER_OF_A;

		idxOfData = idxOfKey % lengthOfData;
		int dataValue = theData[idxOfData] - LETTER_OF_A;
		int encCharValue = dataValue + keyValue;
		
		// If the encrypted value is greater than 26, roll over the character
		if (encCharValue >= TOTAL_CHAR_OF_ALPHABET) {
			encCharValue = encCharValue % TOTAL_CHAR_OF_ALPHABET;
		}
	
		// If the encrypted value is greater than 26, roll over the character
		char c = (char)(encCharValue + LETTER_OF_A);
		
		// Assign each character to the malloc'd string
		outputMallocedKeyStr[idxOfData] = c;	
		idxOfKey++;
	}
	return outputMallocedKeyStr;
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

// This function wraps around the receive() function and
// reads in the data sent from the socket.
// This function also adds a null terminator at the end of the string.
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

// If someone hits Ctrl-C while the server is running, the function will 
// close the socket before the program ends
int listener_d;

void handle_shutdown(int sig) {
	if (listener_d) {
		close(listener_d);
	}
	fprintf(stderr, "Bye!\n");	
	exit(0);
}	

// Sends the data to the client
int sendThisdataToTheClient(int socket, char* s) {
	int result = send(socket, s, strlen(s), 0);
	if (result  == -1) {
		fprintf(stderr, "Error in sending\n");
		exit(1);
	}
	// Return 0 if sending the data is successful
	return 0;
}

// Returns the number of characters in the result
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

// Finds the number of characters in the result
int findDataSize(char* theData) {
	
	int dataSize = strlen(theData) ;
	
	// The dataSize is 0 when the there isn't any data sent from the client.
	if (dataSize == 0) {
		fprintf(stderr, "There isn't data sent from the client\n");
		exit(1);
	}
	
	return dataSize;
} 

// Encrypts the message sent from the client 
char* startEncryption(char* theData, char* theKey) {

	// Open and read the data in the key file
	FILE* key;
	key = fopen(theKey, "r");

	if (key == NULL) {
		fprintf(stderr, "The key file does not exist");
		exit(1);
	}

	int dataSize = findDataSize(theData);
	int keySize = findKeySize(theKey);
	
	char dataBuffer[dataSize + 1];
	char keyBuffer[keySize + 1];
	
	// fgets needs to have a plus one in the keySize length
	char* currKey = fgets(keyBuffer, keySize + 1, key); 
	
	// Stores the retured data
	char *returnedEncData;

	if (dataSize < keySize) {
		returnedEncData = keyLongerThanData(theData, dataSize, currKey, keySize);
		return returnedEncData;
	}

	if (keySize <= dataSize) {
		returnedEncData = dataLongerThanOrEqualToKey(theData, dataSize, currKey, keySize);	
		return returnedEncData;
	}
	return returnedEncData;
	
	free(returnedEncData);
	
	fclose(key);
}

void listenAndBindToSocket(int portNum) {

	// Listen and bind to a socket
	listener_d = open_listener_socket();
	bind_to_port(listener_d, portNum);

	// Listen to the connection
	// The 10 right here represents the queue depth
	if (listen(listener_d, 10) == -1) {
		fprintf(stderr, "Can't listen\n");
		exit(1);
	}
}


int main(int argc, char* argv[]) {

	if (argc < 3){
		fprintf(stderr, "Example usage: ./a.out PORT_NUM key.txt\n");
		exit(1);
	}
	
	// Error if the key.txt file does not exist
	char* keyFile = argv[2];
	if (keyFile == NULL) {
		fprintf(stderr, "The key.txt file does not exist\n");
		exit(1);
	}

	// Calls the handle_shutdown
	if (catch_signal(SIGINT, handle_shutdown)) {
		fprintf(stderr, "Can't set the interrupt handler\n");
		exit(1);
	}

	int portNum = atoi(argv[1]);
	listenAndBindToSocket(portNum);

	struct sockaddr_storage client_addr;
	unsigned int address_size = sizeof(client_addr);
	
	puts("Waiting for the connection\n");			
	
	// Buffer used to read in the data
	char buf[255];
	
	char* returnedEncryptedData;
	
	// Loop to accept connections
	while(1) {

		// Accept the connections
		int connect_d = accept(listener_d, (struct sockaddr *)&client_addr, &address_size);
		
		if (connect_d == -1) {
			fprintf(stderr, "Can't open secondary socket");
			exit(1);
		}

		// Forks a process
		int childPid = fork();
		if (childPid == -1) {
			fprintf(stderr, "Could not fork the child\n");
			exit(1);
		}

		if (childPid > 0) {
			printf("Handling connection by forking request to child PID: %d\n", childPid); 
		} else  {
			// Close the listening descriptor
			close(listener_d);

			// Read in the data
			read_in(connect_d, buf, sizeof(buf));		
		
			// Encrypt the data
			returnedEncryptedData = startEncryption(buf, keyFile);	
		
			// Send this data back to the client
			sendThisdataToTheClient(connect_d, returnedEncryptedData);	
				
			// Free the pointer before closing the socket
			free(returnedEncryptedData);	
			
			// The child should close and then exit and not the server
			close(connect_d);
			exit(0);
		}
	}
	// Close the listener descriptor for the parent
	close(listener_d);
	return 0;
};




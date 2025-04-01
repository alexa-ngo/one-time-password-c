#include <arpa/inet.h>
#include "enc_server.h"

// This program encrypts a data string where
// the letter A has the value of 0, and the letter Z has the value of 25.
// The characters of a key are also converted to integers ranging from 0 to 25. 
// We will add the integer of each character of the data.txt with each character of the key. 
// If the sum of the character of the data and key is greater than 25, we will 
// use the mod operator of 26 to wrap around either the string of the data or the key to encrypt.

// The client should be activated with: ./client_a.out 127.0.0.1 PORT_NUM_123
// The server should be activated with: ./server_a.out PORT_NUM_123 key.txt

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
	
	int listener_d = listenAndBindToSocket(portNum);

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




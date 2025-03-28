#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024

// This is a decryption client that takes a hostname, port number, and data as a 
// command-line argument. The client will use the socket to connect and send.
// NOTE: The server assumes that there is a /r/n

// Usage: ./client.out <127.0.0.1> <8888> <dataString>

int main(int argc, char* argv[]) {	

	if (argc != 4) {
		fprintf(stderr, "Usage: <hostname> <port> <data>");
		exit(1);
	}
	
	const char *hostname = argv[1];
	const char *port = argv[2];
	const char *dataStr = argv[3]; 

	struct addrinfo hints;
	struct addrinfo *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int status = getaddrinfo(hostname, port, &hints, &res);
	if (status != 0) {
		fprintf(stderr, "getaddinfo error: %s\n", gai_strerror(status));
		exit(1);
	}

	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd == -1) {
		perror("socket");
		exit(1);
	}

	if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		perror("connect");
		close(sockfd);
		exit(1);
	}

	freeaddrinfo(res);

	// The server requires an "\r\n" to be sent along with the data
	char *newstr = malloc(sizeof(char) * (strlen(dataStr) + 2));
	strcpy(newstr, dataStr);
	strcat(newstr, "\n");

	ssize_t bytes_sent = send(sockfd, newstr, strlen(newstr), 0);
	
	if (bytes_sent == -1) {
		perror("send");
		close(sockfd);
		exit(1);
	}	
	
	char buffer[MAX_BUFFER_SIZE]; 

	ssize_t bytes_received = recv(sockfd, buffer, MAX_BUFFER_SIZE - 1, 0);

	if (bytes_received == -1) {
		perror("recv");
		close(sockfd);
		exit(1);
	}	
	

	// Add a null terminating character
	buffer[bytes_received] = '\0';

	// Print the decrypted result
	printf("%s\n", buffer);

	// Free the new string before closing out the socket
	free(newstr);	
	
	close(sockfd);
	
	return 0;
}

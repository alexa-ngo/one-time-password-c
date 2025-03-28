#include <stdio.h>
#include <stdlib.h>

// This is a program that will generate a key based on the key length that we input.
// Example usage: ./keygen keyLength

int main(int argc, char* argv[]) {

	if (argc < 2) {
		printf("Example usage: ./keygen keyLength\n");
		exit(1);
	}
	
	// Input the file from argv[1];
	int keyLength = atoi(argv[1]);
	char* resultStr = malloc(sizeof(char) * keyLength);

	int letterA = 65;
	int letterZ = 90;

	for (int i = 0; i < keyLength; i++) {
		int randInt = rand() % (letterZ - letterA + 1) + 65;
		char x = (char)(randInt);
		resultStr[i] = x;
	}
	printf("%s\n", resultStr);
	return 0;
}

# One Time Password (OTP) Authentication Project

This One Time Password (OTP) authentication project implements an encryption and decryption client-server model written in C.

## How This Project Works:

This OTP project has 5 programs containing an encryption client, encryption server, decryption client, decryption server, and a key generator.

### Key Generator Program

A random key can be generated using the keyGenerator program. The key generator program will generate a key based on the key length that we input.

### Example signature for the key gen program:
`$ ./keygen keyLength`

### Example of using key gen:
`$ ./keygen.a.out 8`

will generate a string of NWLRBBMQ

After that we can pipe the result out to a key.txt file to use as the key.
`$ ./keygen.a.out 8 > key.txt`

### To Encrypt a String: 

The encryption client sends the hostname, port number, and string to the encryption server to encrypt. Once the encryption server receives that request, it reads and encrypts that string. The encryption server then sends the encrypted result string back to the encryption client.

### Example signature for the encryption client:
`$ ./client.a.out 127.0.0.1 portNum inputString` 

### Example of an encrytion client sending a string:
The encryption client sends: 
`$ ./client.a.out 127.0.0.1 18181 HELLO`
and if the key file contained: WOWZA
The encryption server would respond back to the client with: DSHKO 

We would do something similar to decrypt a string.

### To Decrypt a String: 

The decryption client sends the hostname, port number, and an encrypted string to the decryption server to decrypt. The decryption server would read, decrypt the string, and sends the decrypted result string back to the decryption client.

### Example signature for the encryption server:
`$ ./server.a.out portNum key.txt`

### Example of a decryption client sending a string:
The decryption client sends: 
`$ ./dec-client.a.out 127.0.0.1 1218 DSHKO`
and if the key file contained: WOWZA
The encryption server would respond back to the client with: HELLO


# To Run this Program You'll Need: 

-the string we want to encrypt and decrypt: The format of the string that we want to encrypt and decrypt should not have any spaces and must be in all uppercase characters
-key file: a key file containing one string that serves as both the encryption and decryption key 
-two terminals opened where one terminal will be the client, and the second terminal is the server.

Yep, happy encrypting and decrypting! :)

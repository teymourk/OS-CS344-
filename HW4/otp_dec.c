#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>

#define MAX_SIZE 100000


//Function prototype
void sendFile(int sock, char *file);
void getFile(int sock, char *file);
int validateFilesLength(char *text, char *key);
int validateFile(char *file, int length);

int main(int argc, char *argv[])
{

	//variables initialized
	struct sockaddr_in server_address;
	struct hostent *local_Server;

	int socketFD;
	int portNum;
	int file;
	int errorFlag;
	int n;
	char buffer[MAX_SIZE];

	//error handler for incorrent number of arguments
	if(argc < 4)
	{
		printf("Error: Incorrect number of arguments\n");
		exit(1);
	}

	//set ups the conncetion and port for the socket
	portNum = atoi(argv[3]);
	socketFD = socket(AF_INET, SOCK_STREAM, 0);

	//error if it fails
	if(socketFD < 0) {
		printf("Error opening socket.\n");
		exit(1);
	}

	//gets the hostname 
	local_Server = gethostbyname("localhost");

	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(portNum);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	memset(buffer, 0, sizeof(buffer));
	n = read(socketFD, buffer, 9);

	//Error connecting to server
	if(connect(socketFD, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
	{
		printf("ERROR connecting\n");
		close(socketFD);
		exit(1);
	}

	//error handling for wrong server.
	if(strcmp(buffer, "otp_dec_d") != 0)
	{
		printf("Error: Attempt connection wrong server.\n");
		exit(1);
	}

	//error handler to checks the key length ot see if its longer than file lenght
	file = validateFilesLength(argv[1], argv[2]);
	if(file < 0)
	{
		printf("Error: Key Too Short\n");
		close(socketFD);
		exit(1);
	}

	//sends the file and waits for the next file
	sendFile(socketFD, argv[1]);

	memset(buffer, 0, sizeof(buffer));
	n = read(socketFD, buffer, MAX_SIZE);

	sendFile(socketFD, argv[2]);

	memset(buffer, 0, sizeof(buffer));
	n = read(socketFD, buffer, MAX_SIZE);

	memset(buffer, 0, sizeof(buffer));
	getFile(socketFD, buffer);

	printf("%s\n", buffer);

	//closes the socket
	close(socketFD);

	return 0;
}

//Checks to make sure the key is lonfer than the file.
int validateFilesLength(char *text, char *key)
{
	//variables iniialized 
	struct stat txt, ky;
	stat(text, &txt);
	stat(key, &ky);

	//compares the sized and if its longer then returns -1
	if(txt.st_size > ky.st_size) {
		return -1;
	}

	else {
		return ky.st_size;
	}
}

int validateFile(char *file, int length)
{
	//checks to see that the file only contains capita letters 
	//and spaces
	int i;

	for(i = 0; i < length; i++)
	{
		if(file[i] != 32 && file[i] != 10 && !(file[i] >= 65 && file[i] <= 90))
			return 0;
	}

	return 1;
}
	
void sendFile(int sock, char *file)
{
	//variables initialized
	int valid; 
	int fd;
	int send_bytes; 
	int send_count;
	int send_file_size;
	char buffer[MAX_SIZE];

	memset(buffer, 0, MAX_SIZE);

	send_count = 0;
	
	//opens the files
	FILE *fp = fopen(file, "r");
	if(fp < 0)
	{
		printf("Error opening file\n");
		close(sock);
		exit(1);
	}
	
	//reads the lined and stores it in the buffer and 
	//validates the contents recieved/
	fgets(buffer, MAX_SIZE, fp);
	send_file_size = strlen(buffer);
	valid = validateFile(buffer, send_file_size);
	//error for invalid character
	if(valid == 0)
	{
		printf("Error: file includes invalid char\n");
		close(sock);
		exit(1);
	}
	
	//it loops and sends file and stops when it hits the same file size
	while(send_count < send_file_size)
	{
		send_bytes = send(sock, buffer, (send_file_size - send_count), 0);

		//erorr handling for failed send
		if(send_bytes < 0)
		{
			printf("Error sending file \n");
			exit(1);
		}
	send_count += send_bytes;
	}

	//close file
	fclose(fp);
}

void getFile(int sock, char *file)
{
	//variable initilized
	int recv_bytes;
	int total_bytes = 0;
	char buffer[MAX_SIZE];
	char delimiter = '\0';

	//it receives the file and loops
	//until it hits null
	do
	{	
		recv_bytes = recv(sock, file, MAX_SIZE, 0);

		//error for receiving file
		if(recv_bytes < 0)		
		{	
			printf("Reciever Error\n");
			close(sock);
			exit(1);
		}

		total_bytes += recv_bytes;
	}while(file[total_bytes] != delimiter);
}

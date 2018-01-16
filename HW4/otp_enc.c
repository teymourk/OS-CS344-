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

//Function Prototype
int validateFilesLength(char *text, char *key);
int validateFile(char *file, int length);
void sendFile(int sock, char *file);
void getFile(int sock, char *file);

int main(int argc, char *argv[])
{

	//variables initialized
	int sockfd;
	int portno;
	int fileSize;
	int errorFlag;
	int n;

	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[MAX_SIZE];

	//argument error
	if(argc < 4)
	{
		printf("Error: Incorrect number of arguments\n");
		exit(1);
	}


	portno = atoi(argv[3]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//error for socket opeening
	if(sockfd < 0)
	{
		printf("Error opening socket.\n");
		exit(1);
	}
	//gets the host name
	server = gethostbyname("localhost");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//error connection to host
	if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("ERROR connecting\n");
		close(sockfd);
		exit(1);
	}

	fileSize = validateFilesLength(argv[1], argv[2]);

	//error for key file smaller than text
	if(fileSize < 0)
	{
		printf("Error: Key file is smaller than text file.\n");
		close(sockfd);
		exit(1);
	}

	memset(buffer, 0, sizeof(buffer));
	n = read(sockfd, buffer, 9);

	//error to see if connection to wrong server.
	if(strcmp(buffer, "otp_enc_d") != 0)
	{
		printf("ERROR: Client is attempting to connect to wrong server\n");
		exit(1);
	}

	//sends the file and waits to send the other files
	sendFile(sockfd, argv[1]);

	memset(buffer, 0, sizeof(buffer));
	n = read(sockfd, buffer, MAX_SIZE);

	sendFile(sockfd, argv[2]);

	memset(buffer, 0, sizeof(buffer));
	n = read(sockfd, buffer, MAX_SIZE);

	memset(buffer, 0, sizeof(buffer));
	getFile(sockfd, buffer);

	printf("%s\n", buffer);

	close(sockfd);

	return 0;
}


//checks to see that the key is longetr than the file
int validateFilesLength(char *text, char *key)
{
	//variable initialized
	struct stat txt, ky;
	stat(text, &txt);
	stat(key, &ky);

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
			return -1;
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
	char delimiter = '\n';
	memset(buffer, 0, MAX_SIZE);

	send_count = 0;

	//opens the file
	FILE *fp = fopen(file, "r");
	if(fp < 0)
	{
		printf("Error opening file\n");
		close(sock);
		exit(1);
	}

	fgets(buffer, MAX_SIZE, fp);
	send_file_size = strlen(buffer);
	valid = validateFile(buffer, send_file_size);
	//error for invalid character
	if(valid < 0)
	{
		printf("Error: file includes invalid char\n");
		close(sock);
		exit(1);
	}

	//it loops and sends file and stops when it hits the same file size
	while(send_count < send_file_size)
	{
		send_bytes = send(sock, buffer, (send_file_size - send_count), 0);

		//error handling for failed send
		if(send_bytes < 0)
		{
			send_bytes = 0;
			printf("Error sending file \n");
			close(sock);
			exit(1);
		}
	send_count += send_bytes;
	}
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
			recv_bytes = 0;
			printf("recv error\n");
			close(sock);
			exit(1);
		}

		total_bytes += recv_bytes;
	}while(file[total_bytes] != delimiter);
}

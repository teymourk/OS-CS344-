#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define MAX_SIZE 100000

//Function protoypes
void getFile(int sock, char *file);
void sendFile(int sock, char *file);
char *encryptMessage(char *message, char *key);

int main(int argc, char *argv[])
{
	//variable initialized
	int connectNo = 0;
	int sockfd;
	int newsockfd;
	int portno;
	int pid;
	int n;
	
	struct sockaddr_in serv_addr, cli_addr;
	char key[MAX_SIZE], txtFile[MAX_SIZE];
	char *encryptFile = malloc(sizeof(char) * MAX_SIZE);

	socklen_t clilen;

	//argument error handling
	if(argc < 2)
	{
		printf("ERROR: no port provided.\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	//error for socket oppening
	if(sockfd < 0) {

		printf("ERROR opening socket\n");
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	//error binding
	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("Error on binding\n");
		exit(1);
	}

	listen(sockfd, 5);
	clilen = sizeof(cli_addr);

	//accepts connection and then forks
	while(connectNo < 10)
	{

		//accepts mew connections and creates child processes.
		newsockfd = accept(sockfd,  (struct sockaddr *) &cli_addr, &clilen);
		if(newsockfd < 0) {
			printf("Error on accept\n");
		}

		pid = fork();
		if(pid < 0) {
			printf("Error on fork\n");
		}

		if(pid == 0)
		{
			
			close(sockfd);

			//checks the communication
			n = write(newsockfd, "otp_enc_d", 9);

			getFile(newsockfd, txtFile);
			getFile(newsockfd, key);
			memset(encryptFile, 0, MAX_SIZE);
		
			//enctypts the file
			encryptFile = encryptMessage(txtFile, key);
			sendFile(newsockfd, encryptFile);	
			exit(0);
		}

	//close connection
	close(newsockfd);
	connectNo++;
	}
	close(sockfd);
	return 0;
}

void getFile(int sock, char *file)
{

	//variable initilized
	int recv_bytes;
	int total_bytes = 0;
	char buffer[MAX_SIZE];
	char delimiter = '\0';


	//loops through untill hits null
	do
	{	

		recv_bytes = recv(sock, file, MAX_SIZE, 0);
		if(recv_bytes < 0)		
		{	
			recv_bytes = 0;
			//error message for receiving
			printf("receive error\n");
			close(sock);
			exit(1);
		}

		total_bytes += recv_bytes;
	}while(file[total_bytes] != delimiter);

	recv_bytes = write(sock, "got file", 8);
}

char *encryptMessage(char *message, char *key)
{
	//variable initialized
	int cipherNum;
	int messageNum;
	int keyNum;
	int i;
	int count;

	//gets the length of the message and stores it
	int cipherLen = strlen(message);	
	
	char *ciphertext = malloc(sizeof(char) *cipherLen);
	
	//loops through until it hits new line
	for(i = 0; message[i] != '\n'; i++)
	{
		//if a letter is a space it is assigned the number 27
		if(message[i] == ' ') {
			messageNum = 26;
		}

		//it is assigned a number based of it's position
		else{
			messageNum = message[i] - 65;
		}
		
		if(key[i] == ' '){
			keyNum = 26;
		}
	
		else {
			keyNum = key[i] - 65;
		}
		
		//add the messageNum by the key
		cipherNum = (messageNum + keyNum);

		if(cipherNum > 27) {
			cipherNum -= 27;
		}
	
		//if it is equal to 27, assign it the space
		if(cipherNum == 27) {
			ciphertext[i] = ' ';
		}
		
		else {
			ciphertext[i] = (cipherNum - 1) + 'A';
		}

		if(ciphertext[i] == '@') {
			ciphertext[i] = 'A';
		}
		count++;
	}

	return ciphertext;
}	

void sendFile(int sock, char *file)
{
	//variable initialized
	int send_bytes;
	int send_count;
	int send_file_size;

	send_count = 0;

	send_file_size = strlen(file);

	//loops through until its compeltly sent
	while(send_count < send_file_size)
	{
		send_bytes = send(sock, file, (send_file_size - send_count), 0);
		if(send_bytes < 0)
		{
			//error handling for sent
			printf("Error sending file \n");
			exit(1);
		}
	send_count += send_bytes;
	}
}

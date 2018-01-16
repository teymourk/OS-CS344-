#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define MAX_SIZE 100000

//Function Prototypes
void getFile(int sock, char *file);
void sendFile(int sock, char *file);
char *decryptMessage(char *encrypt, char *key);

int main(int argc, char *argv[])
{

	//initializing the variables and the socket struct
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t clilen;

	int connectNo = 0,
	int sockfd;
	int newsockfd;
	int portno;
	int pid;
	int n;
	
	char key[MAX_SIZE], txtFile[MAX_SIZE];
	char *messageFile = malloc(sizeof(char) * MAX_SIZE);

	//Error on no port number entered
	if(argc < 2) {

		printf("ERROR: no port provided.\n");
		exit(1);
	}

	//Sets up the socket and gets the information
	//form the port
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(sockfd < 0) {
		printf("ERROR opening socket\n");
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("Error on binding\n");
		exit(1);
	}

	listen(sockfd, 5);
	clilen = sizeof(cli_addr);

	//Loops through all the connections and then forks
	while(connectNo < 10)
	{

		//As it find the connections it accepts them and puts them in the 
		//Child process.
		newsockfd = accept(sockfd,  (struct sockaddr *) &cli_addr, &clilen);
		if(newsockfd < 0){
			printf("Error on accept\n");
		}

		pid = fork();
		if(pid < 0) {
			printf("Error on fork\n");
		}

		//Child process
		if(pid == 0) {
			close(sockfd);

			//checks to see if the server is communicating with the right client
			n = write(newsockfd, "otp_dec_d", 9);

			//recievies the files and then decrypts the messagge and passes
			//it to the client.
			getFile(newsockfd, txtFile);
			getFile(newsockfd, key);
			memset(messageFile, 0, MAX_SIZE);
		
			messageFile = decryptMessage(txtFile, key);
			sendFile(newsockfd, messageFile);	
			exit(0);
		}

	//Close the connection
	close(newsockfd);
	connectNo++;
	}
	close(sockfd);
	return 0;
}

//Gets the file from the clinet
void getFile(int sock, char *file)
{
	int recv_bytes, total_bytes = 0;
	char buffer[MAX_SIZE];
	char delimiter = '\0';


	//Loops throught until there are no more 
	//characters and hit null
	do
	{	
	
		recv_bytes = recv(sock, file, MAX_SIZE, 0);
		if(recv_bytes < 0)		
		{	
			printf("recv error\n");
			close(sock);
			exit(1);
		}


		total_bytes += recv_bytes;
	}while(file[total_bytes] != delimiter);

	//sends confirmation of the file recieved to the client
	recv_bytes = write(sock, "got file", 8);
}


char *decryptMessage(char *encrypt, char *key)
{

	//variable initialized
	int messageNum;
	int encryptNum;
	int keyNum;
	int i;
	int count;

	//find the length of the text that was decrypted
	int cipherLen = strlen(encrypt);	
	
	char *message = malloc(sizeof(char) *cipherLen);

	//loops trhought intill it hits a new line 
	for(i = 0; encrypt[i] != '\n'; i++)
	{

		//if its a space is assigns 21 to it
		if(key[i] == ' ') {
			keyNum = 27;
		}
		
		//assings based on its postion 
		else {
			keyNum = key[i]  - 64;
		}

		if(encrypt[i] == ' ') {
			encryptNum = 27;
		}
	
		else {
			encryptNum = encrypt[i] - 64;
		}
		
		//Subtracts the encrypted number by the key 
		messageNum = (encryptNum - keyNum) + 1;

		//if its negative number loops back throught the alphabet	
		if(messageNum < 0) {
			messageNum += 27;
		}

		//if its greater than 36 then its assings space
		if(messageNum >= 26) {
			message[i] = ' ';
		}

		else{
			message[i] = (messageNum) + 'A';
		}
	}

	return message;
}	

//functuon to send file to client.
void sendFile(int sock, char *file)
{

	//variables initialized
	int send_bytes;
	int send_count;
	int send_file_size;

	send_count = 0;

	send_file_size = strlen(file);

	//loops throught all the files until its completelt sent
	while(send_count < send_file_size)
	{
		send_bytes = send(sock, file, (send_file_size - send_count), 0);
		if(send_bytes < 0)
		{
			send_bytes = 0;
			printf("Error sending file \n");
			exit(1);
		}
	send_count += send_bytes;
	}
} 


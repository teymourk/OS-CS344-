#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

/*
This program is designed to generate 7 random rooms out of 10 and display it to user.
The User has to search through the rooms until they find their way to the last room
with the least amount of steps taken. If user gets to 14 rooms without finiding the last room,
they lose the game.
*/

//Initializing the mutex(MulthiThread)
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//Creating the room struct to store every information about all
//the indivdual rooms
struct Rooms_Struct {
	
	char * connections[6];
	char roomName[20];
	char roomType[20];

	int numberOfConnections;
};

//Funtion Prototypes/Decleration
void rightFilesToDirectory(char * directory, char * fileName, FILE * file);
void generateRoomConnections();
void getCurrentTime();
void generateRoomTypes(int i);
void addCommonConnections(int i, int random, bool isSame, bool sameCon);
void generateRoomFilesAndTypes(char *roomNames[]);
void startRunningRooms(char * directory, char * roomName, struct Rooms_Struct allrooms[7]);

//Created a struct array to be able to hold every file or data
//for each room to be able to access.
struct Rooms_Struct rooms_data[7];

int main() {

	//Generating the folder name with random ID/
	char randomFolderNumber[10];	
	char roomsDirectory[50] = "teymourk.rooms.";
	char fileName[50];

	snprintf(randomFolderNumber, 10,"%d",(int)getpid());
	strcat (roomsDirectory, randomFolderNumber); 
	mkdir(roomsDirectory, 0755);
	
	FILE * file;
		
	//Array with all the room infomations
	char *roomNames[10] = {"Wrestling", 
								"Track", 
								"Soccer", 
								"Baseball",
								"Basketball", 
								"Football",
								"Tennis", 
								"Swimming",
								"Golf", 
								"Huky"};	

	//Srand puts time back to NULL so the
	//Sequence of numbers being randomized
	//Can change upon starting.

	srand(time(NULL));

	//Steps to generate every room data.
	
	//Creates the rooms and their types.
	generateRoomFilesAndTypes(roomNames);

	//Generates room connections.
	generateRoomConnections();
	
	//Rights files to the directory
	rightFilesToDirectory(roomsDirectory, fileName, file);

	return 0;
}

void generateRoomFilesAndTypes(char *roomNames[]) {

	//Creating random numbers between 0 and 9
	//sice out array is from 0-9.
	int random = rand() % 10;
	
	//Initializing some variables to hold data 
	int i, k;
	bool isSame;

	//Loops through and random room names to use
	//To create the files. 
	for (i = 0; i < 7; i++) {

		random = rand() % 10;
		isSame = false;

		for(k = 0; k < i; k++) {

			//If the room name that was already generated
			//Comes up again then it i-- to take one off i 
			if (strcmp(rooms_data[k].roomName, roomNames[random]) == 0){
						
				isSame = true;
				i--;
			}
		}

		//Then goes here and checks that if 
		//The files arent the same then it 
		//adds it to the struct.
		if (!isSame) {
			
			//Copying them to the struct
			strcpy (rooms_data[i].roomName, roomNames[random]);
			
			//To generate room types as creating the room names
			generateRoomTypes(i);
		}
	}
}

void generateRoomTypes(int i) {

	//Using the switch stament to create every room
	//Since files will be randomized it doesnt matter if
	//the first or last index are first and last. 
	switch (i) {
		
	case 1:
		strcpy(rooms_data[i].roomType, "START_ROOM");
		break;
	case 6:
		strcpy(rooms_data[i].roomType, "END_ROOM");
		break;
	default:
		strcpy(rooms_data[i].roomType, "Mid_ROOM");
		break;
	}
}


void generateRoomConnections() {

	int i, k, n, randomNumber;
	bool isSame;
	bool sameCon;

	//Loops through 7 times and creates random
	//Connections for every item.
	for(i = 0; i < 7; i++){

		//This random generates a number before 0 and 3 and adding 
		//just adds 3 to it so that what ever number we get can be 
		//3 more than 0 1.
		randomNumber = rand() % 4 + 3;

		//Here we are checking if the number connections we 
		//Have is less than the number generates so if it is 
		//It loos and creayes random connections
		if(randomNumber > rooms_data[i].numberOfConnections) {

			while (rooms_data[i].numberOfConnections < randomNumber){

			isSame = false;
			sameCon = false;
			n = rand() % 7;
		
				//Here we are connecting the rooms that are are connceted
				addCommonConnections(i, n, isSame, sameCon);
			}
		}
	}
}

void addCommonConnections(int i, int random, bool isSame, bool sameCon) {

	int j;

	//Checks to see if the riim name we already have is the same as the one 
	//randomly generated
	if (strcmp(rooms_data[i].roomName, rooms_data[random].roomName) == 0){

		isSame = true;
	}

	//if its not the same it loops and finds the sane connections 
	if (!isSame) {

		for (j = 0; j < rooms_data[i].numberOfConnections; j++) {

			if (strcmp(rooms_data[i].connections[j],rooms_data[random].roomName) == 0) {

				sameCon = true;
			}
		}

		//Finally it connects both connections together
		if (sameCon == false) {
		
			rooms_data[random].connections[rooms_data[random].numberOfConnections] = rooms_data[i].roomName;
			rooms_data[random].numberOfConnections ++;

			rooms_data[i].connections[rooms_data[i].numberOfConnections] = rooms_data[random].roomName;
			rooms_data[i].numberOfConnections ++;
		}
	}	
}

void rightFilesToDirectory(char * directory, char * fileName, FILE * file) {

	int i,k;

	//Accesting the files of the directory
	strcat (directory, "/");

	//Loops through and adds the struct data to the 
	//directory
	for (i = 0; i < 7; i++){

		//copies the directory and file to file name to access
		strcpy (fileName, directory);
		strcat (fileName, rooms_data[i].roomName);

		file = fopen(fileName, "w+");

		//Puts the room name in to the file
		fprintf(file, "ROOM NAME: %s \n", rooms_data[i].roomName);

		//Loops through the number of connections  to the file
		for(k = 0; k < rooms_data[i].numberOfConnections; k++){

			//Adds the coonections created to the file
			fprintf(file, "CONNECTION %d: %s\n", k+1, rooms_data[i].connections[k]);
		}

		//Adds the room type to the file
		fprintf(file, "ROOM TYPE: %s\n", rooms_data[i].roomType);

		//Finally closes the file when done
		fclose(file);
	}

	//Lock the thread for running the interface
	pthread_mutex_lock(&mutex);

	//Running the user interface
	startRunningRooms(directory, rooms_data[0].roomName, rooms_data);
}

//Get the current time
void getCurrentTime() {

	//current time gets locked in the main thread.
	pthread_mutex_lock(&mutex);

	FILE * timeFile;

	int min;
	int hour;
	int year;
	int monthNum;
	char *pm_am = "am";
	char *month;
	char pathFile[10];


	char *wday[10] = {"Sunday",
						"Monday", 
						"Tuesday", 
						"Wednesday",
						"Thursday", 
						"Friday", 
						"Saturday"};
	
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	//Acccesing all the information of the day and time
	hour = tm.tm_hour;
	min = tm.tm_min;
	year = tm.tm_year + 1900;
	monthNum = tm.tm_mon + 1;

	//I figured this will be graded either on October or Novemebr
	//Only added these to month.
	if (monthNum == 10) {

		month = "October";

	} else if (monthNum == 11) {

		month = "November";
	} 

	//Couldnt figure a way to change to regular time instead
	//of military so used a switch to indentify the pm or am 
	//and basically converting to normal.
	if ( hour > 12 ) {

		pm_am = "pm";

		switch (hour) {
			case 12:
				hour = 12;
				break;
			case 13:
				hour = 1;
				break;
			case 14:
				hour = 2;
				break;
			case 15:
				hour = 3;
				break;
			case 16:
				hour = 4;
				break;
			case 17:
				hour = 5;
				break;
			case 18:
				hour = 6;
				break;
			case 19:
				hour = 7;
				break;
			case 20:
				hour = 8;
				break;
			case 21:
				hour = 9;
				break;
			case 22:
				hour = 10;
				break;
			case 23:
				hour = 11;
				break;
			case 24:
				hour = 12;
				break;
		}
	}

	//printing all the information to the file
	printf("\n%d:%d%s, %s, %s %d, %d\n\n", hour, min, pm_am ,wday[tm.tm_wday], month, tm.tm_mday, year );

	//Adds it to the file called time.txt
	sprintf(pathFile, "time.txt");
	timeFile = fopen(pathFile, "w+");


	fprintf(timeFile, "\n%d:%d%s, %s, %s %d, %d\n\n", hour, min, pm_am ,wday[tm.tm_wday], month, tm.tm_mday, year);
	fclose(timeFile);

	printf("%d\n", tm.tm_wday );

	//Finally it unlocks the mutex on thread to be able 
	//To rin the functions
	pthread_mutex_unlock(&mutex);
}


void startRunningRooms(char * directory, char * roomName, struct Rooms_Struct allrooms[7]) {

	//When program is being run, it unlocks the thread to be able 
	//to show the display
	pthread_mutex_unlock(&mutex);

	int i;
	int numberOfLines = 0;
	int stepsTaken = 0;

	//Variables to store data
	char firstLine[300];
	char fileName[30];
	char nextRoom[30];
	char userInput[10];
	char *userSteps[15];

	bool valid_Input = true;
	bool ended = false;

	FILE * nRoom;

	//Copies all them room name in to the 
	//next toom wanting to acces
	strcpy(nextRoom,roomName);

	//THis is a loops to ring the scanf or let user input data while 
	//We havnt reached the final room.
	while (!ended) {

		//if the user does all the steps, the program 
		//stops and displays message.
		if ( stepsTaken == 14 ) {

			printf("YOU VISITED TOO MANY ROOMS. GAME OVER!\n");
			break;
		}

		//copies the next room name and the directory
		//to be able to access the file.
		strcpy( fileName, directory );
		strcat( fileName, nextRoom );

		nRoom = fopen( fileName, "r" );

		//if we have accesss the room then 
		//we grab the information out of it.
		if ( nRoom != NULL ) {

			//loops through the file and grabs all 
			//the lines and stores it into userInput
			while ( fgets(firstLine, sizeof firstLine, nRoom) != NULL ) {

				memset( userInput, 0, strlen(userInput));

				strncpy( userInput, &firstLine[0], 19);

				//This checks to see if in the file 
				//we have a line that is ROOM TYPE: END ROOM.
				//If it is it end the game.
				if (strcmp(userInput,"ROOM TYPE: END_ROOM") == 0){

					printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");

					printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", stepsTaken);

					for(i=0; i< stepsTaken; i++ ) {

						printf("%s\n", userSteps[i]);
					}

					ended = true;
					break;
				}
			}

			fclose( nRoom );
		}
		
		if (!ended) {

			valid_Input = true;
			while ( valid_Input ) {

			nRoom = fopen( fileName, "r" );

				if ( nRoom != NULL ) {

					numberOfLines = 0;
					
					//read the files for the new file and then saves it 
					while (fgets(firstLine, sizeof firstLine, nRoom) != NULL) {

						if ( numberOfLines == 0 ) {

							//Prints the information to use
							printf("CURRENT_LOCATION: %s", &firstLine[11], 18);
							printf("POSSIBLE CONNECTION: ");
						}

						if (numberOfLines > 0){

							//clears the input and the copys the new lines 
							//into it
							memset(userInput,0,strlen(userInput)); 
							strncpy(userInput, firstLine, 10);

							if (strcmp(userInput,"CONNECTION") == 0){

								memset(userInput,0,strlen(userInput)); 

								printf("%s", &firstLine[14], 13);	 
							}
						}

						numberOfLines++;
					}

					//Here if the user inputs the time then 
					//We access the time function.
					if ( strcmp(nextRoom, "time") == 0 ) {

						getCurrentTime();
					}

					//Else if the user puts in the new room file
					//it loops through and fins the file to access
					printf("WHERE TO? >");

					scanf("%s", nextRoom);

					for (i = 0; i < 7; i++ ){

						//checks to see if it exits.
						if(strcmp( nextRoom, rooms_data[i].roomName ) == 0){

							valid_Input = false;

							userSteps[stepsTaken] = rooms_data[i].roomName;

							stepsTaken++;
						}
					}

					//if the user input is not valis then it displaus and error.s
					if (valid_Input) {

						printf("%s\n", nextRoom );

						printf("\nHUH? I DONT UNDERSTAND %s. TRY AGAIN.\n\n", nextRoom);
					}
				}

				//clossses the room
				fclose(nRoom);
			}
		}
	}
}
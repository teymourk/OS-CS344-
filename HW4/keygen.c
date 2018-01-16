#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void gen(char *argv[]);

int main(int argc, char *argv[])
{

	//Random number generator
	srand(time(0));

	//To see if the right amoint of arguments are entered
	if(argc != 2)
	{
		printf("Error: Incorrect number of arguments\n");
		exit(1);

	} else {
		gen(argv);
	}

	return 0;
}
//Generated the number arguments created
void gen(char *argv[])
{
	int i;
	char keyCharacter;
	int key;

	//find the length of the keys and loops through all the letters 
	//in the alphabet
	int keyLength = atoi(argv[1]);

	for(i = 0; i < keyLength; i++)
	{
		key = (rand() % 27);

		if(key == 26) {
			keyCharacter = ' ';
		}
		else {
			keyCharacter = key + 'A';
		}

		printf("%c", keyCharacter); 
	}

	printf("\n");
}

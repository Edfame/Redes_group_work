#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LOCAL 2
#define TIPO 1
#define VERSAO 3
#define ID 0

//ID tipo local versão firmware

void clearArray(char string[], char fill, int length)
{
	int i = 0;
	while(i < length)
	{
		string[i]=fill;
		i++;
	}
}

void getInfo(char message[], char sensor[], int step)
{
	int i = 0;
	short commaCounter = 0;

	while(commaCounter<step)
	{
		if(sensor[i]==',')
		{
			commaCounter++;
		}
		i++;
	}

	int j = 0;

	while(sensor[i]!=',' && sensor[i]!='\0')
	{
		message[j]=sensor[i];
		j++;
		i++;
	}
}

int main()
{
	char sensor[] = {'a',',','b',',','a','c','d',',','z','\0'};
	char messageAux[20];
	clearArray(messageAux,'\0', 20);
	getInfo(messageAux,sensor,VERSAO);
	printf("%s\n", messageAux);

	return 0;
}
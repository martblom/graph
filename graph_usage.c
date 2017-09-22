#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "graph.h"
#define SIZE 100
void print(float* tal, int size)
{
	int i;
	for(i=0;i<size;i++)
		if((i)%10==0)
			printf("\n%f\t", tal[i]);
		else
			printf("%f\t", tal[i]);
	printf("\n");
}
int main()
{
	char val;
	char buffer[SIZE];
	float* tal=NULL;
	int size=100;
	do
	{
		printf("MENY\n");
		printf("1. Load data\n");
		printf("2. Display data\n");
		printf("3. Display graph\n");
		printf("0. Exit\n");
		scanf("%s", buffer);
		val = buffer[0];
		if(strlen(buffer)>1)
		{
			printf("Too many characters!\n");
		}
		else
		{
			switch(val)
			{
				case '1': 	tal = load("posneg.txt", &size);	break;
				case '2': 	print(tal, size); 					break;
				case '3': 	graph(tal, size); 					break;
				case '0': 	printf("Exiting program\n"); 		break;
				default: 	printf("Wrong input!\n"); 			break;
			}
		}
	}while(val!='0');
}
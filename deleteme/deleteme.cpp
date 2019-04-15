#include <windows.h>
#include <stdarg.h>
#include <conio.h>
#include <iostream.h>
#include <assert.h>
#include <io.h>
#include <fcntl.h>
#include <fstream.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <process.h>
#include <string.h>


char *OldFileName;
char *OldFileName2;
char *NewFileName;
char *NewFileName2;
struct _stat st;


void fcopy(char *sourcefile, char *targetfile)							// copy a file 
{
	char buffer;
	
	ifstream source(sourcefile, ios::in|ios::nocreate|ios::binary);
	if(!source)
		return;

	ofstream target(targetfile, ios::out|ios::binary);
	if(!target)
		return;
	
	while(source.get(buffer))
		target.put(buffer);
	
	source.close();
	target.close();
}


void main(void)
{
	int pf;
	FILE *DataFile = NULL;
	OldFileName="DerangedRaid.exe";
	OldFileName2="AutoUpdate.exe";
	NewFileName="copyme.exe";
	NewFileName2="copyme2.exe";
	int A_exist=0;
	int A_cp=0;


	//Check if there's something to update
	if (_stat(NewFileName,&st))
	{
		exit(1); // File does not exist
	}

	if (_stat(OldFileName2,&st))
		A_exist=0;
	else
		A_exist=1;

	if (_stat(NewFileName2,&st))
		A_cp=0;
	else
		A_cp=1;

	
	printf("Wait For Writable");
	//Wait for DerangedRaid.exe to become writable
	do 
	{
		DataFile = fopen(OldFileName,"w");
	} while (DataFile == NULL);
	fclose(DataFile);

	if (A_exist==1)
	{
		do 
		{
			DataFile = fopen(OldFileName2,"w");
		} while (DataFile == NULL);
		fclose(DataFile);
	}


	//Copy new file over the old one
	fcopy(NewFileName,OldFileName);

	//Remove new file
	pf = remove(NewFileName); 

	if (A_cp==1)
	{
		//Copy new file over the old one
		fcopy(NewFileName2,OldFileName2);
		//Remove new file
		pf = remove(NewFileName2); 
	}


	//Start game again
	_spawnlp(_P_NOWAIT,OldFileName,OldFileName,NULL);

	exit(0);
}

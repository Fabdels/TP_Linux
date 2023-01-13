#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

void withF(){
	FILE * fid = fopen("asserv.txt", "w+");
	if (fid == NULL)
	{
		printf("Error opening file\n");
		exit(-1);
	}

	fprintf(fid, "b");
	fclose(fid);
}

int withoutF(char* fileAddr, int value){
	int fileID = open(fileAddr, O_WRONLY, 0); 
	if(fileID<0){
		printf("ERROR\n");
		return -1;
	}
	write(fileID, &value, 1);
	close(fileID);
	return 0;
}

int readThat(char* fileAddr){
	int fileID = open(fileAddr, O_WRONLY, 0); 
	if(fileID<0){
		printf("ERROR\n");
		return -1;
	}
	char bufchar[4];
	read(fileID, bufchar, 4);
	close(fileID);
	//printf("dir:%d\n", bufchar[0]);
	return (int) *bufchar;
}

int charToHexa(const char* mot){
	int res=0, i=0, translate;
	while(mot[i] != 0){
		if(mot[i]>='0' && mot[i]<= '9')
			translate = mot[i] - '0';
		else if(mot[i]>='a' && mot[i]<= 'f')
			translate = mot[i] - 'a' + 10;
		res = (res << (4*i)) | translate; 
		i++;
	}
	return res;
}

int main(int argc, char const *argv[])
{	
	char ptrn, dir;
	// int i;
	// printf("argc: %d\n", argc);
	// for(i=0; i<argc;i++){
	// 	printf("%s\n", argv[i]);
	// }
	// return -1;
	
	switch(argc){
		case 3:
			ptrn = charToHexa(argv[2]);
			printf("pattern = %d\n",  ptrn);
			withoutF("/dev/ensea-led", ptrn);
		case 2:
			dir = atoi(argv[1]);
			withoutF("/proc/ensea/dir", dir);
			printf("direction = %d\n",  dir);
			break;

		default:
			printf("syntax: direction pattern\n");
			dir = readThat("/proc/ensea/dir");
			printf("direction = %d\n",  dir);
			break;
	}

	return 0;
}
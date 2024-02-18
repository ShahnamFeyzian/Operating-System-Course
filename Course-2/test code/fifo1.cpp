// C program to implement one side of FIFO
// This side writes first, then reads
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include<iostream>

int main()
{
	int fd1;

	// FIFO file path
	char myfifo[] = "myfifo";

	// Creating the named file(FIFO)
	// mkfifo(<pathname>, <permission>)
	mkfifo(myfifo, 0666);
	// fd1 = open(myfifo, O_RDWR);

	char str1[80], arr2[80];
	
	fd1 = open(myfifo, O_WRONLY);
	write(fd1, "salam\0", 6);
	close(fd1);

	std::cout << "hello fucker\n";
	fd1 = open(myfifo, O_RDONLY);
	std::cout << "hello fucker\n";
	read(fd1, str1, 80);
	std::cout << "hello fucker\n";
	printf("%s\n", str1);
	close(fd1);

	// while (1)
	// {
	// 	// Open FIFO for write only
	// 	// fd = open(myfifo, O_WRONLY);

	// 	fgets(arr2, 80, stdin);

	// 	write(fd, arr2, strlen(arr2));
	// 	// close(fd);

	// 	fgets(arr2, 80, stdin);

	// 	write(fd, arr2, strlen(arr2));
	// 	// close(fd);

	// 	// Open FIFO for Read only
	// 	// fd = open(myfifo, O_RDONLY);

	// 	// Read from FIFO
	// 	read(fd, arr1, sizeof(arr1));

	// 	// Print the read message
	// 	printf("User2: %s\n", arr1);
	// }
	return 0;
}

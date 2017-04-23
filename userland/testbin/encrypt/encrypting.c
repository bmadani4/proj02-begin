#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <err.h>

int
main(int argc, char *argv[])
{
	int fd, rv, fileS = 40;                                                         
	static char writebuf[40] = "Twiddle dee dee, Twiddle dum dum.......\n";
	unsigned long int readbuf[(fileS)/4];
	const char *file;
	

	if (argc == 0) {
		/*warnx("No arguments - running on \"testfile\"");*/
		file = "testfile";
	}
	else if (argc == 2) {
		file = argv[1];
	}
	else {
		errx(1, "Usage: filetest <filename>");
	}

	fd = open(file, O_WRONLY|O_CREAT|O_TRUNC, 0664);

	rv = write(fd, writebuf, fileS);
	if (rv<0) {
		 err(1, "%s: write", file);
	}

	rv = close(fd);
	if (rv<0) {
		err(1, "%s: close (1st time)", file);
	}
    
	encrypt(file, fileS);
	
	fd = open(file, O_RDONLY);
	if (fd<0) {
		err(1, "%s: open for read", file);
	}
	int i = 0;
	for (i=0; i<((fileS)/4);i++){
	rv = read(fd, &readbuf[i], 4);
	if (rv<0) {
		err(1, "%s: read", file);
	}}

	rv = close(fd);
	printf("\nLast 16 bytes of encrypted file \n");
	size_t neuFileS = fileS/4;
	for (i=neuFileS - 16; i<fileS/4; i++)
		printf("%lu\n",readbuf[i]);
			

	printf("File is now encrypted.\n");
	return 0;
}

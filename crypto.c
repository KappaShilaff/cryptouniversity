#include <gcrypt.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ENCR 1
#define DECR 0

void myCrypt(int encdec, const char *pass, const char *salt, const char *text,char **argv)
{
	gcry_error_t gcryError;
	gcry_cipher_hd_t hd;
	size_t i;
	size_t passLength = strlen(pass);
	size_t saltLength = strlen(salt);
	size_t textLength = strlen(text) + encdec;
	char *outBuffer = (char *) malloc(textLength);

	FILE	*fd;
	char 	*newfile;
	int 	k = 0;
	newfile = malloc(strlen(argv[4]) + 2);
	while (argv[4][k])
	{
		newfile[k] = argv[4][k];
		k++;
	}
	newfile[k++] = 'n';
	newfile[k] = '\0';
	fd = fopen(newfile, "w");
	gcryError = gcry_cipher_open(&hd,
								 GCRY_CIPHER_AES128,
								 GCRY_CIPHER_MODE_CBC,
								 GCRY_CIPHER_CBC_CTS);

	if (gcryError)
	{
		printf("gcry_cipher_open failed:  %s/%s\n",
			   gcry_strsource(gcryError), gcry_strerror(gcryError));
		return;
	}

	gcryError = gcry_cipher_setkey(hd, pass, passLength);
	if (gcryError)
	{
		printf("gcry_cipher_setkey failed:  %s/%s\n",
			   gcry_strsource(gcryError), gcry_strerror(gcryError));
		return;
	}

	gcryError = gcry_cipher_setiv(hd, salt, saltLength);
	if (gcryError)
	{
		printf("gcry_cipher_setiv failed:  %s/%s\n",
			   gcry_strsource(gcryError), gcry_strerror(gcryError));
		return;
	}

	switch (encdec)
	{
		case ENCR:
			gcryError = gcry_cipher_encrypt(hd, outBuffer, textLength, text,
											textLength);
			break;
		case DECR:
			gcryError = gcry_cipher_decrypt(hd, outBuffer, textLength, text,
											textLength);
	}
	if (gcryError)
	{
		printf("gcry_cipher_encrypt failed:  %s/%s\n",
			   gcry_strsource(gcryError), gcry_strerror(gcryError));
		return;
	}
	switch (encdec)
	{
		case ENCR:
			printf("kek\n");
			for (i = 0; i < textLength; i++)
				fprintf(fd, "%02X", (unsigned char) outBuffer[i]);
			break;
		case DECR:
			fprintf(fd,"%s\n", outBuffer);
	}
	fclose(fd);
	gcry_cipher_close(hd);
	free(outBuffer);
}

int main(int argc, char **argv)
{
	int fd;
	struct stat statistics;


//	fd = open(argv[4], O_WRONLY);

//	fd = fopen("test.txt","w");
//	fprintf(fd,"kekw\n");
//	fclose(fd);
	if (argc != 5)
	{
		printf("usage: %s <-e|-d> \"<password>\" \"<salt>\" \"<password>\"\n", argv[0]);
		return 1;
	}

	int encdec = ENCR;
	char *line;

	if (!(fd = open(argv[4], O_RDONLY)))
	{
		printf("sasuha\n");
		return (0);
	}
	if (fstat(fd, &statistics) != -1) {
		line = (char*)malloc(statistics.st_size);
	}
	read(fd, line, statistics.st_size);
	close(fd);
	if (!strcmp(argv[1], "-d"))
	{
		int i = 0;
		char a[3] = {"00"};
		for (; i < strlen(line); i += 2)
		{
			sprintf(a, "%c%c", line[i], line[i + 1]);
			line[i / 2] = strtol(a, NULL, 16);
		}
		line[i / 2 - 1] = '\0';
		encdec = DECR;
	}

	myCrypt(encdec, argv[2], argv[3], line, argv);

	return 0;
}
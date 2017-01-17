#include <openssl/sha.h>
#define size 1024

int my_sha256(char *input, char *outputBuffer)
{
	FILE *file;
	char hash[SHA256_DIGEST_LENGTH];
	char *buffer=malloc(size+1);
	int bytesRead=0;
	SHA256_CTX sha256_temp;
	
	SHA256_Init(&sha256_temp);
	if(DEBUG) printf("*** Input  ***\n");
	if((file=(strcmp(input, "stdin") == 0)?stdin:fopen(input, "rb")))
	{
		while((buffer[bytesRead++]=fgetc(file)) != EOF)
		{
			if(((bytesRead)%size)==0)
			{
				buffer[size]=0;
				SHA256_Update(&sha256_temp, buffer, bytesRead);
				if(DEBUG) printf("%s", buffer);
				bytesRead=0;
				for(int i = 0; i < size; i++) buffer[i]=0;
			}
		}
		for(int i = bytesRead-2; i < size; i++) buffer[i]=0;
		SHA256_Update(&sha256_temp, buffer, bytesRead-2);
		if(DEBUG) printf("%s", buffer);
	}
	else
	{
		SHA256_Update(&sha256_temp, input, strlen(input));
		if(DEBUG) printf("%s", input);
	}
	if(DEBUG) printf("\n");
	SHA256_Final(hash, &sha256_temp);
	for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(outputBuffer + (i * 2), "%02x", (unsigned char)hash[i]);
	}
	if(DEBUG) printf("*** hash   ***\n");
	if(DEBUG) printf("%s\n", outputBuffer);
	free(buffer);
	
	return 0;
}

#undef size

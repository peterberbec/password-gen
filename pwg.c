#include <stdio.h>
#include <stdlib.h>	// getenv()
#include <string.h>	// strcat(), strcmp(), strlen()
#include "over-five.h" 	// word list const char words[]
#define DEBUG		0
#include "mysha.h"	// defines sha256 _input, _file, _string
#undef DEBUG
// <macro>
// Bit Fiddling
#define topxbits(x,y)	(y>>(8-x))
#define botxbits(x,y)	(y&((1<<x)-1))
#define up(x,y) 	(y<<x)
// Hex to int conversions
#define hextol(x)	((x>'9')?((x>'F')?(x+10-'A'):(x+10-'a')):(x-'0'))+(16*(((*((&x)+1))>'9')?(((*((&x)+1))>'F')?((*((&x)+1))+10-'A'):((*((&x)+1))+10-'a')):((*((&x)+1))-'0')))
// Asci to int conversions
#define c2toi(x)	((x[1]=='\0')?(x[0]-'0'):((10*(x[0]-'0'))+(x[1]-'0')))
// Index functions
#define bitsa(x)	((8-(x*3)<0)?(8-(x*3)+8<0)?(8-(x*3)+16):(8-(x*3)+8):8-(x*3))
#define bitsb(x)	(11-(bitsa(x)<3?3-bitsa(x):0)-bitsa(x))
#define bitsc(x)	(bitsa(x)<3?3-bitsa(x):0)
#define index(x)	((j>2?(j>5)?4:2:0)+(j*2)+(i*22)+x)
// </macro>
#define DEBUG		0

void hash_break(char hash_in[64], unsigned int hash_out[24])
{
	for(int i=0, j=0; i<3; (j<7)?j++:(i++,j=0))
	{
		hash_out[(7*i)+j]=			topxbits(bitsa(j), hextol(hash_in[index(0)]))  +
					up(bitsa(j),	botxbits(bitsb(j), hextol(hash_in[index(2)]))) +
					up(11-bitsc(j),	botxbits(bitsc(j), hextol(hash_in[index(4)])));
	}
}

int readconfig(int *num_words, char *hash, int *password_num)
{
        FILE *config;
        char file_name[]="pwg.conf", variable[1024], value[1024], input[1024];
        int ret=0;

	if(!(config=fopen(file_name, "r"))) // first check current dir
	{
		sprintf(value, "%s/%s", getenv("HOME"), file_name);
		if(!(config=fopen(value, "r")))	// next check home dir
		{
			sprintf(value, "/etc/%s", file_name);
			if(!(config=fopen(value, "r")))	// finally check /etc
			{
		                return ret;
			}
			else
			{
				if(DEBUG) printf("CONFIG_FILE: %s\n", value);
			}
		}
		else
		{
			if(DEBUG) printf("CONFIG_FILE: %s\n", value);
		}
        }
	else
	{
		if(DEBUG) printf("CONFIG_FILE: %s\n", file_name);
	}
        while(fgets(input, 1024, config))
	{
		if(input[0] != '#') // ignore comments
                {
			sscanf(input,"%s %s", variable, value);
                        if(strcmp(variable, "words") == 0)
			{
				if(c2toi(value) > 0)
				{
                                	*num_words=c2toi(value)>24?24:c2toi(value);
					if(DEBUG) printf("CONFIG_FILE: Number of words set to %d.\n", *num_words);			
				}
				else
				{
					printf("Config file error. words value must be a number and we were given \"%s\". Defaulting to %d.\n", value, *num_words);
				}
			}
                        else if(strcmp(variable, "key") == 0)
                        {
				if(value[0]=='~')
				{
					sprintf(input,"%s/%s", getenv("HOME"), (value+2));
					sprintf(value,"%s", input);
				}
				FILE *temp;
				if(temp=fopen(value, "r"))
				{
					fclose(temp);
					my_sha256(value, hash);
					if(DEBUG) printf("CONFIG_FILE: Input to hash appended with file %s.\n", value);
					ret=1;
				}
				else
				{
					printf("Config file error. cannot open key file \"%s\"\n", value);
				}
                        }
			else if(strcmp(variable, "password_num") == 0)
			{
				if(c2toi(value) > -1)
				{
					*password_num=c2toi(value);
					if(DEBUG) printf("CONFIG_FILE: Starting at password #%d.\n", *password_num);
				}
				else
				{
					printf("Config file error. password_num value must be a number and we were given \"%s\". Defaulting to %d.\n", value, *password_num);
				}
			}
                }
	}
        fclose(config);
        
	return ret;
}

int main(int argc, char **argv)
{
	char input[130]="stdin", hash[65];
	unsigned int hash_word_nums[24], num_words=3, password_num=0;
	
	printf("Enter value to hash in to a password. CTRL-D on a newline to end input.\n");
	my_sha256(input, hash);
	if(readconfig(&num_words, input, &password_num))//>0)
	{
		strcat(input, hash);
		my_sha256(input, hash);
	}
	if(DEBUG) printf("num_words: %d\npassword_num: %d\n", num_words, password_num);
	if((password_num*num_words)>23)
	{
		printf("There are not %d passwords of length %d. Defaulting to password 0.\n", password_num, num_words);
		password_num=0;
	}
	hash_break(hash, hash_word_nums); // break hash up into 11 bit pieces (+3)
	for(int i=0;i<num_words;i++)
	{ 
		printf("%s", words[hash_word_nums[i+(password_num*num_words)]]);
	}
	printf("\n");
	return 0;
}

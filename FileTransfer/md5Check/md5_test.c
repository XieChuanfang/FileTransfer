#include "MD5.h"
 
int main(int argc, char *argv[])
{
	char filename[FILE_NAME_MAX_SIZE];
	char md5_sum[MD5_LEN];
	
	if(argc != 2)
	{
		printf("Usage: <filename> <filename>\n");   
        	return -1;    
	}

	memset(filename, 0, sizeof(filename));
	strcpy(filename, argv[1]);

	if(CalcFileMD5(filename, md5_sum))
		printf("success, md5_sum = %s\n", md5_sum);
	else 
		printf("error!\n");

	return 1;	
}

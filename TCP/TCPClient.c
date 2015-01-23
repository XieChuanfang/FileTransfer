#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define    MAXLINE        1024*128
#define    SERVER_PORT     8001  
#define FILE_NAME_MAX_SIZE 512  
static int count = 0;   

int main(int argc,char **argv)
{
    struct sockaddr_in    serv_addr;
    char                  buf[MAXLINE];
    int                    sock_id;
    int                    recv_len;
    int                    write_len;
    int                    send_len;
    FILE                  *fp;
    int                    i_ret;
    char                   file_name[FILE_NAME_MAX_SIZE]; 
    char                   client_file_name[FILE_NAME_MAX_SIZE] = "recvfile.txt"; 
    time_t                 t_start,t_end;
   
    
    if ((sock_id = socket(AF_INET,SOCK_STREAM,0)) < 0) {
        perror("Create socket failed\n");
        exit(0);
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	/* 打开文件，准备写入 */   
	fp = fopen(client_file_name, "w");    
	if(NULL == fp)    
	{      
		printf("File:\t%s Can Not Open To Write\n", client_file_name);       
		exit(1);    
	}   
	
	/* 输入文件名到缓冲区 */   
 
	bzero(file_name, FILE_NAME_MAX_SIZE);    
	printf("Please Input File Name On Server: ");    
	scanf("%s", file_name);  

  
    i_ret = connect(sock_id, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));
    if (-1 == i_ret) 
    {
        printf("Connect socket failed\n");
	return -1;
    }
    if((send_len = send(sock_id, file_name, sizeof(file_name), 0)) < sizeof(file_name))
    {
	printf("send file name error\n");
	exit(1); 
    }
	
    t_start=time(NULL);	
	while (recv_len = recv(sock_id, buf, MAXLINE, 0)) 
	{   
        	if(recv_len < 0) 
		{
           		 printf("Recieve Data From Server Failed!\n");
           		 break;
        	}
       		 printf("----------recv_len = %d, id = %d\n", recv_len, ++count);
        	write_len = fwrite(buf, sizeof(char), recv_len, fp);
        	if (write_len < recv_len) 
		{
           		 printf("Write file failed\n");
            		break;
        	}
       	 	bzero(buf, MAXLINE);
    	}
    
    t_end=time(NULL);
    printf("共用时%.0fs\n",difftime(t_end,t_start));
	
    printf("File:%s Transfer Successful!\n", file_name); 
    fclose(fp);
    close(sock_id);
    return 0;
}

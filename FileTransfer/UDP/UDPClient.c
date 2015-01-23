/*************************************************************************    
> File Name: client.c    
> Author: Chuanfang Xie   
************************************************************************/ 
#include<sys/types.h> 
#include<sys/socket.h>  
#include<unistd.h>  
#include<netinet/in.h>  
#include<arpa/inet.h>  
#include<stdio.h>  
#include<stdlib.h>  
#include<errno.h>  
#include<netdb.h>  
#include<stdarg.h>  
#include<string.h>
#include <time.h>
#include <limits.h> 

 
#define SERVER_PORT 8001 
#define BUFFER_SIZE 1024*48
#define FILE_NAME_MAX_SIZE 512  
   
/* 包头 */ 
typedef struct  
{    
	int id;    
	int buf_size;  
}PackInfo;     

/* 接收包 */ 
struct RecvPack  
{    
	PackInfo head;    
	char buf[BUFFER_SIZE];  
} data;        

int main()  
{    
	int id = 1;
	static char recvfile[] = "recvfile.txt"; 
	time_t t_start,t_end;
     
	/* 服务端地址 */   
	struct sockaddr_in server_addr;    
	bzero(&server_addr, sizeof(server_addr));    
	server_addr.sin_family = AF_INET;    
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");    
	server_addr.sin_port = htons(SERVER_PORT);    
	socklen_t server_addr_length = sizeof(server_addr);       
	/* 创建socket */   
	int client_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);    
	if(client_socket_fd < 0)   
	{      
		perror("Create Socket Failed:");      
		exit(1);    
	}       
	/* 输入文件名到缓冲区 */   
	char file_name[FILE_NAME_MAX_SIZE+1];    
	bzero(file_name, FILE_NAME_MAX_SIZE+1);    
	printf("Please Input File Name On Server: ");    
	scanf("%s", file_name);       
	char buffer[BUFFER_SIZE];    
	bzero(buffer, BUFFER_SIZE);    
	//strncpy(buffer, file_name, strlen(file_name)>BUFFER_SIZE?BUFFER_SIZE:strlen(file_name));       
	/* 发送文件名 */   
	if(sendto(client_socket_fd,  file_name, sizeof(file_name), 0, (struct sockaddr*)&server_addr, server_addr_length) < 0)    
	{      
		perror("Send File Name Failed:");      
		exit(1);   
	}      
	/* 打开文件，准备写入 */   
	FILE *fp = fopen(recvfile, "w");    
	if(NULL == fp)    
	{      
		printf("File:\t%s Can Not Open To Write\n", file_name);       
		exit(1);    
	}       
	/* 从服务器接收数据，并写入文件 */   
	int len = 0; 

	t_start=time(NULL);  
	while(1)    
	{      
		PackInfo pack_info;         
		if((len = recvfrom(client_socket_fd, (char*)&data, sizeof(data), 0, (struct sockaddr*)&server_addr, &server_addr_length)) > 0)      
		{        
			if(data.head.id == id)       
			{          
				pack_info.id = data.head.id;          
				pack_info.buf_size = data.head.buf_size;          
				++id; 
        
				/* 发送数据包确认信息 */         
				if(sendto(client_socket_fd, (char*)&pack_info, sizeof(pack_info), 0, (struct sockaddr*)&server_addr, server_addr_length) < 0)          
				{            
					printf("Send confirm information failed!");          
				}   
                                else
				{
					printf("looking forward to receive id = %d\n", id);
				}       
				/* 写入文件 */         
				if(fwrite(data.buf, sizeof(char), data.head.buf_size, fp) < data.head.buf_size)          
				{            
					printf("File:\t%s Write Failed\n", file_name);            
					break;          
				}
				else 
				{	
					fflush(fp);     //fwrite函数写到用户空间缓冲区,将内存与文件同步
					printf("write id = %d into file successfully len = %d, bufsize= %d\n", data.head.id, len, data.head.buf_size);
				}
				
			}        
			else if(data.head.id < id) /* 如果是重发的包 */      
			{          
				pack_info.id = data.head.id;          
				pack_info.buf_size = data.head.buf_size;          
				/* 重发数据包确认信息 */         
				if(sendto(client_socket_fd, (char*)&pack_info, sizeof(pack_info), 0, (struct sockaddr*)&server_addr, server_addr_length) < 0)          
				{            
					printf("Send confirm information failed!");         
				} 
				printf("lose packet id = %d\n", data.head.id);      	
			}   
			else       
			{
				printf("exit while\n");
				break;          
			}      
		}      
		else     
		{       
			printf("exit while\n"); 
			break;      
		}    
	}   

        t_end=time(NULL);
    	printf("共用时%.0fs\n",difftime(t_end,t_start));

	printf("Receive File:\t%s From Server IP Successful!\n", file_name);    
	fclose(fp);    
	close(client_socket_fd);  

	return 0;  
}
		

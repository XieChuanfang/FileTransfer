/*************************************************************************    
> File Name: server.c    
> Author: Chuanfang Xie 
> Time: 2015/1/23   
************************************************************************/ 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define    MAXLINE             1024*128
#define    SERVER_PORT         8001
#define    FILE_NAME_MAX_SIZE  512

static int count = 0;   

int main(int argc,char **argv) 
{
    struct sockaddr_in    serv_addr;
    struct sockaddr_in    clie_addr;
    char                  buf[MAXLINE];
    int                    sock_id;
    int                    link_id;
    int                    read_len;
    int                    send_len;
    int                    recv_len;
    int                    clie_addr_len;
    FILE                   *fp;
    char                   file_name[FILE_NAME_MAX_SIZE]; 

    if ((sock_id = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("Create socket failed\n");
        exit(0);
    }
  
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock_id, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Bind socket failed\n");
        exit(0);
    }

    if (-1 == listen(sock_id, 10)) {
        perror("Listen socket failed\n");
        exit(0);
    }

    while (1) {
        clie_addr_len = sizeof(clie_addr);
        link_id = accept(sock_id, (struct sockaddr *)&clie_addr, &clie_addr_len);
        if (-1 == link_id) {
            perror("Accept socket failed\n");
            exit(0);
        }
		if((recv_len = recv(link_id, file_name, FILE_NAME_MAX_SIZE, 0)) < 0) {
			printf("recv file name error\n");
			exit(1);
		}

		printf("file name: %s\n", file_name);

		if ((fp = fopen(file_name, "r")) == NULL) {
			perror("Open file failed\n");
			exit(0);
		}

		bzero(buf, MAXLINE);
		while ((read_len = fread(buf, sizeof(char), MAXLINE, fp)) >0 ) {
			send_len = send(link_id, buf, read_len, 0);
			if ( send_len < read_len ) {
				perror("Send file failed\n");
				exit(0);
			}
			
			printf("------send_len = %d, id = %d\n", send_len, ++count);
			bzero(buf, MAXLINE);
		}
		
        fclose(fp);
        close(link_id);
    }
	
    close(sock_id); 
    return 0;
}

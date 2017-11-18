#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h> 


#include "libevent.h"

int send_file(char *file)
{
    /* 判断文件是否可读 */
    // ??????

    unsigned long file_size;
    int fd = 0;
    struct stat sfile;
    char data[1024] = {0};
    
    stat(file, &sfile );
    file_size = sfile.st_size;
            
    
    fd = open(file, O_RDONLY);
    if(-1 == fd){
        printf("file open error\n");
        return 1;
    }
    //读取文件并发送
    //发送文件名
    // strcpy(pac.data, strrchr(pac.data, '/') + 1);
    // write(sock_fd, &pac, sizeof(pac));
    // memset(&pac, 0, sizeof(pac));

    //发送文件长度
    sprintf(data, "%s%ld%s", "!@##@!:",file_size,":!@##@!");
    send_content(data);

    
    memset(data, 0, sizeof(data));

    int read_len = 0;
    while((read_len = read(fd, data, 1024)) > 0){
        send_content(data);
        memset(data, 0, sizeof(data));
    }

    close(fd);

    printf("file size : %u B\n", file_size);
    printf("send file success\n");

    return 0;
}

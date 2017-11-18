#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>


static unsigned int g_file_length = 0;
static int g_file_fd = 0;
int g_file_flag = 0;

int open_file(char *file)
{
    assert(file);

    printf("file=%s\n", file);
    
    /* 打开文件 */
    g_file_fd = open(file, O_CREAT|O_WRONLY, 0777);
    if(-1 == g_file_fd){
        printf("open file error!\n");
        return 1;
    }

    return 0;
}
int wait_file()
{
    
    g_file_length = 0;
    
    /* 等待发送端发送的文件长度 */
    while (!g_file_length)
    {
        usleep(10);
    }

    /* 等待文件接收完成 */
    while (g_file_length > 0)
    {
        usleep(10);
    }

    g_file_flag = 0;
    close(g_file_fd);
    
    return 0;
}


static int save_file(char *content)
{
    assert(content);
    
    if (-1 != g_file_fd)
    {
        g_file_length -= write(g_file_fd, content, strlen(content));
    }
    
    return 0;
}

int process_recv_file(char *content)
{
    assert(content);
    if (!memcmp(content, "!@##@!:", 7))
    {
        char *p_end = NULL;
        int len = 0;

        p_end = strstr(&content[7], ":!@##@!");
        len = p_end-content;
        *p_end = 0;
        g_file_length = atoi(&content[7]);
        printf("g_file_length=%u\n", g_file_length);
        save_file(&content[len+7]);
    }
    else
    {
        save_file(content);
    }
    return 0;
}


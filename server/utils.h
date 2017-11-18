#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include<event.h>  
#include<event2/bufferevent.h>  
#include<event2/buffer.h>  
#include<event2/util.h> 

#define  SUCCESS  0
#define  FAILURE  1


/*调试信息*/
#define DEBUG(format, arguments...) \
    do{\
        printf(format, ##arguments);\
    }while(0)

/*错误信息*/    
#define ERROR(format, arguments...) \
    do{\
        printf(format, ##arguments);\
    }while(0)

/*info信息*/    
#define INFO(format, arguments...) \
    do{\
        printf(format, ##arguments);\
    }while(0)

typedef signed   char     BOOL;
typedef char              INT1;
typedef short             INT2;
typedef unsigned char     UINT1;
typedef unsigned short    UINT2;
typedef int               INT4;
typedef unsigned int      UINT4;

#define MAX_USER_NUM 100



#endif

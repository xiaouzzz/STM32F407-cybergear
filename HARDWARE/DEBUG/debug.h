#ifndef __DEBUG_H
#define __DEBUG_H

    #include <stdlib.h>
    #include <stdio.h>

    #define __DEBUG__

    #ifdef __DEBUG__
        #define DPRINTF(...) printf(__VA_ARGS__)  ///宏打印函数定义
    #else
        #define DPRINTF(...)
    #endif

#endif

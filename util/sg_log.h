/*
 * @Author: yh chen yh_chan_kanio@163.com
 * @Date: 2023-12-30 16:33:35
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2023-12-30 16:42:36
 * @FilePath: /SplitGPU/util/log.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include <assert.h>


#define SG_LOG(format, args...)     \
do                                  \
{                                   \
    printf("\033[33;33m<%s line:%d %s> \033[0m",__FUNCTION__,__LINE__,__TIME__); \
    printf(format, ##args);         \
    printf("\n");                   \
}while(0)

#define SG_ERR(format, args...)     \
do                                  \
{                                   \
    printf("\033[33;31m<%s line:%d %s> \033[0m",__FUNCTION__,__LINE__,__TIME__); \
    printf(format, ##args);         \
    printf("\n");                   \
}while(0)

/*--------------------------------------------------------------------------------*/

#ifdef RELEASE  

    #define SG_DEBUG(format, args...)  

    #define SG_ASSERT(exp)

#else  

    #define SG_DEBUG(format, args...)     \
    do                                    \
    {                                     \
        printf("\033[33;34m<%s line:%d %s> \033[0m",__FUNCTION__,__LINE__,__TIME__); \
        printf(format, ##args);           \
        printf("\n");                     \
    }while(0)

    #define SG_ASSERT(exp)      \
    do                          \
    {                           \
        assert(exp);            \
    }while(0)

#endif  
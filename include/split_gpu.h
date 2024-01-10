/*
 * @Author: yh chen yh_chan_kanio@163.com
 * @Date: 2023-12-30 16:39:17
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-04 14:11:14
 * @FilePath: /SplitGPU/include/split_gpu.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#pragma once
#include "sg_log.h"
namespace SplitGPU {

#define MB (size_t)(1<<20)
#define GB (size_t)(1<<30)
#define TIME_SLICE_UNIT 500

enum RET {
    RET_OK,
    RET_FAIL,
    RET_ERR,
};

#define CHECK_RET(ret) {if(ret!=RET_OK){ SG_ERR("ret err"); exit(-1);}}

}
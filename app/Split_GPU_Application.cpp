/*
 * @Author: yh chen yh_chan_kanio@163.com
 * @Date: 2023-12-29 17:01:56
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-01 22:59:21
 * @FilePath: /SplitGPU/app/Split_GPU_Application.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "controller.h"

int main() {
    SplitGPU::Controller* ctlr = 
    new SplitGPU::Controller(
    SplitGPU::SHARE_MEMORY,
    SplitGPU::TIME_SLICE_SCHEDULE);
    ctlr->load_clients();
    ctlr->intern_start();
    ctlr->func_start();
    return 0;
}
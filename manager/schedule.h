/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2023-12-29 17:18:53
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-22 18:44:38
 * @FilePath: /SplitGPU/include/schedule.h
 * @Description: 
 * 
 */
#pragma once

#include <atomic>
#include <cstddef>
#include <list>
#include <thread>
#include <utility>

#include "split_gpu.h"
#include "user_context.h"


namespace SplitGPU {


enum Schedule_type {
    TIME_SLICE_SCHEDULE,
};

typedef std::pair<Client_id, int> Client_weight;



class Schedule {
public:
    Schedule() {}
    virtual ~Schedule(){};
    virtual Schedule_type get_type() = 0;
    virtual RET push_user(Client_id id, int weight) = 0;
    virtual RET remove_user(Client_id id, int weight) = 0;
    virtual RET check(Client_id id, int weight) = 0;
    virtual void start() = 0;
    virtual int query() = 0;
    virtual int inline_number() = 0;
};

class Time_schedule : public Schedule {
public:
    Time_schedule();
    ~Time_schedule();
    Schedule_type get_type() override { return TIME_SLICE_SCHEDULE; }
    RET push_user(Client_id id, int weight) override;
    RET remove_user(Client_id id, int weight) override;
    RET check(Client_id id, int weight) override;
    void start() override;
    int query() override;
    int inline_number() override {
        return client_time_list.size()==client_time_table.size()?client_time_list.size():-1;
        }
private:
    const int max_clock = 100;
    size_t time_slice_unit;
    std::atomic<int> time_clock;
    std::list<Client_weight> client_time_list;
    std::map<Client_id, Client_weight&> client_time_table;
    const int max_wegiht = 100;
    int weights; 
    std::thread clock_thread;
    
};

}
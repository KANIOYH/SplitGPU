/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2023-12-29 17:04:38
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-22 19:13:20
 * @FilePath: /SplitGPU/manager/schedule.cpp
 * @Description: 
 * 
 */
#include "schedule.h"
#include "sg_log.h"
#include "split_gpu.h"
#include "user_context.h"
#include <unistd.h>
#include <utility>
#include <openssl/rand.h>  
#include <chrono>  
#include <thread> 
#include <iostream>
#include <fstream>
namespace SplitGPU {

Time_schedule::Time_schedule():time_slice_unit(TIME_SLICE_UNIT) {}
Time_schedule::~Time_schedule() {

}
RET Time_schedule::push_user(Client_id id, int weight) {
    int sum_weight = client_time_list.back().second + weight;
    if(sum_weight > max_wegiht) {
        SG_ERR("sum weight > max wegiht sum:%d", sum_weight);
        return RET_ERR;
    }
    client_time_list.push_back(Client_weight(id, sum_weight));
    client_time_table.insert(std::pair<Client_id, Client_weight&>(id,client_time_list.back()));
    weights += weight;
    return RET_OK;
}

RET Time_schedule::remove_user(Client_id id, int weight) {
    for(auto item = client_time_list.begin();item != client_time_list.end();) {
        if(item->first == id) {
            item = client_time_list.erase(item);
            client_time_table.erase( client_time_table.find(id) );
            weights -= weight;
            /*recaculate sum_weight*/
            while(item != client_time_list.end()) {
                item->second -= weight;
                item++;
            }
            return RET_OK;
        } else {
            item++;
        }
    }
    SG_LOG("no find client id in client_time list");
    return RET_ERR;
}

RET Time_schedule::check(Client_id id, int weight) {
    if(client_time_table.find(id) != client_time_table.end()) {
        Client_weight &item = client_time_table.at(id);
        int sum_weight = item.second;
        int now_clock = query();
        // if(weights > 0)
        //     now_clock %= weights;
        if( (sum_weight - weight) <= now_clock  && now_clock < sum_weight) {
            return RET_OK;
        } else {
            return RET_FAIL;
        }
    } else {
        SG_LOG("check not find,client id:%d",id);
        return RET_FAIL;
    }
}

void Time_schedule::start() {
    time_clock.store(0);
    clock_thread = std::thread([&]() {
        int temp = 0;
        while(true) {
            temp = time_clock.load();
            temp++;
            temp %= max_clock;
            time_clock.store(temp);
            std::this_thread::sleep_for(std::chrono::microseconds(time_slice_unit));
            //usleep(time_slice_unit);
        }
    });
}

int Time_schedule::query() {
    int now_clock = time_clock.load();
    // unsigned char* rand_value;
    // RAND_bytes(rand_value, sizeof(int));
    // now_clock = *((unsigned*)rand_value)%100;
    return now_clock;
}

}
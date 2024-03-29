###
 # @Author: Yamphy Chan && yh_chan_kanio@163.com
 # @Date: 2023-11-19 19:04:10
 # @LastEditors: yh chen yh_chan_kanio@163.com
 # @LastEditTime: 2024-03-25 13:21:02
 # @FilePath: /SplitGPU/script/test_cuda_start.sh
 # @Description: 
 # 

#!/bin/bash  
WORK_PATH=$(cd $(dirname $0) && pwd) && cd $WORK_PATH
process_command="$WORK_PATH/../build/test_cuda"
start=$(date +%s.%N)  
  
for ((i=0;i<=10;i++))
    do
    $process_command&  
    done
wait

end=$(date +%s.%N)  
duration=$(echo "$end - $start" | bc)  
  
echo "Took $duration seconds to complete."
 

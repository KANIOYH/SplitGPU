'''
Author: Yamphy Chan && yh_chan_kanio@163.com
Date: 2024-01-22 16:44:42
LastEditors: yh chen yh_chan_kanio@163.com
LastEditTime: 2024-02-07 12:07:14
FilePath: /SplitGPU/app/sp_watchdog.py
Description: watchdog in container

'''
import os
import signal
import socket
import threading
from time import sleep
import psutil
import requests

lock = threading.Lock()
max_process = 6
plist={}
ikey=[123,456,789,147,258,369]

def check_alive(pid):
    try:
        process = psutil.Process(pid)
        if process.is_running():
            return True
    except(psutil.NoSuchProcess,psutil.AccessDenied):
        pass
    return False

def notify(pid):
    postdata = {'pid':str(pid)}
    r = requests.get('http://0.0.0.0:8999/exit',params = postdata)
    print("notify:",pid,r.status_code)

def register(id,weight):
    data = {'id':str(id),'weight':str(weight)}
    r = requests.get('http://0.0.0.0:8999/register',params = data)
    print(id,"result:",r.status_code)

def change_weight(id,weight):
    data = {'id':str(id),'weight':str(weight)}
    r = requests.get('http://0.0.0.0:8999/changeweight',params = data)
    print(id,"result:",r.status_code)
    

def kill_all():
    for pid in plist:
        os.kill(pid, signal.SIGTERM)
    plist.clear()

def poll_process():
    
    print("polling")
    tcp_server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcp_server_socket.bind(('127.0.0.1', 8889))
    tcp_server_socket.listen(128)
 
    while True:
        new_client_socket, client_addr = tcp_server_socket.accept()
        print('now client: %s' % str(client_addr))

        recv_data = int(new_client_socket.recv(1024))
        print('client address{} client message{}'.format(client_addr[0], recv_data))
        
        if recv_data == -22:
            lock.acquire()
            kill_all()
            lock.release()
            
        pid = recv_data
        lock.acquire()
        if check_alive(pid):
            send_data = '-1'
            if pid not in plist.keys():
                for k in ikey:
                    if k not in plist.values():
                        plist[pid] = k
                        send_data = str(k)
                        break
        lock.release()
        new_client_socket.send(send_data.encode('utf-8'))
        new_client_socket.close()
        print('over')

    tcp_server_socket.close()


if __name__ == "__main__":
    t = threading.Thread(target=poll_process)
    t.start()
    while True:
        sleep(1)
        lock.acquire()
        ids = []
        for id in plist.keys():
            if not check_alive(id):
                ids.append(id)
                notify(id)
        for id in ids:
            plist.pop(id)
        lock.release()
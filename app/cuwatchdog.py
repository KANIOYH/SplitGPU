import socket
import threading
from time import sleep
import psutil
import requests

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
    r = requests.post('127.0.0.1:8888',data = postdata)
    print("notify back:",r.status_code)

def register():
    
    print("polling")
    # 1.创建tcp套接字
    tcp_server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
 
    #  2.必须选择接收时的ip和端口 (ip,端口)
    tcp_server_socket.bind(('', 8889))
 
    # 3.监听将tcp的主动连接特性转为被动连接，最大可支持128个客户端的连接
    tcp_server_socket.listen(128)
 
    while True:
        # 4.等待接收（accept）客户端的链接
        # 返回的是一个元组(一个新的tcp套接字对象，对方的ip和端口)
        new_client_socket, client_addr = tcp_server_socket.accept()
        print('当前客户端是: %s' % str(client_addr))
 
        # 5.接收对方发送的数据
        while True:
            recv_data = new_client_socket.recv(1024).decode('utf-8')
            print('当前客户端地址：{}，当前接受的信息是：{}'.format(client_addr[0], recv_data))
            pid = int(recv_data)
            if check_alive(pid):
                if pid not in plist:
                    plist.append(pid)
        new_client_socket.close()
        print('服务完毕...')
 
    # 6.关闭套接字
    tcp_server_socket.close()

plist=[]
if __name__ == "__main__":
    t = threading(target=register)
    t.start()
    while True:
        sleep(1)
        for pid in plist:
            if not check_alive(p):
                plist.remove(pid)
                notify(pid)
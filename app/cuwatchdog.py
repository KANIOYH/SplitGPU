import socket
import threading
from time import sleep
import psutil
import requests

lock = threading.Lock()
plist=[]

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
    r = requests.post('127.0.0.1:9999',data = postdata)
    print("notify:",pid)

def register():
    
    print("polling")
    # 1.创建tcp套接字
    tcp_server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
 
    #  2.必须选择接收时的ip和端口 (ip,端口)
    tcp_server_socket.bind(('127.0.0.1', 8889))
 
    # 3.监听将tcp的主动连接特性转为被动连接，最大可支持128个客户端的连接
    tcp_server_socket.listen(128)
 
    while True:
        # 4.等待接收（accept）客户端的链接
        # 返回的是一个元组(一个新的tcp套接字对象，对方的ip和端口)
        new_client_socket, client_addr = tcp_server_socket.accept()
        print('当前客户端是: %s' % str(client_addr))
 
        # 5.接收对方发送的数据

        recv_data = int(new_client_socket.recv(1024))
        print('当前客户端地址：{}，当前接受的信息是：{}'.format(client_addr[0], recv_data))
        pid = recv_data
        lock.acquire()
        if check_alive(pid):
            if pid not in plist:
                plist.append(pid)
        lock.release()
        send_data = 'A'
        new_client_socket.send(send_data.encode('utf-8'))
        new_client_socket.close()
        print('服务完毕...')
 
    # 6.关闭套接字
    tcp_server_socket.close()


if __name__ == "__main__":
    t = threading.Thread(target=register)
    t.start()
    while True:
        sleep(1)
        lock.acquire()
        for pid in plist:
            if not check_alive(pid):
                plist.remove(pid)
                notify(pid)
        lock.release()
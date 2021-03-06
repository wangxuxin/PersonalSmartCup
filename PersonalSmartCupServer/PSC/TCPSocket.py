# coding=utf-8
# 导入socket库:
import socket
import threading
import time

import Command

port = 23333
thread = 5


def tcplink(sock, addr):
    print('Accept new connection from %s:%s...' % addr)
    ip = addr[0]
    port = addr[1]
    while True:
        data = sock.recv(1024)
        time.sleep(0.01)
        echo = data.decode('utf-8')
        netcmd = echo.split(' ')
        if echo == '':
            break
        if not echo == 'keepAlive':
            print('<---' + addr.__str__() + '>' + echo)

        Command.command(netcmd, sock, addr)
        if echo == 'exit' or echo == 'stop':
            break
    sock.close()
    print('Connection from %s:%s closed.' % addr)


def server():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # 监听端口:
    s.bind(('0.0.0.0', port))

    s.listen(thread)
    print('Waiting for connection...')

    while True:
        # 接受一个新连接:
        sock, addr = s.accept()
        sock.settimeout(5)
        # 创建新线程来处理TCP连接:
        t = threading.Thread(target=tcplink, args=(sock, addr))
        t.start()


def startServer():
    serverThread = threading.Thread(target=server)
    serverThread.start()


if __name__ == '__main__':
    startServer()

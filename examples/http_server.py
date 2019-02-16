#!/usr/bin/env python3
import time
import socket
from http.server import BaseHTTPRequestHandler, HTTPServer

HOST_NAME = 'localhost'
PORT_NUMBER = 8000

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("0.0.0.0", 8001))

class MyHandler(BaseHTTPRequestHandler):
    def _set_headers(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
    def do_HEAD(self):
        self._set_headers()

    def do_GET(self):
        request = "GET " + self.path + " "
        global s
        s.send(request.encode())
        response = s.recv(1024)
        print(response.decode())
        self._set_headers()
        self.wfile.write(bytes(response.decode() + "\n", "utf8"))
        return

    def do_POST(self):
        request = "POST " + self.path + " "
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)
        print(post_data)
        request += post_data.decode('utf-8')
        request += " "
        s.send(request.encode())
        response = s.recv(1024)
        self._set_headers()
        self.wfile.write(bytes(response.decode() + "\n", "utf8"))
        return

if __name__ == '__main__':
    server_class = HTTPServer
    httpd = server_class((HOST_NAME, PORT_NUMBER), MyHandler)
    print(time.asctime(), 'Server Starts - %s:%s' % (HOST_NAME, PORT_NUMBER))
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    s.close()
print(time.asctime(), 'Server Stops - %s:%s' % (HOST_NAME, PORT_NUMBER))

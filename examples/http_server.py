#!/usr/bin/env python3
import time
import socket
from http.server import BaseHTTPRequestHandler, HTTPServer

HOST_NAME = 'localhost'
PORT_NUMBER = 8000


class MyHandler(BaseHTTPRequestHandler):
    def do_HEAD(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    def do_GET(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(("0.0.0.0", 8001))
        request = "GET " + "0.0.0.0" + self.path + " "
        s.send(request.encode())
        '''s.recv(len(request))'''
        response = s.recv(1024);
        s.close()
        print(response.decode())
        print("the end")
        return "hrllo"

    def handle_http(self, status_code, path):
        self.send_response(status_code)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        content = '''
        <html><head><title>Title goes here.</title></head>
        <body><p>This is a test.</p>
        <p>You accessed path: {}</p>
        </body></html>
        '''.format(path)
        return bytes(content, 'UTF-8')

    def respond(self, opts):
        response = self.handle_http(opts['status'], self.path)
        self.wfile.write(response)

if __name__ == '__main__':
    server_class = HTTPServer
    httpd = server_class((HOST_NAME, PORT_NUMBER), MyHandler)
    print(time.asctime(), 'Server Starts - %s:%s' % (HOST_NAME, PORT_NUMBER))
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
print(time.asctime(), 'Server Stops - %s:%s' % (HOST_NAME, PORT_NUMBER))

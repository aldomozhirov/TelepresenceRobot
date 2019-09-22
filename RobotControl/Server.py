#!/usr/bin/env python  
  
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer  
import os  
  
#Create custom HTTPRequestHandler class  
class KodeFunHTTPRequestHandler(BaseHTTPRequestHandler):  

  #handle GET command  
  def do_GET(self):  
        path = "/tmp/myfifo"
        input = self.path.replace("/", "")
        print(input)
        fifo = open(path, "w")
        fifo.write(input)
        fifo.close()
  
        #send code 200 response  
        self.send_response(200)
  
        #send header first  
        self.send_header('Content-type','text-html')  
        self.end_headers()  
  
        return

def run():  
  print('http server is starting...')  
  
  #ip and port of servr  
  #by default http server port is 8080  
  server_address = ('192.168.0.103', 8080)  
  httpd = HTTPServer(server_address, KodeFunHTTPRequestHandler)  
  print('http server is running...')  
  httpd.serve_forever()  

if __name__ == '__main__':  
  run()  
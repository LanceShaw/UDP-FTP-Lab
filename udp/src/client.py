import socket
 
size = 8192
host = '127.0.0.1'
port = 31500
addr = (host,port)
client_count = 0

try:
  while True:
    msg = str(client_count)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    msg = msg.encode(encoding="utf-8")
    sock.sendto(msg,addr)
    data,addr = sock.recvfrom(size)
    data = data.decode("utf-8")
    if data:
      print(data)
      client_count = client_count + 1
      if client_count >50:
        break
  sock.close()
 
except:
  print("cannot reach the server")
import socket

size = 8192
host = '127.0.0.1'
port = 31500
addr = (host,port)
count = 0

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(addr)

try:
  while True:
    data, address = sock.recvfrom(size)
    data = data.decode("utf-8")
    if data:
      data = str(count % 51) + ' ' + data
      count += 1
      data = data.encode(encoding="utf-8")
      sock.sendto(data.upper(), address)

finally:
  sock.close()
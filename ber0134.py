# -*- coding: utf-8 -*-

import socket

"""
# TCP
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
sock.connect(("192.168.0.1", 1989))
login = "BER0134"
jmeno = "Beránek Jakub"

arr = bytearray()
arr.append(192)
arr.append(168)
arr.append(0)
arr.append(101)

data = (login + IP)
sock.send(login.encode("ascii"))
sock.send(arr)
sock.send(jmeno.encode("utf-8"))

sock.close()"""

# UDP
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
sock.sendto("BER0134".encode("ascii"), ("192.168.0.1", 1312))
sock.close()


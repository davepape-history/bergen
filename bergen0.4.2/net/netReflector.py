#!/usr/bin/python
import sys
import socket
import struct
import time
import string


def requestStream(requestInfo,clientAddress):
	global clientDict
	print 'got request from', clientAddress, ':', requestInfo[4:]
	duration = float(string.split(requestInfo[4:])[0])
	endTime = time.time() + duration
	clientDict[clientAddress] = endTime


def broadcastData(data,sourceAddress):
	global clientDict
	now = time.time()
	for client in clientDict.keys():
		if (clientDict[client] < now):
			print client, 'timed out'
			del clientDict[client]
	for client in clientDict.keys():
		if client != sourceAddress:
			mySocket.sendto(data,client)


portNum = 5000
if (len(sys.argv) > 1):
	portNum = int(sys.argv[1])

mySocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
mySocket.bind(('',portNum))

clientDict = { }

BERGEN_NET_SAMPLEDATA = 1
BERGEN_NET_REQUESTSTREAM = 2

while 1:
	data, fromAddress = mySocket.recvfrom(65536)
	chunkType = struct.unpack('h',data[0:2])[0]
	chunkType = socket.ntohs(chunkType)
	if chunkType == BERGEN_NET_SAMPLEDATA:
		broadcastData(data,fromAddress)
	elif chunkType == BERGEN_NET_REQUESTSTREAM:
		requestStream(data,fromAddress)

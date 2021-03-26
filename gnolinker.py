import socket



class vizbridge(object):
    def __init__(self):
        pass

    def send_str(self, msg):
        self.send( bytes(str(msg), 'utf-8') )

    def send(self, msgbytes):
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = ('localhost', 2864)
        sock.connect(server_address)
        sock.sendall(msgbytes)
        sock.close()

        #sock.sendall( bytes(str(x), 'utf-8') )
        #sock.sendall(b'\x00\xC2\xA9\x00\xF0\x9D\x8C\x86\x20\xE2\x98\x83')

    def close(self):
        self.send(b'\x00')

    def open(self):
        pass

    @property
    def isopen(self):
        pass




def tcp_send(msgbytes):
    """
    # Create a TCP/IP socket
    Client:
        Create the socket.
        connect to it.
        recv data.
        If recv returns 0, it means the other end has performed an orderly shutdown. Go to step 7.
        send response.
        Go to step 3.
        Stop.

    Server:

        Create the socket.
        bind the socket to an address.
        Mark the socket as listening.
        accept a connection.
        If accepted connection is invalid, go to step 4.
        send data.
        recv response.
        If recv returns 0, it means the other end has performed an orderly shutdown. Go to step 4 to accept a new connection.
        Go to step 6.
    """




    # values = (1, b'ab', 2.7)
    # packer = struct.Struct('I 2s f')
    # packed_data = packer.pack(*values)
    # print('values =', values)

    #try:
    #    print('sending {!r}'.format(binascii.hexlify(packed_data)))
    #    sock.sendall(packed_data)
    #
    #finally:
    #     print('closing socket')
    #     sock.close()
    for x in range(1,10):
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = ('localhost', 2864)
        sock.connect(server_address)
        

        sock.sendall( bytes(str(x), 'utf-8') )
        sock.sendall(msgbytes)

        #sock.sendall(b'\x00\xC2\xA9\x00\xF0\x9D\x8C\x86\x20\xE2\x98\x83')
        sock.close()

    
    #sock.sendall(b'\x4b\x69\x73\x4b\x69\x73\x4b\x69\x73\x4b\x69\x73\x4b\x69\x73\x4b\x69\x73')
 

    ## for x in range(2,5):
    ##     print(" byte val ", x)
    ##     sock.sendall( bytes(x) )
    ##     sock.close()

    #sock.sendall(b'\x00')









"""
import binascii
import struct
MSGLEN = 2


//GO LOOK AT 
//http://www.verajankorva.com/cms/?p=57

class my_socket:
    ##demonstration class only
    ##  - coded for clarity, not efficiency
    
    def __init__(self, sock=None):
        if sock is None:
            self.sock = socket.socket(
                            socket.AF_INET, socket.SOCK_STREAM)
        else:
            self.sock = sock

    def connect(self, host, port):
        self.sock.connect((host, port))

    def send(self, msg):
        totalsent = 0
        while totalsent < MSGLEN:
            sent = self.sock.send(msg[totalsent:])
            if sent == 0:
                raise RuntimeError("socket connection broken")
            totalsent = totalsent + sent

    def receive(self):
        chunks = []
        bytes_recd = 0
        while bytes_recd < MSGLEN:
            chunk = self.sock.recv(min(MSGLEN - bytes_recd, 2048))
            if chunk == b'':
                raise RuntimeError("socket connection broken")
            chunks.append(chunk)
            bytes_recd = bytes_recd + len(chunk)
        return b''.join(chunks)


def tcp_serve():
    # create a socket object
    serversocket = socket.socket(
                socket.AF_INET, socket.SOCK_STREAM) 
    # get local machine name
    host = socket.gethostname()                           
    port = 2864                                           
    # bind to the port
    serversocket.bind((host, port))                                  
    # queue up to 5 requests
    serversocket.listen(5)                                           
    while True:
       # establish a connection
       clientsocket,addr = serversocket.accept()      
       print("Got a connection from %s" % str(addr))
       msg = 'Thank you for connecting'+ "\r\n"
       clientsocket.send(msg.encode('ascii'))
       clientsocket.close()
"""




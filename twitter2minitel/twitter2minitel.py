"""
twitter2minitel.py

Read tweets from stdin, format them
for display on a Minitel terminal, 
and pass each along to an Arduino
running serial2minitel.ino.

Tested using the ttytter.pl client.
http://www.floodgap.com/software/ttytter/

Kevin Driscoll, 2012-2013

"""
from minitel import *
import re
import serial
import time
import sys

def wait_for_OK():
    while not _minitel.inWaiting():
        # still waiting
        pass
    while _minitel.inWaiting():
        _minitel.read()

def minitel_print(s):
    MAXBYTES = 64
    count = 0
    for b in s:
        # b = b.encode('ascii','replace')
        _minitel.write(bytes(b))
        count += 1
        if (count % 64) ==0:
            wait_for_OK()

if __name__=='__main__':

    linewidth = 40 
    sleeptime = 5
    device = '/dev/ttyACM0' # Good luck, Windows/Mac lusers!

    _minitel = serial.Serial(device)

    _minitel.write(str(CLRSCR))

    # skip the ttytter intro image
    countdown = 28 
    while countdown > 0:
        line = sys.stdin.readline()
        if not line == '':
            print countdown, line
            countdown -= 1
    
    _minitel.write(str(CLRSCR))

    line = 'Retrieving tweets...'.ljust(linewidth,' ')
    print [line]
    minitel_print(line)
    time.sleep(sleeptime)

    while True:
        queue = []
        line = sys.stdin.readline()
        if line == '':
            pass
        else:
            start = line.find('<')
            s = ''
            for c in line[start:]:
                s += c
                if (len(s) % linewidth) == 0:
                    queue.append(s.strip().replace('\n','').ljust(linewidth,' '))
                    s = '' 
            if len(s) > 0:
                queue.append(s.strip().replace('\n','').ljust(linewidth,' '))
            for tweet in queue:
                print [tweet]
                minitel_print(tweet)
                time.sleep(sleeptime)

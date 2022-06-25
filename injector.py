import serial
import time

BAUD = 57600
FRAME_COUNT = 7737
HEIGHT = 64
WIDTH = 128
BPF = HEIGHT*WIDTH
FILEPATH = 'video\\original_frames\\bw.txt'
fnum = 0
ready = False
check = ''
check2 = ''
# uncommon combonations for binary images, good for headers/footers
t1 = 204 
t2 = 51 
t3 = 172 
t4 = 163 
TOP = t1.to_bytes(1, byteorder='big')  # this is the best i could come up with for a uncommon top and bottom
BOTTOM = t2.to_bytes(1, byteorder='big')
LINE_START = t3.to_bytes(1, byteorder='big')
LINE_END = t4.to_bytes(1, byteorder='big')

def split_str(s):
  return [ch for ch in s]

def bitstring_to_bytes(s):
   return int(s, 2).to_bytes((len(s) + 7) // 8, byteorder='big')

ser = serial.Serial("COM5", BAUD)
encodes = open(FILEPATH, "r") #FILEPATH
print("reading frame file...")
frames = encodes.readlines()
print("done reading frame file, initializing...")

ser.write(bytes('Initialized....', 'ASCII')) # initialize
print("  initialized. awaiting arduino...")
while (ready != True):
   check = ser.read()
   if (check == b'y'):  # 'y' is last character in arduino initialization string
      print("arduino ready. sending header...")
      ser.write(bytes('H', 'ASCII')) # initialize
      print("  awaiting response...")
   if (check == bytes('H', 'ASCII')):
      ready = True
      print("response received. sending...\n")

# transmits 1/2 a line at a time, or 32 bits, requiring 256 packets per frame
for i in range(0, FRAME_COUNT*2):
   frame = ''
   framesplit = ''
   if i % 2 == 1: # every other line in frame files is a newline and should be discarded
      continue
   fnum += 1
   frame = frames[i]
   frame = frame[:-1]
   frame_split = split_str(str(frame)) # splits frames into lines
   packet = bytearray()
   c = 0
   g = 0
   p = 0 # line number
   gemma = ''
   # 8192 is the number of pixels per frame (128x64)
   for j in range(0, BPF):
      if j >= BPF:   # unnecessary but saved me so it stays
         break
      c += 1
      gemma += frame_split[j]
      if c >= 8:
         c = 0
         packet.extend(bitstring_to_bytes(gemma))  # converts line bits to bytes
         g += 1
         gemma = ''
         if (g % 16 == 0 and j <= BPF) or j > BPF:
            # frame header
            if p == 0:
               ser.write(TOP)
            p += 1
            #time.sleep(0.05) # only necessary when not using R buffering

            # checks for response from arduino. sends back 'R' when data received and ready for more
            # print('waiting for response')
            while (check2 != bytes('R', 'ASCII')):
               check2 = ser.read()
            check2 = ''
            ser.write(LINE_START)      # line header
            ser.write(bytes(packet))   # line
            ser.write(LINE_END)        # line footer

            # frame footer
            if p == 64:
               ser.write(BOTTOM)

            print('frame ' + str(fnum) + ' line '+ str(p) + ' sent') # prints cute little frame and line numbers
            #print(packet)                                           # prints the contents of the packet
            packet = bytearray()
print("done")


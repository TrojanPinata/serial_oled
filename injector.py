import serial
import time

FRAME_COUNT = 7737
HEIGHT = 64
WIDTH = 128
BPF = HEIGHT*WIDTH
FILEPATH = 'video\\original_frames\\bw.txt'
fnum = 0
ready = False
check = ''

def split_str(s):
  return [ch for ch in s]

def bitstring_to_bytes(s):
   return int(s, 2).to_bytes((len(s) + 7) // 8, byteorder='big')

ser = serial.Serial("COM4",9600)
encodes = open('D:\Shitty Code\oled animations\sketch_jun19a\\video\\original_frames\\bw.txt', "r") #FILEPATH
frames = encodes.readlines()

ser.write(bytes('Initialized    ', 'ASCII')) # initialize
while (ready != True):
   check = ser.read()
   if (check == b's'):
      ser.write(bytes('H', 'ASCII')) # initialize
   if (check == bytes('H', 'ASCII')):
      ready = True
      print("ready")

# transmits 1/2 a line at a time, or 32 bits, requiring 256 packets per frame
for i in range(0, FRAME_COUNT*2):
   frame = ''
   framesplit = ''
   if i % 2 == 1:
      continue
   fnum += 1
   frame = frames[i]
   frame = frame[:-1]
   frame_split = split_str(str(frame))
   packet = bytearray()
   c = 0
   g = 0
   p = 0 # line number
   gemma = ''
   for j in range(0, BPF):
      if j == 8192:
         break
      c += 1
      if c < 8:
         gemma += frame_split[j]
      else:
         c = 0
         packet.extend(bitstring_to_bytes(gemma))
         g += 1
         gemma = ''
         if (g % 16 == 0 and j <= BPF) or j > BPF:
            p += 1
            time.sleep(0.032) # change based on frame rate and processor speed
            ser.write(packet)
            packet = bytearray()
            print('frame ' + str(fnum) + ' line '+ str(p) + ' sent')


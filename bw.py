import cv2

## outputs to bw.txt and bw\ subdirectory
## place all input frames (.jpg or .png) in frames subdirectory
## cut files with dain

## change to specific video
NL = True                  # for writing to file
HEIGHT = 64                # height of the frame
WIDTH = 128                # width of the frame
BPF = HEIGHT*WIDTH         # bits per frame
FRAME_COUNT = 3868         # number of frames in the video
EXTENSION = '.png'         # file extension

export = open('bw.txt', 'w')
file = 0000000000
print('starting conversion into binary data and frames')

for i in range(0, FRAME_COUNT):
   file = file + 1
   filename = 'frames\\' + str(file) + EXTENSION
   print('filename: ' + filename)
   img = cv2.imread(filename, 2)

   bw_img = cv2.threshold(img, 127, 255, cv2.THRESH_BINARY)

   newfilename = 'bw\\' + str(file) + EXTENSION
   cv2.imwrite(newfilename, bw_img[1])
   print(newfilename)
   
   vector = ''
   for j in range(0, BPF):
      k = int(j/WIDTH)
      l = int(j%WIDTH)
      color = str(bw_img[1][k][l])
      if color == '0':
         vector = vector + '0'
      elif color == '255':
         vector = vector + '1'
      else:
         print('error')
         break
   if NL:
      vector = vector + '\n'
      export.writelines(vector + '\n')
   else: 
      vector = vector
      export.writelines(vector)
   print('image ' + str(file) + ' done')

print('conversion completed') 
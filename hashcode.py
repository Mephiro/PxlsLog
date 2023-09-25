#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from hashlib import sha256
from PIL import Image, ImageDraw
from datetime import datetime
import time
import numpy as np
import matplotlib.pyplot as plt

Palette = ["#FFFFFF", "#C2CBD4", "#858D98", "#4B4F58", "#22272D", "#000000", "#38271D",
           "#6C422C", "#BC7541", "#FFB27F", "#FFD6BF", "#FEB2D9", "#F854CF", "#C785F3", 
           "#9C29BC", "#562972", "#1E1E5B", "#153FA2", "#1C95DF", "#A0E8FF", "#17A8A3", 
           "#226677", "#094C45", "#278242", "#43C91E", "#B7F954", "#FFFFAF", "#FAE70F", 
           "#FEA815", "#EA5B15", "#5A0400", "#990700", "#D81515", "#FF635E"]

print('Warning : You\'ll need a \'users_keys.txt\' file contaning at least one log key (one per line).\
     \nIf multiple log keys are written the placemap will contain the pxls of all the log keys.\n')

logfilename = input('Enter .sanit.log path without extention: ')
initcanvasfilename = input('Enter inital canvas path without extention: ')
keysfilename = input('Enter key log file path without extention: ')

outputfilename = input('Enter placemap path+filename without extenion: ') 

full_log = input('Do you want to show full canvas (take a very long time) (yes|no): ')
if (full_log == "yes"):
    full_log = True
else:
    full_log = False
    
users_keys = open(keysfilename+'.txt','r')
myfile = open('my_pixels.log', 'w')

timelapse = input('Timelapse (yes|no): ')
if timelapse == 'yes':
    timelapse = True
    croparea = input('Enter timelapse crop area (x,y,x+offx,y+offy): ')
    croparea = tuple(map(int, croparea.split(',')))
    timeBegin = input('Enter date & time of timelapse start (%Y-%m-%d %H:%M:%S): ')
    timeBegin = datetime.strptime(timeBegin,'%Y-%m-%d %H:%M:%S')
    timeBegin = int(time.mktime(timeBegin.timetuple())*1e3)
    duration_sec = int(input('Enter duration of timelapse (in sec): '))
    interval = int(input('Enter interval between frames (in sec): '))
    gif_filename = input("Enter gif filename: ")
else:
    timelapse = False
    timeBegin = 0

if full_log == False:
    user_key = users_keys.readline().strip('\n')
    j = 0
    while len(user_key) != 0:
        j += 1
        with open(logfilename+'.sanit.log') as logfile:
            for line in logfile:
                [date, random_hash, x, y, color_index, action] = line.split('\t')    
                digest_format = ','.join([date, x, y, color_index, user_key])
                digested = sha256(digest_format.encode('utf-8')).hexdigest()
                
                if digested == random_hash:
                    # This is my pixel!
                    myfile.write(line)
        
        print('Finished hashing user #'+str(j)+' !')
        user_key = users_keys.readline().strip('\n')

users_keys.close()
myfile.close()
print('Finished hashing !')

if full_log == False:
    myfile = open('my_pixels.log', 'r')
else:
    myfile = open(logfilename+'.sanit.log','r')
    
line = myfile.readline()
data = [[],[],[]]
nb_pxls=0
nb_pxls_tot=0
while len(line) != 0:
    values = line.split('\t')[0:6]
    nb_pxls_tot+=1
    chrono = datetime.strptime(values[0],'%Y-%m-%d %H:%M:%S,%f')
    data[0].append(int(time.mktime(chrono.timetuple())*1e3+chrono.microsecond*1e-3))
    data[1].append((int(values[2]),int(values[3])))
    data[2].append(int(values[4]))
    if values[5] == "user place\n":
        nb_pxls+=1
    elif values[5] == "user undo\n":
        nb_pxls-=1
    line = myfile.readline()

print("Finished extract !")
myfile.close()

sort_data = [[],[],[]]
idx_sort = np.argsort(data[0])
for i in range(nb_pxls_tot):
    for k in range(1,3):
        sort_data[k].append(data[k][idx_sort[i]])
    sort_data[0].append(data[0][idx_sort[i]]-data[0][idx_sort[0]])
    
if full_log == False:    
    coord_plot = plt.plot([x/3600000 for x in sort_data[0]],sort_data[1])
    plt.legend(coord_plot,['x','y'])
    plt.xlabel('canvas life (h)')
    plt.ylabel('pixels position')

img_gif=[]
currentTime = 0
timeBegin -= data[0][idx_sort[0]]

with Image.open(initcanvasfilename+".png") as im:
    draw = ImageDraw.Draw(im)
    for i in range(nb_pxls_tot):
        draw.point(sort_data[1][i],Palette[sort_data[2][i]])
        print("Drawing pxls : "+format(100*i/nb_pxls_tot,'.3f').zfill(7)+"%", end="\r")
        
        if timelapse == True:
            if (sort_data[0][i] >= timeBegin)&(sort_data[0][i] <= timeBegin+duration_sec*1e3):
                if (currentTime+interval*1e3 < sort_data[0][i]):
                    img_gif.append(im.crop(croparea))
                    currentTime = sort_data[0][i]

    print("\nFinished drawing !")
    print("Total number of pixels placed: "+str(nb_pxls))
    print("Total number of pixels: "+str(nb_pxls_tot))

    if timelapse == True:
        img_gif[0].save(gif_filename+".gif", save_all=True, append_images=img_gif[1:], duration=100, loop=0)
    im.save(outputfilename+".png","PNG")
    im.show()
plt.show()

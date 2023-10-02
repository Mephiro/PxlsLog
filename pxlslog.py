#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Created on Thu Sep 26 18:42:51 2023

@author: mephir0
"""

from hashlib import sha256
from PIL import Image, ImageDraw
from datetime import datetime
import time
import numpy as np
import matplotlib.pyplot as plt  
import cv2

def brightnessFunc(img,coef):
    source = img.split()
    Red = source[0].point(lambda i: i*coef)
    Green = source[1].point(lambda i: i*coef)
    Blue = source[2].point(lambda i: i*coef)
    Alpha = source[3]
    return Image.merge(img.mode,(Red,Green,Blue,Alpha))

def alphaFunc(img,coef):
    source = img.split()
    Red = source[0]
    Green = source[1]
    Blue = source[2]
    Alpha = source[3].point(lambda i: i*coef)
    return Image.merge(img.mode,(Red,Green,Blue,Alpha))

def hashFunc(usersKeys,myfile,logFilename):
    heatmapColors = "#FF0000"
    userKey = usersKeys.readline().strip('\n')
    j = 0
    while len(userKey) != 0:
        colorCheck = (userKey[0] == "#")&(len(userKey) == 7)
        if colorCheck:
            heatmapColors = userKey
            print("Heatmap color changed to "+heatmapColors)
        else:
            with open(logFilename+'.sanit.log') as logfile:
                for line in logfile:
                    [date, randomHash, x, y, colorIndex, action] = line.split('\t')    
                    digestFormat = ','.join([date, x, y, colorIndex, userKey])
                    digested = sha256(digestFormat.encode('utf-8')).hexdigest()
                    
                    if digested == randomHash:
                        # This is my pixel!
                        myfile.write(line.strip('\n')+'\t'+heatmapColors+'\n')
            j += 1
            print('Finished hashing logkey #'+str(j)+' !')
        userKey = usersKeys.readline().strip('\n')

def extractFunc(myfile,nbPxlsTot=0,nbPxls=0,nbUndos=0):
    line = myfile.readline()
    data = [[],[],[],[],[]]
    while len(line) != 0:
        values = line.split('\t')
        nbPxlsTot+=1
        chrono = datetime.strptime(values[0],'%Y-%m-%d %H:%M:%S,%f')
        data[0].append(int(time.mktime(chrono.timetuple())*1e3+chrono.microsecond*1e-3))
        data[1].append((int(values[2]),int(values[3])))
        data[2].append(int(values[4]))
        data[3].append(values[1])
        if len(values) == 7:
            data[4].append(values[6].strip('\n'))
        else:
            data[4].append("0")
        if values[5].strip('\n') == "user place":
            nbPxls+=1
        elif values[5].strip('\n') == "user undo":
            nbPxls-=1
            nbUndos+=1
        line = myfile.readline()
    return data,nbPxlsTot,nbPxls,nbUndos

def sortFunc(data,nbPxlsTot,hashCondition):
    sortData = [[],[],[],[],[]]
    idxSort = np.argsort(data[0])
    heatmapOffset = 1 if hashCondition else 0
    for i in range(nbPxlsTot):
        for k in range(1,4+heatmapOffset):
            sortData[k].append(data[k][idxSort[i]])
        sortData[0].append(data[0][idxSort[i]]-data[0][idxSort[0]])
    initTime = data[0][idxSort[0]]
    return sortData,initTime
    
def hashColorSearch(sortData1,sortData2):
    hashKey = sortData2[3]
    color = sortData2[4]
    j = 0
    for i in range(len(sortData1[3])):
        if sortData1[3][i] == hashKey[j]:
            sortData1[4][i] = color[j]
            if j < len(hashKey)-1:
                j+=1
            else:
                return sortData1
    return sortData1

Palette = ["#FFFFFF", "#C2CBD4", "#858D98", "#4B4F58", "#22272D", "#000000", "#38271D",
           "#6C422C", "#BC7541", "#FFB27F", "#FFD6BF", "#FEB2D9", "#F854CF", "#C785F3", 
           "#9C29BC", "#562972", "#1E1E5B", "#153FA2", "#1C95DF", "#A0E8FF", "#17A8A3", 
           "#226677", "#094C45", "#278242", "#43C91E", "#B7F954", "#FFFFAF", "#FAE70F", 
           "#FEA815", "#EA5B15", "#5A0400", "#990700", "#D81515", "#FF635E"]

print('Warning : You\'ll need a \'usersKeys.txt\' file contaning at least one log key (one per line).\
     \nIf multiple log keys are written the placemap will contain the pxls of all the log keys.\n')

logFilename = input('Enter .sanit.log path without extention: ') or "pixels"
initCanvasFilename = input('Enter inital canvas path without extention: ') or "canvas_initial"
keysFilename = input('Enter key log file path without extention: ') or "log_key"

outputFilename = input('Enter placemap path+filename without extenion: ') or "output"

fullLog = input('Do you want to show full canvas (take a very long time) (yes|no*): ')
if (fullLog == "yes"):
    fullLog = True
else:
    fullLog = False
    
usersKeys = open(keysFilename+'.txt','r')
myfile = open('my_pixels.log', 'w')

timelapse = input('Timelapse (yes|no*): ')
if timelapse == 'yes':
    timelapse = True
    heatmap = True if (input('Heatmap mode (yes|no*): ') == "yes") else False
    if heatmap:
        overlay = True if (input('Overlay mode (yes|no*): ') == "yes") else False
    else:
        overlay = False

    cropArea = input('Enter timelapse crop area (x,y,x+offx,y+offy): ')
    cropArea = tuple(map(int, cropArea.split(',')))
    timeBegin = input('Enter date & time of timelapse start (%Y-%m-%d %H:%M:%S): ')
    timeBegin = datetime.strptime(timeBegin,'%Y-%m-%d %H:%M:%S')
    timeBegin = int(time.mktime(timeBegin.timetuple())*1e3)
    durationSec = int(input('Enter duration of timelapse (in sec): '))
    interval = int(input('Enter interval between frames (in sec): '))
    gifVideo = input("output type : gif* or video: ") or "gif"
    gifFilename = input("Enter gif/video filename: ") or "animated"
else:
    timelapse = False
    heatmap = False
    timeBegin = 0

fullOverlay = fullLog and overlay
hashCondition = (not fullLog) or fullOverlay

if hashCondition:
    hashFunc(usersKeys,myfile,logFilename)

usersKeys.close()
myfile.close()
print('Finished hashing !')

if not fullLog:
    myfile = open('my_pixels.log', 'r')
else:
    myfile = open(logFilename+'.sanit.log','r')
    
data,nbPxlsTot,nbPxls,nbUndos = extractFunc(myfile)
myfile.close()

if fullOverlay:
    overlayFile = open('my_pixels.log', 'r')
    data2,nbPxlsTot2,nbPxls2,nbUndos2 = extractFunc(overlayFile)
    overlayFile.close()

print("Finished extract !")

sortData,initTime = sortFunc(data,nbPxlsTot,hashCondition)

if fullOverlay:
    sortData2,initTime2 = sortFunc(data2,nbPxlsTot2,hashCondition)
    sortData = hashColorSearch(sortData,sortData2)

if not fullLog:    
    coord_plot = plt.plot([x/3600000 for x in sortData[0]],sortData[1],ls='none',marker='.',markersize=1)
    plt.legend(coord_plot,['x','y'])
    plt.xlabel('canvas life (h)')
    plt.ylabel('pixels position')

timeBegin -= initTime
if timeBegin < 0:
    timeBegin = 0

timeCapture = timeBegin
frameStop = False
imgGif = []

with Image.open(initCanvasFilename+".png") as im:
    bgStatic = brightnessFunc(im, 0.3)
    heatCalque = Image.new("RGBA",im.size)
    heatDraw = ImageDraw.Draw(heatCalque)
    draw = ImageDraw.Draw(im)
    
    for i in range(nbPxlsTot):
        
        draw.point(sortData[1][i],Palette[sortData[2][i]])
        print("Drawing pxls : "+format(100*i/nbPxlsTot,'.3f').zfill(7)+"%", end="\r")           

        if timelapse:
            startTimelapse = (sortData[0][i] >= timeBegin)
            stopTimelapse = (sortData[0][i] >= timeBegin+durationSec*1e3)
        else:
            startTimelapse = False
            stopTimelapse = True
        
        if startTimelapse & (not stopTimelapse):
            if (sortData[0][i] >= timeCapture) & (not frameStop):
                frameStartIdx = i
                frameStop = True
            elif (sortData[0][i] >= timeCapture+interval*1e3) & frameStop:
                timeCapture = sortData[0][i]
                frameStopIdx = i
                frameStop = False

                if heatmap:
                   heatCalque = alphaFunc(heatCalque, 0.8)
                   heatDraw = ImageDraw.Draw(heatCalque)
                   heatPxls = sortData[1][frameStartIdx:frameStopIdx]
                   heatColors = sortData[4][frameStartIdx:frameStopIdx]
                   for j in range(len(heatPxls)):
                       if hashCondition:
                           if len(heatColors[j]) == 7:
                               heatDraw.point(heatPxls[j],heatColors[j])
                       else:
                           heatDraw.point(heatPxls[j],"#FF0000")

                   if overlay:
                       bgDynamic = brightnessFunc(im, 0.3)
                       bgDynamic.paste(heatCalque, (0,0), heatCalque)
                       imgGif.append(bgDynamic.crop(cropArea))
                   else:
                       bgStatic.paste(heatCalque, (0,0), heatCalque)
                       imgGif.append(bgStatic.crop(cropArea))
                else:
                    imgGif.append(im.crop(cropArea))

    print("\nFinished drawing !")
    print("Total number of pixels placed: "+str(nbPxls))
    print("Total number of undos: "+str(nbUndos))

    if timelapse == True:
        if gifVideo == "video":
            fourcc = cv2.VideoWriter_fourcc(*'mp4v')
            video = cv2.VideoWriter(gifFilename+".mp4",fourcc, 10,imgGif[0].size)
            for img in imgGif:
                video.write(cv2.cvtColor(np.array(img), cv2.COLOR_RGB2BGR))
            video.release()
        else:
            imgGif[0].save(gifFilename+".gif", optimize=True, save_all=True, append_images=imgGif[1:], duration=100, loop=0)
            
    im.save(outputFilename+".png","PNG")
    im.show()
plt.show()
